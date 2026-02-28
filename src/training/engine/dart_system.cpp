#include "dart_system.h"
#include "target_zone_detector.h"
#include "scorer.h"
#include <iostream>
#include <cstring>
#include <sys/stat.h>
#include <chrono>

// ============================================================================
// STATIC GLOBALS (hidden from outside)
// ============================================================================
static cv::VideoCapture g_cap;
static TargetZoneDetector* g_tz_detector = nullptr;
static TargetZoneDetector* g_calibration_detector = nullptr;
static Scorer* g_scorer = nullptr;

static bool g_initialized = false;
static bool g_paused = false;
static bool g_calibration_confirmed = false;

static int g_frame_width = 600;
static int g_frame_height = 600;
static int g_crop_size = 600;
static int g_frame_idx = 0;
static int g_shot_count = 0;

// Calibration state
static std::vector<cv::RotatedRect> g_last_detected_ellipses;
static std::vector<cv::RotatedRect> g_user_approved_geometry;
static bool g_last_setup_valid = false;
static int g_calibration_check_counter = 0;
static std::string g_session_folder;


struct PersistentShot {
    cv::Point display_point;
    int shot_number;
    int score;
    int direction;
    std::vector<cv::Point> contour;
};
static std::vector<PersistentShot> session_history;


// Constants
static const int CALIBRATION_CHECK_INTERVAL = 20;
static const float MAX_ASPECT_RATIO = 1.04f;
static const int MIN_RINGS_REQUIRED = 6;

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================
static cv::Mat crop_center(const cv::Mat& frame, int crop_size) {
    int x = (frame.cols - crop_size) / 2;
    int y = (frame.rows - crop_size) / 2;
    x = std::max(0, x);
    y = std::max(0, y);
    int w = std::min(crop_size, frame.cols - x);
    int h = std::min(crop_size, frame.rows - y);
    return frame(cv::Rect(x, y, w, h)).clone();
}

static bool is_setup_good(const std::vector<cv::RotatedRect>& ellipses) {
    if (ellipses.size() < MIN_RINGS_REQUIRED) return false;
    
    for (const auto& ellipse : ellipses) {
        float aspect = std::max(ellipse.size.width, ellipse.size.height) / 
                       std::min(ellipse.size.width, ellipse.size.height);
        if (aspect > MAX_ASPECT_RATIO) return false;
    }
    return true;
}

// ============================================================================
// CAMERA DISCOVERY
// ============================================================================
std::vector<CameraInfo> get_available_cameras() {
    std::vector<CameraInfo> cameras;
    
    for (int i = 0; i < 10; i++) {
        cv::VideoCapture test_cap(i, cv::CAP_V4L2);
        if (test_cap.isOpened()) {
            cv::Mat test_frame;
            if (test_cap.read(test_frame) && !test_frame.empty()) {
                CameraInfo info;
                info.index = i;
                info.width = (int)test_cap.get(cv::CAP_PROP_FRAME_WIDTH);
                info.height = (int)test_cap.get(cv::CAP_PROP_FRAME_HEIGHT);
                
                // Get camera name via v4l2-ctl
                info.name = "Camera " + std::to_string(i);
                std::string cmd = "v4l2-ctl --device=/dev/video" + std::to_string(i) + " --info 2>/dev/null";
                FILE* pipe = popen(cmd.c_str(), "r");
                if (pipe) {
                    char buffer[256];
                    while (fgets(buffer, sizeof(buffer), pipe)) {
                        if (strstr(buffer, "Card type")) {
                            char* pos = strchr(buffer, ':');
                            if (pos) {
                                info.name = pos + 2;
                                size_t end = info.name.find_last_not_of(" \t\n\r");
                                if (end != std::string::npos) {
                                    info.name = info.name.substr(0, end + 1);
                                }
                            }
                            break;
                        }
                    }
                    pclose(pipe);
                }
                
                cameras.push_back(info);
            }
            test_cap.release();
        }
    }
    
    return cameras;
}

// ============================================================================
// SYSTEM INIT
// ============================================================================
bool init_system(int camera_index, int crop_size) {
    // Cleanup any existing state
    cleanup_system();
    
    // Open camera
    g_cap.open(camera_index, cv::CAP_V4L2);
    if (!g_cap.isOpened()) {
        std::cerr << "Failed to open camera " << camera_index << std::endl;
        return false;
    }
    
    g_cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    g_cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
    
    // Verify camera works
    cv::Mat test_frame;
    if (!g_cap.read(test_frame) || test_frame.empty()) {
        std::cerr << "Failed to read from camera" << std::endl;
        g_cap.release();
        return false;
    }
    
    // Set dimensions
    g_crop_size = crop_size;
    g_frame_width = crop_size;
    g_frame_height = crop_size;
    
    // Create detectors and scorer
    g_tz_detector = new TargetZoneDetector(g_frame_height, g_frame_width);
    g_calibration_detector = new TargetZoneDetector(g_frame_height, g_frame_width, 1);
    g_scorer = new Scorer();
    
    // Reset state
    g_initialized = true;
    g_paused = false;
    g_calibration_confirmed = false;
    g_last_setup_valid = false;
    g_calibration_check_counter = 0;
    g_frame_idx = 0;
    g_shot_count = 0;
    g_last_detected_ellipses.clear();
    g_user_approved_geometry.clear();
    session_history.clear();
    
    std::cout << "System initialized: " << g_frame_width << "x" << g_frame_height << std::endl;
    return true;
}

// ============================================================================
// Create Session Folder
// ============================================================================
static std::string create_session_folder() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::tm* tm = std::localtime(&time);
    char buf[64];
    snprintf(buf, sizeof(buf), "session_%04d%02d%02d_%02d%02d%02d_%03d",
        tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
        tm->tm_hour, tm->tm_min, tm->tm_sec, (int)ms.count());
    
    std::string folder(buf);
    mkdir(folder.c_str(), 0755);
    return folder;
}

// ============================================================================
// MAIN PROCESSING
// ============================================================================
FrameResult process_frame(int mode) {
    FrameResult result;
    result.has_new_shot = false;
    result.shot.score = -1;
    result.shot.direction = 0;
    result.shot.shot_number = g_shot_count;
    result.shot.total_score = g_scorer ? g_scorer->get_total_score() : 0;
    
    if (!g_initialized) {
        std::cerr << "System not initialized!" << std::endl;
        return result;
    }
    
    if (g_paused) {
        // Return last frame or empty
        return result;
    }
    
    // Capture frame
    cv::Mat full_frame;
    if (!g_cap.read(full_frame) || full_frame.empty()) {
        std::cerr << "Failed to capture frame" << std::endl;
        return result;
    }
    
    // Crop to center
    cv::Mat frame = crop_center(full_frame, g_crop_size);
    cv::Mat frame_gray;
    cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
    
    g_frame_idx++;
    
    // Process based on mode
    switch (mode) {
        case MODE_STREAMING: {
            // Just return the cropped frame, no overlay
            break;
        }
        
        case MODE_CALIBRATION: {
            // Calibration text
            cv::putText(frame, "CALIBRATION - Adjust camera angle", 
                       cv::Point(20, 30), cv::FONT_HERSHEY_SIMPLEX, 0.6, 
                       cv::Scalar(0, 255, 255), 2);
            
            // Periodic detection check
            g_calibration_check_counter++;
            if (g_calibration_check_counter >= CALIBRATION_CHECK_INTERVAL) {
                g_calibration_check_counter = 0;
                g_last_detected_ellipses = g_calibration_detector->_detect(frame_gray);
                g_last_setup_valid = is_setup_good(g_last_detected_ellipses);
                
                if (g_last_setup_valid) {
                    g_user_approved_geometry = g_last_detected_ellipses;
                }
            }
            
            // Draw rings
            cv::Scalar color = g_last_setup_valid ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);
            for (const auto& ellipse : g_last_detected_ellipses) {
                cv::ellipse(frame, ellipse, color, 1);
            }
            
            // Status text
            std::string status;
            if (g_last_setup_valid) {
                status = "READY - Press SPACE to start scoring";
            } else if (g_last_detected_ellipses.size() < MIN_RINGS_REQUIRED) {
                status = "Need " + std::to_string(MIN_RINGS_REQUIRED) + "+ rings, found " + 
                         std::to_string(g_last_detected_ellipses.size());
            } else {
                status = "Adjust camera angle - rings too elliptical";
            }
            cv::putText(frame, status, cv::Point(20, frame.rows - 20), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.5, color, 1);
            break;
        }
        
        case MODE_SCORING: {
    if (!g_calibration_confirmed || !g_tz_detector->is_ready()) {
        cv::putText(frame, "NOT CALIBRATED - Press 1 for calibration", 
                   cv::Point(20, 30), cv::FONT_HERSHEY_SIMPLEX, 0.6, 
                   cv::Scalar(0, 0, 255), 2);
        break;
    }
    
    int numEllipses = g_tz_detector->process(frame, frame_gray);
    if (numEllipses <= 0) {
        cv::putText(frame, "TARGET LOST - Reposition camera", 
                   cv::Point(20, 30), cv::FONT_HERSHEY_SIMPLEX, 0.6, 
                   cv::Scalar(0, 0, 255), 2);
        break;
    }
    
    std::vector<cv::RotatedRect> scoring_ellipses = g_tz_detector->get_ellipses_for_scoring();
    if (scoring_ellipses.empty()) {
        cv::putText(frame, "NO RINGS DETECTED", 
                   cv::Point(20, 30), cv::FONT_HERSHEY_SIMPLEX, 0.6, 
                   cv::Scalar(0, 0, 255), 2);
        break;
    }
    
    // Check if target is moving
    if (g_tz_detector->is_shifting()) {
        cv::putText(frame, "TARGET MOVING - DETECTION PAUSED", 
                   cv::Point(50, 150), cv::FONT_HERSHEY_SIMPLEX, 0.8, 
                   cv::Scalar(0, 165, 255), 2);
        break;
    }
    
    // Build target_zone map for scorer
    std::map<std::string, std::vector<cv::RotatedRect>> target_zone;
    target_zone["ellipses"] = scoring_ellipses;
    
    // Process scoring
    long timestamp = g_frame_idx * 33;
    size_t history_before = g_scorer->get_history().size();
    int score = g_scorer->process(frame, frame_gray, target_zone, timestamp, false);
    size_t history_after = g_scorer->get_history().size();
    
    // Check for new shot
    if (history_after > history_before && score >= 0) {
        g_shot_count++;
        
        result.has_new_shot = true;
        result.shot.score = score;
        result.shot.direction = g_scorer->get_last_direction();
        result.shot.shot_number = g_shot_count;
        result.shot.total_score = g_scorer->get_total_score();
        
        // Store shot in session history
        const auto& scorer_history = g_scorer->get_history();
        if (!scorer_history.empty()) {
            const auto& latest_contour = scorer_history.back();
            if (!latest_contour.empty()) {
                cv::Point2f center_f;
                float radius = 0.f;
                cv::minEnclosingCircle(latest_contour, center_f, radius);
                
                PersistentShot shot;
                shot.display_point = cv::Point(static_cast<int>(center_f.x), static_cast<int>(center_f.y));
                shot.shot_number = g_shot_count;
                shot.score = score;
                shot.direction = g_scorer->get_last_direction();
                shot.contour = latest_contour;
                session_history.push_back(shot);
            }
        }
        
        std::cout << "Shot #" << g_shot_count << " | Score: " << score 
                  << " | Total: " << result.shot.total_score << std::endl;
    }
    
// Save screenshot (only current shot contour, no rings)
    if (result.has_new_shot && !session_history.empty()) {
        cv::Mat screenshot = frame.clone();
        const auto& current_shot = session_history.back();
        std::vector<std::vector<cv::Point>> contour_vec = { current_shot.contour };
        cv::drawContours(screenshot, contour_vec, -1, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);
        cv::imwrite(g_session_folder + "/shot_" + std::to_string(g_shot_count) + ".png", screenshot);
    }
    
    // Draw contour history on display frame
    for (size_t i = 0; i < session_history.size(); ++i) {
        const auto& shot = session_history[i];
        std::vector<std::vector<cv::Point>> contour_vec = { shot.contour };
        cv::Scalar color = (i == session_history.size() - 1) ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);
        cv::drawContours(frame, contour_vec, -1, color, 1, cv::LINE_AA);
    }
    
    // Draw rings (after screenshot)
    for (const auto& ellipse : scoring_ellipses) {
        cv::ellipse(frame, ellipse, cv::Scalar(255, 255, 255), 1);
    }
    
    // Draw overlay (after screenshot)
    std::string info = "Score: " + std::to_string(g_scorer->get_total_score()) +
                      " | Shots: " + std::to_string(g_shot_count) +
                      " | Last: " + std::to_string(g_scorer->get_last_score());
    cv::putText(frame, info, cv::Point(20, 30), 
               cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);
    break;
}    }
    
    result.frame = frame;
    return result;
}

// ============================================================================
// CALIBRATION
// ============================================================================
CalibrationStatus get_calibration_status() {
    CalibrationStatus status;
    status.is_valid = g_last_setup_valid;
    status.rings_detected = (int)g_last_detected_ellipses.size();
    
    if (g_last_setup_valid) {
        status.message = "Good! " + std::to_string(status.rings_detected) + " rings detected";
    } else if (status.rings_detected < MIN_RINGS_REQUIRED) {
        status.message = "Need " + std::to_string(MIN_RINGS_REQUIRED) + "+ rings, found " + 
                        std::to_string(status.rings_detected);
    } else {
        status.message = "Camera angle too steep - adjust position";
    }
    
    return status;
}

bool confirm_calibration() {
    if (!g_initialized) return false;
    if (!g_last_setup_valid) return false;
    if (g_user_approved_geometry.empty()) return false;
    
    // Set approved geometry
    g_tz_detector->set_user_approved_geometry(g_user_approved_geometry);
    
    // Warm up detector with a few frames
    for (int i = 0; i < 10; i++) {
        cv::Mat frame;
        if (g_cap.read(frame) && !frame.empty()) {
            frame = crop_center(frame, g_crop_size);
            cv::Mat frame_gray;
            cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
            g_tz_detector->prepare(frame, frame_gray);
        }
    }
    g_calibration_confirmed = true;
    g_session_folder = create_session_folder();
    std::cout << "Calibration confirmed with " << g_user_approved_geometry.size() << " rings" << std::endl;
    return true;
}

// ============================================================================
// SESSION CONTROL
// ============================================================================
void reset_session() {
    if (g_scorer) {
        delete g_scorer;
        g_scorer = new Scorer();
    }
    g_shot_count = 0;
    g_frame_idx = 0;  // Reset frame counter
    g_session_folder = create_session_folder();
    std::cout << "Session reset" << std::endl;
}

void pause_system() {
    g_paused = true;
}

void resume_system() {
    g_paused = false;
}

bool is_paused() {
    return g_paused;
}

// ============================================================================
// SCORE GETTERS
// ============================================================================
int get_total_score() {
    return g_scorer ? g_scorer->get_total_score() : 0;
}

int get_last_score() {
    return g_scorer ? g_scorer->get_last_score() : -1;
}

int get_shot_count() {
    return g_shot_count;
}

// ============================================================================
// CLEANUP
// ============================================================================
void cleanup_system() {
    if (g_cap.isOpened()) {
        g_cap.release();
    }
    
    if (g_tz_detector) {
        delete g_tz_detector;
        g_tz_detector = nullptr;
    }
    
    if (g_calibration_detector) {
        delete g_calibration_detector;
        g_calibration_detector = nullptr;
    }
    
    if (g_scorer) {
        delete g_scorer;
        g_scorer = nullptr;
    }
    
    g_initialized = false;
    g_calibration_confirmed = false;
    g_last_detected_ellipses.clear();
    g_user_approved_geometry.clear();
    
    std::cout << "System cleaned up" << std::endl;
}


std::string get_session_folder() {
    return g_session_folder;
}