//#ifndef SCORER_H
//#define SCORER_H
//
//#include <opencv2/opencv.hpp>
//#include <sstream>
//
//template<typename T>
//std::string vectorToString(const std::vector<T>& vec) {
//    std::ostringstream oss;
//
//    oss << "[ ";
//    if (!vec.empty()) {
//        // Insert the first element
//        oss << vec[0];
//        // Insert the rest of the elements with a space separator
//        for (size_t i = 1; i < vec.size(); ++i) {
//            oss << " " << vec[i];
//        }
//    }
//    oss << " ]";
//    return oss.str();
//}
//
//class Scorer {
//private:
//    int warmup_frames;
//    int min_hit_duration;
//    cv::Ptr<cv::BackgroundSubtractorMOG2> bg_sub;
//    long last_hit_time;
//    std::vector<std::vector<cv::Point>> history;
//    std::vector<int> score_history;
//    int warmup_count;
//    int total_score;
//    int last_score;
//    cv::Point last_hit_point;
//    cv::Mat last_black_frame;
//    cv::Point p;
//
//    void _init_state();
//
//    // Helper function to check if point is inside ellipse
//    bool isPointInEllipse(const cv::Point& point, const cv::RotatedRect& ellipse);
//
//    // helper function for bullet direction
//    int getBulletDirection(cv::Point2f bullet_center, cv::Point2f target_center);
//
//public:
//    Scorer(int warmup_frames = 5, int min_hit_duration = 1);
//    // Updated to accept ellipses instead of circles
//    int process(cv::Mat frame, cv::Mat frame_gray, std::map<std::string, std::vector<cv::RotatedRect>> target_zone, long timestamp,bool is_ble);
//    int get_total_score() { return total_score; }
//    int get_last_score() { return last_score; }
//    std::string get_history_as_string() { return vectorToString(score_history); }
//    std::vector<std::vector<cv::Point>> get_history_for_display(cv::Rect roi = cv::Rect());
//    std::vector<std::vector<cv::Point>> get_history() const { return history; }
//
//    // for bullet direction
//    std::vector<int> direction_history;
//
//    // bullet direction to sent back in main
//    int get_last_direction() const { return direction_history.empty() ? 0 : direction_history.back(); }
//
//};
//
//#endif // SCORER_H


#ifndef SCORER_H
#define SCORER_H

#include <opencv2/opencv.hpp>
#include <sstream>
#include <deque>

template<typename T>
std::string vectorToString(const std::vector<T>& vec) {
    std::ostringstream oss;
    oss << "[ ";
    if (!vec.empty()) {
        oss << vec[0];
        for (size_t i = 1; i < vec.size(); ++i) {
            oss << " " << vec[i];
        }
    }
    oss << " ]";
    return oss.str();
}

class Scorer {
private:
    int warmup_frames;
    int min_hit_duration;
    cv::Ptr<cv::BackgroundSubtractorMOG2> bg_sub;
    long last_hit_time;
    std::vector<std::vector<cv::Point>> history;
    std::vector<int> score_history;
    int warmup_count;
    int total_score;
    int last_score;
    cv::Point last_hit_point;
    cv::Mat last_black_frame;
    cv::Point p;

    // Buffer for recent frames' data (up to 5 frames)
    struct FrameData {
        cv::Mat frame;
        std::vector<cv::RotatedRect> ellipses;
        std::vector<cv::Point> bullet_contour;
        long timestamp;
    };
    std::deque<FrameData> frame_buffer;
    static const size_t MAX_FRAMES = 5;

    cv::Mat first_comparison_frame;
    long first_comparison_timestamp;

    void _init_state();
    bool isPointInEllipse(const cv::Point& point, const cv::RotatedRect& ellipse, float shrink_pixels = 0.0f);
    bool isPointOnEllipseBoundary(const cv::Point& point, const cv::RotatedRect& ellipse, float shrink_pixels = 0.0f);
    int getBulletDirection(cv::Point2f bullet_center, cv::Point2f target_center);
    int calculate_score(const std::vector<cv::Point>& bullet_contour, const std::vector<cv::RotatedRect>& ellipses);

public:
    Scorer(int warmup_frames = 5, int min_hit_duration = 1);
    int process(cv::Mat frame, cv::Mat frame_gray, std::map<std::string, std::vector<cv::RotatedRect>> target_zone, long timestamp , bool is_ble);
    int get_total_score() { return total_score; }
    int get_last_score() { return last_score; }
    std::string get_history_as_string() { return vectorToString(score_history); }
    std::vector<std::vector<cv::Point>> get_history_for_display(cv::Rect roi = cv::Rect());
    std::vector<std::vector<cv::Point>> get_history() const { return history; }
    std::vector<int> direction_history;
    int get_last_direction() const { return direction_history.empty() ? 0 : direction_history.back(); }
};

#endif // SCORER_H


///Working_Now_25_oct
//#ifndef SCORER_H
//#define SCORER_H
//
//#include <opencv2/opencv.hpp>
//#include <sstream>
//#include <deque>
//
//template<typename T>
//std::string vectorToString(const std::vector<T>& vec) {
//    std::ostringstream oss;
//    oss << "[ ";
//    if (!vec.empty()) {
//        oss << vec[0];
//        for (size_t i = 1; i < vec.size(); ++i) {
//            oss << " " << vec[i];
//        }
//    }
//    oss << " ]";
//    return oss.str();
//}
//
//class Scorer {
//private:
//    cv::Mat prev_blur;  // Store previous frame
//    int warmup_frames;
//    int min_hit_duration;
//    cv::Ptr<cv::BackgroundSubtractorMOG2> bg_sub;
//    long last_hit_time;
//    std::vector<std::vector<cv::Point>> history;
//    std::vector<int> score_history;
//    int warmup_count;
//    int total_score;
//    int last_score;
//    cv::Point last_hit_point;
//    cv::Mat last_black_frame;
//    cv::Point p;
//    cv::Mat bg_model;              // Running background (CV_32F)
//    cv::Mat confirmed_bullet_mask; // Mask of confirmed bullets (exclude from bg update)
//    struct CandidateBullet {
//        cv::Point2f center;
//        int frames_seen;
//        long first_seen_time;
//    };
//    std::vector<CandidateBullet> candidates;
//    std::vector<cv::Point2f> confirmed_bullet_positions;  // ADD THIS
//    cv::Point2f last_candidate_center;
//    int candidate_frame_count;
//    std::vector<int> candidate_scores;
//
//    // Buffer for recent frames' data (up to 5 frames)
//    struct FrameData {
//        cv::Mat frame;
//        std::vector<cv::RotatedRect> ellipses;
//        std::vector<cv::Point> bullet_contour;
//        long timestamp;
//    };
//    std::deque<FrameData> frame_buffer;
//    static const size_t MAX_FRAMES = 5;
//
//    cv::Mat first_comparison_frame;
//    long first_comparison_timestamp;
//
//    void _init_state();
//    bool isPointInEllipse(const cv::Point& point, const cv::RotatedRect& ellipse, float shrink_pixels = 0.0f);
//    bool isPointOnEllipseBoundary(const cv::Point& point, const cv::RotatedRect& ellipse, float shrink_pixels = 0.0f);
//    int getBulletDirection(cv::Point2f bullet_center, cv::Point2f target_center);
//    int calculate_score(const std::vector<cv::Point>& bullet_contour, const std::vector<cv::RotatedRect>& ellipses);
//
//public:
//    Scorer(int warmup_frames = 5, int min_hit_duration = 1);
//    int process(cv::Mat frame, cv::Mat frame_gray, std::map<std::string, std::vector<cv::RotatedRect>> target_zone, long timestamp , bool is_ble);
//    int get_total_score() { return total_score; }
//    int get_last_score() { return last_score; }
//    std::string get_history_as_string() { return vectorToString(score_history); }
//    std::vector<std::vector<cv::Point>> get_history_for_display(cv::Rect roi = cv::Rect());
//    std::vector<std::vector<cv::Point>> get_history() const { return history; }
//    std::vector<int> direction_history;
//    int get_last_direction() const { return direction_history.empty() ? 0 : direction_history.back(); }
//};
//
//#endif // SCORER_H
