#include "camera_stream.h"
#include <opencv2/opencv.hpp>
#include <pigpio.h>
#include <iostream>
#include <ctime>
#include <sstream>

static cv::VideoCapture cap;
static std::time_t lastPhotoTime = 0;

void initCamera() {
    cap.open(0);
    if (!cap.isOpened()) {
        std::cerr << "Camera failed to open!" << std::endl;
        exit(1);
    }
}

static void takePhoto(const cv::Mat& frame) {
    std::time_t now = std::time(nullptr);
    if (now - lastPhotoTime < 10) {
        std::cout << "Photo not taken: cooldown active (" << (10 - (now - lastPhotoTime)) << "s remaining)." << std::endl;
        return;
    }

    lastPhotoTime = now;

    std::stringstream filename;
    filename << "motion_photo_" << now << ".jpg";

    if (!cv::imwrite(filename.str(), frame)) {
        std::cerr << "Failed to save photo!" << std::endl;
    } else {
        std::cout << "Photo saved as " << filename.str() << std::endl;
    }
}

void startStreaming(int pirPin) {
    cv::Mat frame;
    std::cout << "Starting camera stream..." << std::endl;

    bool photoTakenThisMotion = false;

    while (gpioRead(pirPin) == 1) {
        cap >> frame;
        if (frame.empty()) continue;

        // Take photo only once per motion event, if cooldown allows
        if (!photoTakenThisMotion) {
            takePhoto(frame);
            photoTakenThisMotion = true;
        }

        cv::imshow("Camera Stream", frame);
        if (cv::waitKey(30) == 'q') break;
    }

    std::cout << "Stopping camera stream." << std::endl;
    cv::destroyAllWindows();
}

