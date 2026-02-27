#include "scorer.h"
#include <iostream>
#include <fstream>
#include <algorithm>

Scorer::Scorer(int warmup_frames, int min_hit_duration)
        : warmup_frames(warmup_frames), min_hit_duration(min_hit_duration), last_hit_time(-10000),
          warmup_count(0), total_score(0), last_score(-1),  first_comparison_timestamp(0) {
    bg_sub = cv::createBackgroundSubtractorMOG2(100, 16, false);
    _init_state();
}

void Scorer::_init_state() {
    last_hit_time = -10000;
    warmup_count = 0;
    total_score = 0;
    last_score = -1;
    last_hit_point = cv::Point();
    last_black_frame = cv::Mat();
    p = cv::Point();
    frame_buffer.clear();
}

bool Scorer::isPointInEllipse(const cv::Point &point, const cv::RotatedRect &ellipse,
                              float shrink_pixels) {
    cv::Point2f center = ellipse.center;
    float angle_rad = ellipse.angle * CV_PI / 180.0f;
    float dx = point.x - center.x;
    float dy = point.y - center.y;
    float cos_angle = cos(-angle_rad);
    float sin_angle = sin(-angle_rad);
    float x_rot = dx * cos_angle - dy * sin_angle;
    float y_rot = dx * sin_angle + dy * cos_angle;
    float a = (ellipse.size.width / 2.0f) - shrink_pixels;
    float b = (ellipse.size.height / 2.0f) - shrink_pixels;
    if (a <= 0 || b <= 0) return false;
    return (x_rot * x_rot) / (a * a) + (y_rot * y_rot) / (b * b) <= 1.0f;
}

bool Scorer::isPointOnEllipseBoundary(const cv::Point &point, const cv::RotatedRect &ellipse,
                                      float shrink_pixels) {
    cv::Point2f center = ellipse.center;
    float angle_rad = ellipse.angle * CV_PI / 180.0f;
    float dx = point.x - center.x;
    float dy = point.y - center.y;
    float cos_angle = cos(-angle_rad);
    float sin_angle = sin(-angle_rad);
    float x_rot = dx * cos_angle - dy * sin_angle;
    float y_rot = dx * sin_angle + dy * cos_angle;
    float a = (ellipse.size.width / 2.0f) - shrink_pixels;
    float b = (ellipse.size.height / 2.0f) - shrink_pixels;
    if (a <= 0 || b <= 0) return false;
    float margin = 0.0f;
    a += margin;
    b += margin;
    float normalized_dist = (x_rot * x_rot) / (a * a) + (y_rot * y_rot) / (b * b);
    return normalized_dist <= 1.0f;
}

int Scorer::getBulletDirection(cv::Point2f bullet_center, cv::Point2f target_center) {
    float dx = bullet_center.x - target_center.x;
    float dy = bullet_center.y - target_center.y;

    // Calculate angle from target center to bullet (0° = right, 90° = down)
    float angle = atan2(dy, dx) * 180.0f / CV_PI;
    if (angle < 0) angle += 360.0f;  // Normalize to 0-360°

    // Check if bullet is very close to center (within ~5 pixels)
    float distance = sqrt(dx * dx + dy * dy);
    if (distance < 5.0f) return 0;  // Center hit
    // Determine direction based on angle ranges
    if ((angle >= 345 && angle < 360) || (angle >= 0 && angle < 15))
        return 4;  // →  East
    else if (angle >= 15 && angle < 75)
        return 5;  // ↘  Southeast
    else if (angle >= 75 && angle < 105)
        return 6;  // ↓  South
    else if (angle >= 105 && angle < 165)
        return 7;  // ↙  Southwest
    else if (angle >= 165 && angle < 195)
        return 8;  // ←  West
    else if (angle >= 195 && angle < 255)
        return 1;  // ↖  Northwest
    else if (angle >= 255 && angle < 285)
        return 2;  // ↑  North
    else if (angle >= 285 && angle < 345)
        return 3;
    else /* angle >= 285 && angle < 345 */
        return 9;  // 0 orign
}

int Scorer::calculate_score(const std::vector <cv::Point> &bullet_contour,
                            const std::vector <cv::RotatedRect> &ellipses) {
    std::vector <std::pair<cv::RotatedRect, size_t>> indexed_ellipses;
    for (size_t i = 0; i < ellipses.size(); ++i) {
        indexed_ellipses.emplace_back(ellipses[i], i);
    }
    std::sort(indexed_ellipses.begin(), indexed_ellipses.end(),
              [](const auto &a, const auto &b) {
                  return (a.first.size.width * a.first.size.height) <
                         (b.first.size.width * b.first.size.height);
              });

    cv::Moments moments = cv::moments(bullet_contour);
    cv::Point2f bullet_center(moments.m10 / moments.m00, moments.m01 / moments.m00);

    int assigned_score = 0;
    for (size_t i = 0; i < indexed_ellipses.size(); ++i) {
        cv::RotatedRect ellipse = indexed_ellipses[i].first;
        int ring_score = 10 - i;
        bool boundary_touched = false;
        for (const cv::Point &point: bullet_contour) {
            if (isPointOnEllipseBoundary(point, ellipse, 0.0f)) {
                boundary_touched = true;
                assigned_score = ring_score;
                break;
            }
        }
        if (boundary_touched) {
            break;
        }
    }

    if (assigned_score == 0) {
        for (size_t i = 0; i < indexed_ellipses.size(); ++i) {
            cv::RotatedRect ellipse = indexed_ellipses[i].first;
            int ring_score = 10 - i;
            if (isPointInEllipse(bullet_center, ellipse, 0.0f)) {
                assigned_score = ring_score;
                break;
            }
        }
    }

    return assigned_score;
}

struct InvalidDetection {
    cv::Point2f center;
    double area;
    float radius;
    long timestamp;
    int count;  // How many times we've seen this same invalid detection
};

// Add these member variables to your Scorer class:
InvalidDetection last_invalid_detection;
const float SAME_INVALID_TOLERANCE = 10.0f;  // Consider it "same" if within 10px
const int MAX_SAME_INVALID_COUNT = 2;  // Reset frame after 2 same invalid detections

// ==================== STRUCTURES (Keep as is) ====================
struct CandidateDetection {
    std::vector<cv::Point> contour;
    cv::Point2f center;
    int score;
    long timestamp;
};

struct ConfirmedHit {
    cv::Point2f position;
    long timestamp;
};

std::deque<CandidateDetection> recent_detections;  // Rolling window of last 3 detections
std::vector<ConfirmedHit> confirmed_hits;  // Track all confirmed bullet positions
const int REQUIRED_DETECTION_FRAMES = 2;  // Must appear in 3 consecutive frames
const float POSITION_TOLERANCE = 10.0f;  // Pixels - same hole should be within this distance
const long CONFIRMED_HIT_MEMORY = 1000;  // Remember confirmed hits for 3 seconds

// ==================== MAIN PROCESS FUNCTION ====================
int Scorer::process(
        cv::Mat frame,
        cv::Mat frame_gray,
        std::map <std::string, std::vector<cv::RotatedRect>> target_zone,
        long timestamp, bool is_ble) {

    std::vector <cv::RotatedRect> ellipses = target_zone["ellipses"];
    if (ellipses.empty()) {
        return -1;
    }

    cv::RotatedRect main_ellipse = ellipses[0];
    cv::Point target_center(static_cast<int>(main_ellipse.center.x),
                            static_cast<int>(main_ellipse.center.y));

    if (frame.empty() || frame_gray.empty()) {
        return -1;
    }

    // Store first frame for 500ms comparison
    if (first_comparison_frame.empty()) {
        first_comparison_frame = frame_gray.clone();
        first_comparison_timestamp = timestamp;
    }

    cv::Mat frame_hsv;
    cv::cvtColor(frame, frame_hsv, cv::COLOR_BGR2HSV);
    cv::Mat black_mask;
    cv::inRange(frame_hsv, cv::Scalar(0, 0, 0), cv::Scalar(180, 255, 120), black_mask);

    cv::Mat blur;
    cv::GaussianBlur(frame_gray, blur, cv::Size(11, 11), 0);
    cv::Mat motion;
    bg_sub->apply(blur, motion);

    warmup_count++;
    if (warmup_count < warmup_frames) {
        return -1;
    }

    cv::Mat crop_black_mask = black_mask;
    cv::Mat crop_motion = motion;
    cv::Mat new_bullet_mask;

    if (last_black_frame.empty()) {
        new_bullet_mask = cv::Mat::zeros(crop_black_mask.size(), crop_black_mask.type());
    } else {
        if (last_black_frame.rows != crop_black_mask.rows ||
            last_black_frame.cols != crop_black_mask.cols) {
            last_black_frame = crop_black_mask;
            return -1;
        }
        cv::Mat joint_mask;
        cv::bitwise_and(last_black_frame, crop_black_mask, joint_mask);
        cv::bitwise_xor(crop_black_mask, joint_mask, new_bullet_mask);
    }

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::morphologyEx(new_bullet_mask, new_bullet_mask, cv::MORPH_OPEN, kernel);
    cv::dilate(new_bullet_mask, new_bullet_mask, kernel, cv::Point(-1, -1), 1);

    last_black_frame = crop_black_mask.clone();

    cv::morphologyEx(crop_motion, crop_motion, cv::MORPH_OPEN, kernel);
    cv::Mat joint_crop_motion;

    if (crop_motion.rows < crop_black_mask.rows || crop_motion.cols > crop_black_mask.cols) {
        cv::Rect crop_rect(0, 0, crop_motion.cols, crop_motion.rows);
        crop_black_mask = crop_black_mask(crop_rect);
    } else if (crop_motion.rows > crop_black_mask.rows || crop_motion.cols > crop_black_mask.cols) {
        cv::Rect crop_rect(0, 0, crop_black_mask.cols, crop_black_mask.rows);
        crop_motion = crop_motion(crop_rect);
    }

    bool has_issue = false;
    std::string issue_details = "";
    if (crop_motion.empty()) {
        has_issue = true;
        issue_details += "crop_motion=EMPTY ";
    } else if (cv::countNonZero(crop_motion) == 0) {
        has_issue = true;
        issue_details += "crop_motion=ALL_ZERO ";
    }
    if (crop_black_mask.empty()) {
        has_issue = true;
        issue_details += "crop_black_mask=EMPTY ";
    } else if (cv::countNonZero(crop_black_mask) == 0) {
        has_issue = true;
        issue_details += "crop_black_mask=ALL_ZERO ";
    }
    if (new_bullet_mask.empty()) {
        has_issue = true;
        issue_details += "new_bullet_mask=EMPTY ";
    } else if (cv::countNonZero(new_bullet_mask) == 0) {
        has_issue = true;
        issue_details += "new_bullet_mask=ALL_ZERO ";
    }
    if (!crop_motion.empty() && !crop_black_mask.empty() && crop_motion.size() != crop_black_mask.size()) {
        has_issue = true;
        issue_details += "SIZE_MISMATCH(motion-black) ";
    }

    cv::bitwise_and(crop_motion, crop_black_mask, joint_crop_motion);

    std::vector<std::vector<cv::Point>> origin_contours;
    cv::findContours(crop_motion, origin_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(crop_motion.clone(), contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    if (origin_contours.size() > 0 && contours.empty()) {
        //platform_log("[BULLET_MISS] Motion detected but no valid contours - Issue: %s",
        //             issue_details.empty() ? "joint_crop_motion=ZERO_PIXELS" : issue_details.c_str());
    }

    bool time_cond = (timestamp - last_hit_time > min_hit_duration * 500);

    // Clean up old confirmed hits (older than CONFIRMED_HIT_MEMORY)
    confirmed_hits.erase(
            std::remove_if(confirmed_hits.begin(), confirmed_hits.end(),
                           [timestamp](const ConfirmedHit& hit) {
                               return (timestamp - hit.timestamp) > CONFIRMED_HIT_MEMORY;
                           }),
            confirmed_hits.end()
    );

    // ==================== 500MS FRAME COMPARISON WITH 3-FRAME ROLLING WINDOW ====================
    if (!first_comparison_frame.empty() &&
        (timestamp - first_comparison_timestamp >= 250) &&
        (timestamp - last_hit_time >= 250)) {

        // Step 1: Calculate motion between stored frame and current frame
        cv::Mat comparison_blur_first, comparison_blur_current;
        cv::GaussianBlur(first_comparison_frame, comparison_blur_first, cv::Size(11, 11), 0);
        cv::GaussianBlur(frame_gray, comparison_blur_current, cv::Size(11, 11), 0);

        cv::Mat comparison_motion;
        cv::absdiff(comparison_blur_first, comparison_blur_current, comparison_motion);
        cv::threshold(comparison_motion, comparison_motion, 25, 255, cv::THRESH_BINARY);

        // Step 2: Create new_bullet_mask for 500ms comparison
        cv::Mat comp_frame_hsv;
        cv::cvtColor(frame, comp_frame_hsv, cv::COLOR_BGR2HSV);
        cv::Mat comp_black_mask;
        cv::inRange(comp_frame_hsv, cv::Scalar(0, 0, 0), cv::Scalar(180, 255, 120), comp_black_mask);

        cv::Mat comp_new_bullet_mask;
        if (!last_black_frame.empty() &&
            last_black_frame.rows == comp_black_mask.rows &&
            last_black_frame.cols == comp_black_mask.cols) {
            cv::Mat comp_joint_mask;
            cv::bitwise_and(last_black_frame, comp_black_mask, comp_joint_mask);
            cv::bitwise_xor(comp_black_mask, comp_joint_mask, comp_new_bullet_mask);
        } else {
            comp_new_bullet_mask = cv::Mat::zeros(comp_black_mask.size(), comp_black_mask.type());
        }

        // Step 3: Apply morphology operations
        cv::Mat comp_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
        cv::morphologyEx(comp_new_bullet_mask, comp_new_bullet_mask, cv::MORPH_OPEN, comp_kernel);
        cv::dilate(comp_new_bullet_mask, comp_new_bullet_mask, comp_kernel, cv::Point(-1, -1), 1);
        cv::morphologyEx(comparison_motion, comparison_motion, cv::MORPH_OPEN, comp_kernel);

        // Step 4: Ensure size compatibility
        cv::Mat comp_crop_black_mask = comp_black_mask;
        cv::Mat comp_crop_motion = comparison_motion;

        if (comp_crop_motion.rows < comp_crop_black_mask.rows || comp_crop_motion.cols > comp_crop_black_mask.cols) {
            cv::Rect crop_rect(0, 0, comp_crop_motion.cols, comp_crop_motion.rows);
            comp_crop_black_mask = comp_crop_black_mask(crop_rect);
        } else if (comp_crop_motion.rows > comp_crop_black_mask.rows || comp_crop_motion.cols > comp_crop_black_mask.cols) {
            cv::Rect crop_rect(0, 0, comp_crop_black_mask.cols, comp_crop_black_mask.rows);
            comp_crop_motion = comp_crop_motion(crop_rect);
        }

        // Step 5: Apply black mask
        cv::Mat comp_joint_crop_motion;
        cv::bitwise_and(comp_crop_motion, comp_crop_black_mask, comp_joint_crop_motion);

        // Step 6: Find contours
        std::vector<std::vector<cv::Point>> comp_origin_contours;
        cv::findContours(comp_crop_motion, comp_origin_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

        std::vector<std::vector<cv::Point>> comp_contours;
        cv::findContours(comp_joint_crop_motion, comp_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        if (!comp_contours.empty()) {
            // Sort contours by area
            std::vector<std::vector<cv::Point>> comp_sorted_contours = comp_contours;
            std::sort(comp_sorted_contours.begin(), comp_sorted_contours.end(),
                      [](const std::vector<cv::Point> &a, const std::vector<cv::Point> &b) {
                          return cv::contourArea(a) > cv::contourArea(b);
                      });
            comp_sorted_contours.resize(std::min(comp_sorted_contours.size(), size_t(3)));

            std::vector<cv::Point> &comp_bullet_contour = comp_sorted_contours[0];

            // ==================== VALIDATION 1: CHECK AREA ====================
            double comp_area = cv::contourArea(comp_bullet_contour);
            if (comp_area < 5 || comp_area > 1000) {
                //platform_log("[500MS_AREA] Invalid area: %.1f", comp_area);

                // Calculate center for this invalid detection
                cv::Moments comp_moments_temp = cv::moments(comp_bullet_contour);
                cv::Point2f comp_center_invalid(comp_moments_temp.m10 / comp_moments_temp.m00,
                                                comp_moments_temp.m01 / comp_moments_temp.m00);

                // Check if this is the same invalid detection as before
                bool is_same_invalid = false;
                if (last_invalid_detection.count > 0) {
                    float distance = cv::norm(comp_center_invalid - last_invalid_detection.center);
                    float area_diff = std::abs(comp_area - last_invalid_detection.area);

                    if (distance < SAME_INVALID_TOLERANCE && area_diff < 50) {
                        is_same_invalid = true;
                        last_invalid_detection.count++;
                        //platform_log("[500MS_AREA] Same invalid detection #%d (distance: %.1fpx)",
                        //             last_invalid_detection.count, distance);
                    }
                }

                if (!is_same_invalid) {
                    // New invalid detection - start tracking it
                    last_invalid_detection.center = comp_center_invalid;
                    last_invalid_detection.area = comp_area;
                    last_invalid_detection.timestamp = timestamp;
                    last_invalid_detection.count = 1;
                }

                // If we've seen the same invalid detection MAX_SAME_INVALID_COUNT times, reset frame
                if (last_invalid_detection.count >= MAX_SAME_INVALID_COUNT) {
                    //platform_log("[500MS_AREA] Same invalid area detected %d times - updating comparison frame",
                    //             last_invalid_detection.count);
                    first_comparison_frame = frame_gray.clone();
                    last_invalid_detection.count = 0;  // Reset counter
                }

                first_comparison_timestamp = timestamp;
                return -1;
            }

            // ==================== VALIDATION 2: CHECK RADIUS ====================
            cv::Point2f comp_center;
            float comp_radius;
            cv::minEnclosingCircle(comp_bullet_contour, comp_center, comp_radius);

            if (comp_radius < 1.0 || comp_radius > 15.0) {
                //platform_log("[500MS_RADIUS] Invalid comp_radius: %.1f", comp_radius);

                // Check if this is the same invalid detection as before
                bool is_same_invalid = false;
                if (last_invalid_detection.count > 0) {
                    float distance = cv::norm(comp_center - last_invalid_detection.center);
                    float radius_diff = std::abs(comp_radius - last_invalid_detection.radius);

                    if (distance < SAME_INVALID_TOLERANCE && radius_diff < 2.0f) {
                        is_same_invalid = true;
                        last_invalid_detection.count++;
                        //platform_log("[500MS_RADIUS] Same invalid detection #%d (distance: %.1fpx)",
                        //             last_invalid_detection.count, distance);
                    }
                }

                if (!is_same_invalid) {
                    // New invalid detection - start tracking it
                    last_invalid_detection.center = comp_center;
                    last_invalid_detection.radius = comp_radius;
                    last_invalid_detection.timestamp = timestamp;
                    last_invalid_detection.count = 1;
                }

                // If we've seen the same invalid detection MAX_SAME_INVALID_COUNT times, reset frame
                if (last_invalid_detection.count >= MAX_SAME_INVALID_COUNT) {
                    //platform_log("[500MS_RADIUS] Same invalid radius detected %d times - updating comparison frame",
                    //             last_invalid_detection.count);
                    first_comparison_frame = frame_gray.clone();
                    last_invalid_detection.count = 0;  // Reset counter
                }

                first_comparison_timestamp = timestamp;
                return -1;
            }

            // ==================== VALIDATION 3: CALCULATE AND CHECK BULLET RADIUS ====================
            double comp_contour_area = cv::contourArea(comp_bullet_contour);
            float comp_radius_area = std::sqrt(comp_contour_area / CV_PI);
            float comp_radius_enclose;
            cv::minEnclosingCircle(comp_bullet_contour, comp_center, comp_radius_enclose);
            float comp_bullet_radius;
            if (comp_radius_area > comp_radius_enclose * 1.1) {
                comp_bullet_radius = comp_radius_enclose;
            } else if (comp_radius_enclose > comp_radius_area * 1.1) {
                comp_bullet_radius = comp_radius_area;
            } else {
                comp_bullet_radius = (comp_radius_enclose + comp_radius_area) / 2.0f;
            }

            if (comp_bullet_radius < 1.0) {
                //platform_log("[500MS_RADIUS] Invalid bullet_radius: %.1f", comp_bullet_radius);

                // Check if this is the same invalid detection as before
                bool is_same_invalid = false;
                if (last_invalid_detection.count > 0) {
                    float distance = cv::norm(comp_center - last_invalid_detection.center);
                    float radius_diff = std::abs(comp_bullet_radius - last_invalid_detection.radius);

                    if (distance < SAME_INVALID_TOLERANCE && radius_diff < 2.0f) {
                        is_same_invalid = true;
                        last_invalid_detection.count++;
                        //platform_log("[500MS_RADIUS] Same invalid detection #%d (distance: %.1fpx)",
                        //             last_invalid_detection.count, distance);
                    }
                }

                if (!is_same_invalid) {
                    // New invalid detection - start tracking it
                    last_invalid_detection.center = comp_center;
                    last_invalid_detection.radius = comp_bullet_radius;
                    last_invalid_detection.timestamp = timestamp;
                    last_invalid_detection.count = 1;
                }

                // If we've seen the same invalid detection MAX_SAME_INVALID_COUNT times, reset frame
                if (last_invalid_detection.count >= MAX_SAME_INVALID_COUNT) {
                    //platform_log("[500MS_RADIUS] Same invalid bullet_radius detected %d times - updating comparison frame",
                    //             last_invalid_detection.count);
                    
                    first_comparison_frame = frame_gray.clone();
                    last_invalid_detection.count = 0;  // Reset counter
                }

                first_comparison_timestamp = timestamp;
                return -1;
            }

            // ==================== CALCULATE BULLET CENTER ====================
            cv::Moments comp_moments = cv::moments(comp_bullet_contour);
            cv::Point2f comp_bullet_center(comp_moments.m10 / comp_moments.m00,
                                           comp_moments.m01 / comp_moments.m00);

            // ==================== VALIDATION 4: CHECK SCORE ====================
            int temp_score = calculate_score(comp_bullet_contour, ellipses);

            if (temp_score == 0) {
                //platform_log("[500MS] Score is 0, ignoring detection");

                // Check if this is the same invalid detection as before
                bool is_same_invalid = false;
                if (last_invalid_detection.count > 0) {
                    float distance = cv::norm(comp_bullet_center - last_invalid_detection.center);

                    if (distance < SAME_INVALID_TOLERANCE) {
                        is_same_invalid = true;
                        last_invalid_detection.count++;
                        //platform_log("[500MS_SCORE] Same zero-score detection #%d (distance: %.1fpx)",
                        //             last_invalid_detection.count, distance);
                    }
                }

                if (!is_same_invalid) {
                    // New invalid detection - start tracking it
                    last_invalid_detection.center = comp_bullet_center;
                    last_invalid_detection.timestamp = timestamp;
                    last_invalid_detection.count = 1;
                }

                // If we've seen the same invalid detection MAX_SAME_INVALID_COUNT times, reset frame
                if (last_invalid_detection.count >= MAX_SAME_INVALID_COUNT) {
                    //platform_log("[500MS_SCORE] Same zero-score detected %d times - updating comparison frame",
                    //             last_invalid_detection.count);
                    first_comparison_frame = frame_gray.clone();
                    last_invalid_detection.count = 0;  // Reset counter
                }

                first_comparison_timestamp = timestamp;
                return -1;
            }

            // ==================== VALID DETECTION - RESET INVALID COUNTER ====================
            last_invalid_detection.count = 0;

            // ==================== 3-FRAME ROLLING WINDOW LOGIC ====================

            // Add current detection to rolling window
            CandidateDetection new_detection;
            new_detection.contour = comp_bullet_contour;
            new_detection.center = comp_bullet_center;
            new_detection.score = temp_score;
            new_detection.timestamp = timestamp;

            recent_detections.push_back(new_detection);

            // Keep only last 3 detections
            if (recent_detections.size() > REQUIRED_DETECTION_FRAMES) {
                recent_detections.pop_front();
            }

            //platform_log("[3-FRAME] Added detection, window size: %zu/%d",
            //             recent_detections.size(), REQUIRED_DETECTION_FRAMES);

            // Check if we have 3 detections in the window
            if (recent_detections.size() == REQUIRED_DETECTION_FRAMES) {
                // Check if all 3 detections are for the same hole (within position tolerance)
                bool all_match = true;
                cv::Point2f reference_center = recent_detections[0].center;

                for (size_t i = 1; i < recent_detections.size(); ++i) {
                    float distance = cv::norm(recent_detections[i].center - reference_center);
                    //platform_log("[3-FRAME] Frame %zu distance from frame 0: %.1fpx", i, distance);

                    if (distance > POSITION_TOLERANCE) {
                        all_match = false;
                        //platform_log("[3-FRAME] ✗ Detection mismatch - distances too large");
                        break;
                    }
                }

                if (all_match) {
                    //platform_log("[3-FRAME] ✓ CONFIRMED - Same hole detected in all 3 frames!");

                    // Use majority voting for the final score from all 3 detections
                    std::map<int, int> score_counts;
                    for (const auto& det : recent_detections) {
                        score_counts[det.score]++;
                    }

                    int comp_assigned_score = 0;
                    int comp_max_count = 0;
                    for (const auto &pair : score_counts) {
                        if (pair.second > comp_max_count) {
                            comp_assigned_score = pair.first;
                            comp_max_count = pair.second;
                        }
                    }

                    // If no clear majority, use the most recent score
                    if (comp_max_count <= 1) {
                        comp_assigned_score = recent_detections.back().score;
                    }

                    //platform_log("[3-FRAME] Final score: %d (from %d/%zu votes)",
                    //             comp_assigned_score, comp_max_count, recent_detections.size());

                    // Add this position to confirmed hits to prevent re-detection
                    ConfirmedHit new_hit;
                    new_hit.position = comp_bullet_center;
                    new_hit.timestamp = timestamp;
                    confirmed_hits.push_back(new_hit);

                    // Keep only recent confirmed hits (limit to last 20)
                    if (confirmed_hits.size() > 20) {
                        confirmed_hits.erase(confirmed_hits.begin(),
                                             confirmed_hits.begin() + (confirmed_hits.size() - 20));
                    }

                    // NOW commit all the changes (frame buffer, history, etc.)
                    FrameData comp_frame_data;
                    comp_frame_data.frame = frame.clone();
                    comp_frame_data.ellipses = ellipses;
                    comp_frame_data.bullet_contour = comp_bullet_contour;
                    comp_frame_data.timestamp = timestamp;
                    frame_buffer.push_back(comp_frame_data);
                    if (frame_buffer.size() > MAX_FRAMES) {
                        frame_buffer.pop_front();
                    }

                    last_hit_time = timestamp;
                    history.push_back(comp_bullet_contour);
                    last_hit_point = comp_center;

                    // Calculate direction
                    int comp_direction = getBulletDirection(comp_bullet_center,
                                                            cv::Point2f(target_center.x, target_center.y));

                    // Update tracking variables
                    last_score = comp_assigned_score;
                    total_score += comp_assigned_score;
                    score_history.push_back(comp_assigned_score);
                    direction_history.push_back(comp_direction);

                    // Clear the rolling window after successful commit
                    recent_detections.clear();

                    // Reset comparison frame
                    first_comparison_frame.release();
                    first_comparison_timestamp = 0;
                    first_comparison_frame = frame_gray.clone();
                    first_comparison_timestamp = timestamp;

                    return comp_assigned_score;
                }
                else {
                    //platform_log("[3-FRAME] Detections don't match - continuing to accumulate");
                    // Don't reset - keep the window and continue accumulating
                    first_comparison_timestamp = timestamp;
                    return -1;
                }
            }
            else {
                //platform_log("[3-FRAME] Need more frames: %zu/%d", recent_detections.size(), REQUIRED_DETECTION_FRAMES);
                first_comparison_timestamp = timestamp;
                // Keep accumulating detections
                return -1;
            }
        }

        // Reset comparison frame for next cycle if no valid detection
        first_comparison_frame = frame_gray.clone();
        first_comparison_timestamp = timestamp;
    }

    return -1;
}



std::vector <std::vector<cv::Point>> Scorer::get_history_for_display(cv::Rect roi) {
    if (roi.empty()) {
        return history;
    }
    std::vector <std::vector<cv::Point>> display_history;
    for (const auto &shot: history) {
        std::vector <cv::Point> display_shot;
        for (const auto &point: shot) {
            display_shot.push_back(cv::Point(point.x + roi.x, point.y + roi.y));
        }
        display_history.push_back(display_shot);
    }
    return display_history;
}
