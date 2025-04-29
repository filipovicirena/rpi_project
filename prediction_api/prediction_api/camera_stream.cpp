#include "camera_stream.h"

cv::VideoCapture cap;

void initCamera() {
    cap.open(0);
}

cv::Mat getFrame() {
    cv::Mat frame;
    cap >> frame;
    return frame;
}
