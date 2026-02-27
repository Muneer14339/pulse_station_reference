#ifndef DART_SYSTEM_H
#define DART_SYSTEM_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

// ============================================================================
// CONSTANTS
// ============================================================================
#define MODE_STREAMING    0
#define MODE_CALIBRATION  1
#define MODE_SCORING      2

// ============================================================================
// STRUCTS
// ============================================================================
struct CameraInfo {
    int index;
    std::string name;
    int width;
    int height;
};

struct CalibrationStatus {
    bool is_valid;
    int rings_detected;
    std::string message;
};

struct ShotResult {
    int score;          // 0-10, -1 if no new shot
    int direction;      // 0-9
    int shot_number;
    int total_score;
};

struct FrameResult {
    cv::Mat frame;
    bool has_new_shot;
    ShotResult shot;
};

// ============================================================================
// FUNCTIONS
// ============================================================================

// Camera discovery
std::vector<CameraInfo> get_available_cameras();

// System initialization
bool init_system(int camera_index, int crop_size = 600);

// Main processing - call this in your loop
// mode: MODE_STREAMING (0), MODE_CALIBRATION (1), MODE_SCORING (2)
FrameResult process_frame(int mode);

// Calibration
CalibrationStatus get_calibration_status();
bool confirm_calibration();

// Session control
void reset_session();
void pause_system();
void resume_system();
bool is_paused();

// Score getters
int get_total_score();
int get_last_score();
int get_shot_count();

// Cleanup
void cleanup_system();

#endif // DART_SYSTEM_H
