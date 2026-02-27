#include "target_zone_detector.h"
#include <cmath>
#include <algorithm>
#include <iostream>

TargetZoneDetector::TargetZoneDetector(int frame_width, int frame_height, int min_prep_frames)
: detector(frame_width + 1, frame_height + 1), 
  min_prep_frames(min_prep_frames), 
  prep_frames_count(0),
  geometry_initialized(false),
  current_target_center(0, 0),
  has_last_center(false)
{
    detector.SetParameters(CV_PI / 3, 3.4, 0.77);
}

std::vector<cv::RotatedRect> TargetZoneDetector::_detect(cv::Mat frame_gray) {

    // FRAME VALIDATION HERE
    std::string validation_message;
    bool frame_valid = detector.isValidFrame(frame_gray, validation_message);
    
    if (!frame_valid) {
        consecutive_invalid_frames++;
        std::cout << "Frame validation failed: " << validation_message << std::endl;
        // Return empty vector - no detection on bad frames
        return std::vector<cv::RotatedRect>();
    }
    
    // Frame is good - reset counter and proceed with detection
    consecutive_invalid_frames = 0;

    detector.run_FLED(frame_gray);
    
    //std::cout << "\n=== FLED DETECTION DEBUG ===" << std::endl;
    //std::cout << "Raw ellipses detected by FLED: " << detector.detEllipses.size() << std::endl;
    
    // Get the detected ellipses and convert to OpenCV coordinate system
    std::vector<cv::RotatedRect> ellipses;
    for (size_t idx = 0; idx < detector.detEllipses.size(); ++idx) {
        const auto& detEllipse = detector.detEllipses[idx];
        cv::RotatedRect ellipse;
        
        // Apply FLED's coordinate transformation
        ellipse.center.x = detEllipse.center.y;
        ellipse.center.y = detEllipse.center.x;
        ellipse.size.width = detEllipse.size.height;
        ellipse.size.height = detEllipse.size.width;
        ellipse.angle = -detEllipse.angle;
        
        ellipses.push_back(ellipse);
    }

    if (!ellipses.empty()) {
        // Sort by area (largest first)
        std::sort(ellipses.begin(), ellipses.end(), [](const cv::RotatedRect &a, const cv::RotatedRect &b) {
            return (a.size.width * a.size.height) > (b.size.width * b.size.height);
        });

        // Filter ellipses that are close to the largest one
        std::vector<cv::RotatedRect> filtered_ellipses;
        auto largest_ellipse = ellipses[0];
        filtered_ellipses.push_back(largest_ellipse);

        for (size_t i = 1; i < ellipses.size(); ++i) {
            auto ellipse = ellipses[i];
            double distance = cv::norm(ellipse.center - largest_ellipse.center);
            if (distance < 25) {  // Keep ellipses within 25 pixels of largest
                filtered_ellipses.push_back(ellipse);
            }
        }
        
        return filtered_ellipses;
    }
    
    return ellipses;
}

void TargetZoneDetector::prepare(cv::Mat frame, cv::Mat frame_gray) {
    auto ellipses = _detect(frame_gray);
    if (prep_frames_count < min_prep_frames)
        prep_frames_count++;
    
    // Store for backward compatibility
    target_zone_info["ellipses"] = ellipses;
    
    // Initialize geometry when preparation is complete
    if (prep_frames_count >= min_prep_frames && !ellipses.empty() && !geometry_initialized) {
        initialize_stable_geometry();
    }
}

void TargetZoneDetector::initialize_stable_geometry() {
    if (target_zone_info["ellipses"].empty()) {
        return;
    }
    
    // Use the largest ring (first after sorting) as reference center
    cv::Point2f reference_center = target_zone_info["ellipses"][0].center;
    current_target_center = reference_center;
    
    stable_ring_geometry.clear();
    
    // Default scores from outermost to innermost: 5, 6, 7, 8, 9, 10
    std::vector<int> default_scores;
    for (int i = 0; i < (int)target_zone_info["ellipses"].size(); i++) {
        default_scores.push_back(5 + i);
    }
    
    for (size_t i = 0; i < target_zone_info["ellipses"].size(); i++) {
        const auto& ellipse = target_zone_info["ellipses"][i];
        
        RingGeometry ring;
        ring.center_offset = ellipse.center - reference_center;  // Relative position (FIXED)
        ring.size = ellipse.size;                                // Fixed size
        ring.angle = ellipse.angle;                              // Fixed angle
        ring.score_value = (i < default_scores.size()) ? default_scores[i] : (5 + i);
        
        stable_ring_geometry.push_back(ring);
    }
    
    geometry_initialized = true;
    std::cout << "[GEOMETRY] Stable geometry initialized with " << stable_ring_geometry.size() << " rings" << std::endl;
}

int TargetZoneDetector::process(cv::Mat frame, cv::Mat frame_gray) {
    if (!geometry_initialized) {
        return 0;
    }
    // Simple detection - no ROI, but keep smoothing
    auto ellipses = _detect(frame_gray);
    if (ellipses.empty()) {
        return 0;
    }
    // Get detected center
    cv::Point2f detected_center = ellipses[0].center;
    
    // === SMOOTHING LOGIC ===
    cv::Point2f center_to_use = detected_center; // Default to detected
    
    if (has_last_center) {
        // Calculate movement distance
        float movement = cv::norm(detected_center - last_stable_center);
        
        // Adaptive smoothing based on movement magnitude
        float alpha = calculate_smoothing_factor(movement);
        
        // Apply exponential smoothing: new = α * detected + (1-α) * old
        center_to_use.x = alpha * detected_center.x + (1.0f - alpha) * last_stable_center.x;
        center_to_use.y = alpha * detected_center.y + (1.0f - alpha) * last_stable_center.y;
        
        // Update stable center
        last_stable_center = center_to_use;
    } else {
        // First time - initialize
        last_stable_center = detected_center;
        center_to_use = detected_center;
        has_last_center = true;
    }
    
    // Use smoothed center
    current_target_center = center_to_use;
    
    // Store ellipses for scoring
    target_zone_info["ellipses"] = ellipses;
    return stable_ring_geometry.size();
}

bool TargetZoneDetector::is_shifting() {
    return false;
}

std::vector<cv::RotatedRect> TargetZoneDetector::get_ellipses_for_scoring() {
    if (!geometry_initialized) {
        return std::vector<cv::RotatedRect>();
    }
    
    std::vector<cv::RotatedRect> scoring_ellipses;
    
    // Simple full-frame mode only
    for (const auto& ring : stable_ring_geometry) {
        cv::RotatedRect ellipse;
        ellipse.center = current_target_center + ring.center_offset;
        ellipse.size = ring.size;
        ellipse.angle = ring.angle;
        scoring_ellipses.push_back(ellipse);
    }
    
    return scoring_ellipses;
}

cv::Point2f TargetZoneDetector::get_current_target_center() {
    return current_target_center;
}

bool TargetZoneDetector::is_ready() {
    return prep_frames_count >= min_prep_frames && geometry_initialized;
}

bool TargetZoneDetector::is_geometry_initialized() {
    return geometry_initialized;
}

std::map<std::string, std::vector<cv::RotatedRect>> TargetZoneDetector::get_target_zone_info() {
    return target_zone_info;
}

void TargetZoneDetector::set_ring_scores(const std::vector<int>& scores) {
    if (scores.size() != stable_ring_geometry.size()) {
        std::cout << "Warning: Score count (" << scores.size() 
                  << ") doesn't match ring count (" << stable_ring_geometry.size() << ")" << std::endl;
        return;
    }
    
    for (size_t i = 0; i < scores.size() && i < stable_ring_geometry.size(); i++) {
        stable_ring_geometry[i].score_value = scores[i];
    }
    
    std::cout << "Updated ring scores: ";
    for (const auto& score : scores) {
        std::cout << score << " ";
    }
    std::cout << std::endl;
}

bool TargetZoneDetector::is_detection_good_enough(const std::vector<cv::RotatedRect>& ellipses) {
    if (ellipses.size() < 4) return false;  // Need at least 4 rings
    
    // Check concentricity - all rings should have similar centers
    cv::Point2f ref_center = ellipses[0].center;
    for (const auto& ellipse : ellipses) {
        float deviation = cv::norm(ellipse.center - ref_center);
        if (deviation > 15.0f) return false;  // Rings too spread out
    }
    
    return true;
}

float TargetZoneDetector::calculate_smoothing_factor(float movement) {
    // Adaptive alpha based on movement magnitude
    const float noise_threshold = 5.0f;      // Movement below this is likely noise
    const float rapid_threshold = 25.0f;     // Movement above this is definitely intentional
    const float min_alpha = 0.1f;           // Maximum smoothing (for noise)
    const float max_alpha = 0.4f;           // Minimum smoothing (for rapid movement)
    
    if (movement <= noise_threshold) {
        return min_alpha;
    } else if (movement >= rapid_threshold) {
        return max_alpha;
    } else {
        // Interpolate between min and max alpha
        float ratio = (movement - noise_threshold) / (rapid_threshold - noise_threshold);
        return min_alpha + ratio * (max_alpha - min_alpha);
    }
}
