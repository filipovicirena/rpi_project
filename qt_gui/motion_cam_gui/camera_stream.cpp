#include "camera_stream.h"
#include <pigpio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>
#include <sstream>

cv::VideoCapture cap;

std::time_t lastPhotoTime = 0;  //used to be static!
std::time_t getLastPhotoTime() {
    return lastPhotoTime;
}

void initCamera() {
    cap.open(0);
    if (!cap.isOpened()) {
        std::cerr << "Camera failed to open!" << std::endl;
        exit(1);
    } else {
        std::cout << "Camera opened successfully." << std::endl;
    }
}

void takePhoto(const cv::Mat& frame) {
    std::time_t now = std::time(nullptr);
    if (now - lastPhotoTime < 10) {
        std::cout << "Photo not taken: cooldown active ("
                  << (10 - (now - lastPhotoTime)) << "s remaining)." << std::endl;
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
