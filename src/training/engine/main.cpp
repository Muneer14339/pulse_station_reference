#include <iostream>
#include <opencv2/opencv.hpp>
#include "dart_system.h"

// ========================================
// GLOBAL STATE
// ========================================
static int  g_mode    = MODE_CALIBRATION;
static bool g_running = false;

// ========================================
// CALL THESE FUNCTIONS FROM ANYWHERE
// ========================================

// Start the system (call once after init)
void start() {
    g_running = true;
    std::cout << "System started\n";
}

// Stop the system completely
void stop() {
    g_running = false;
    std::cout << "System stopped\n";
}

// Switch to streaming mode (just video, no processing)
void mode_streaming() {
    g_mode = MODE_STREAMING;
    std::cout << "Mode: STREAMING\n";
}

// Switch to calibration mode (point camera at dartboard)
void mode_calibration() {
    g_mode = MODE_CALIBRATION;
    std::cout << "Mode: CALIBRATION\n";
}

// Switch to scoring mode (detect and score darts)
void mode_scoring() {
    g_mode = MODE_SCORING;
    std::cout << "Mode: SCORING\n";
}

// Pause processing (keeps camera open, just skips processing)
void pause() {
    pause_system();
    std::cout << "Paused\n";
}

// Resume processing after pause
void resume() {
    resume_system();
    std::cout << "Resumed\n";
}

// Confirm calibration and auto-switch to scoring
// Returns true if calibration was valid and confirmed
bool confirm() {
    CalibrationStatus s = get_calibration_status();
    if (!s.is_valid) {
        std::cout << "Calibration not ready: " << s.message << "\n";
        return false;
    }
    if (confirm_calibration()) {
        g_mode = MODE_SCORING;
        std::cout << "Calibration confirmed → SCORING\n";
        return true;
    }
    return false;
}

// Reset current session (clears shots and score)
void reset() {
    reset_session();
    std::cout << "Session reset\n";
}

// Get one processed frame — call this in a loop or timer
// Returns FrameResult with:
//   result.frame        → the image to display
//   result.has_new_shot → true if a dart was just detected
//   result.shot.score   → score of the latest shot
//   result.shot.total_score → cumulative score
FrameResult tick() {
    return process_frame(g_mode);
}

// ========================================
// MAIN — init + loop
// ========================================
int main() {
    freopen("/dev/null", "w", stderr);

    // Find cameras
    auto cameras = get_available_cameras();
    if (cameras.empty()) {
        std::cout << "No cameras found!\n";
        return -1;
    }

    // Initialize with first camera
    if (!init_system(cameras[0].index, 600)) {
        std::cout << "Failed to initialize!\n";
        return -1;
    }

    // ── Example usage of the functions ──
    start();
    mode_calibration();

    while (g_running) {
        FrameResult result = tick();

        // Show frame
        if (!result.frame.empty())
            cv::imshow("Dart Scorer", result.frame);

        // New dart detected
        if (result.has_new_shot)
            std::cout << "Shot #"     << result.shot.shot_number
                      << " | Score: " << result.shot.score
                      << " | Total: " << result.shot.total_score << "\n";

        // ESC to quit
        if (cv::waitKey(1) == 27) stop();
    }

    // Final results
    std::cout << "Final Score: " << get_total_score() << "\n";
    std::cout << "Total Shots: " << get_shot_count()  << "\n";

    cleanup_system();
    cv::destroyAllWindows();
    return 0;
}
