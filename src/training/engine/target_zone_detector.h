#ifndef TARGET_ZONE_DETECTOR_H
#define TARGET_ZONE_DETECTOR_H
#include <opencv2/opencv.hpp>
#include "FLED.h"
#include <map>
#include <vector>

class TargetZoneDetector {
private:
    FLED detector;
    
    // Basic parameters
    int min_prep_frames;
    int prep_frames_count;
    
    cv::RotatedRect last_stable_ellipse;
    bool has_stable_ellipse = false;
    
    // Ring geometry structure - stores fixed relative positions and sizes
    struct RingGeometry {
        cv::Point2f center_offset;  // Offset from target center (NEVER changes)
        cv::Size2f size;           // Ring size (NEVER changes)
        float angle;               // Ring angle (NEVER changes)
        int score_value;           // Score for hitting this ring
    };
    
    std::vector<RingGeometry> stable_ring_geometry;  // Fixed ring geometry
    cv::Point2f current_target_center;              // Only this moves
    bool geometry_initialized;
    // Original target_zone_info for backward compatibility
    std::map<std::string, std::vector<cv::RotatedRect>> target_zone_info;
    // Geometry initialization
    void initialize_stable_geometry();
    
    // Smoothing
    cv::Point2f last_stable_center;
    bool has_last_center;
    float calculate_smoothing_factor(float movement);
    
    // for frame validation
    int consecutive_invalid_frames = 0;
    int skip_frame_threshold = 3;  // Skip detection after 3 consecutive bad frames

public:
    TargetZoneDetector(int frame_width, int frame_height, int min_prep_frames = 10);
    
    // Main processing methods
    void prepare(cv::Mat frame, cv::Mat frame_gray);
    int process(cv::Mat frame, cv::Mat frame_gray);
    
    // Status methods
    bool is_ready();
    bool is_geometry_initialized();
    bool is_shifting();
    
    // Scoring methods
    std::vector<cv::RotatedRect> get_ellipses_for_scoring();
    cv::Point2f get_current_target_center();
    
    // Backward compatibility
    std::map<std::string, std::vector<cv::RotatedRect>> get_target_zone_info();
    
    // Configuration
    void set_ring_scores(const std::vector<int>& scores);
    // FIXED: Detection method signature
    std::vector<cv::RotatedRect> _detect(cv::Mat frame_gray);
    
    // NEW: Set geometry from user approval instead of new detection
    void set_user_approved_geometry(const std::vector<cv::RotatedRect>& user_geometry) {
        target_zone_info["ellipses"] = user_geometry;
        if (!user_geometry.empty()) {
            current_target_center = user_geometry[0].center;
            initialize_stable_geometry();  // Use approved geometry for stable rings
        }
    }
    
        bool is_detection_good_enough(const std::vector<cv::RotatedRect>& ellipses);
};

#endif // TARGET_ZONE_DETECTOR_H
