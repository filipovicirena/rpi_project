#include "camera_stream.h"
#include <pigpio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>
#include <sstream>
#include <QDir>
#include <QString>

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

QString takePhoto(const cv::Mat& frame) {
    std::time_t now = std::time(nullptr);
    if (now - lastPhotoTime < 10) {
        //std::cout << "Photo not taken: cooldown active (" << (10 - (now - lastPhotoTime)) << "s remaining)." << std::endl;
        return "";  // Nothing saved
    }

    lastPhotoTime = now;

    QDir dir("photos");
    if (!dir.exists() && !dir.mkpath(".")) {
        std::cerr << "Failed to create photos directory!" << std::endl;
        return "";
    }

    QString filename = QString("photos/motion_photo_%1.jpg").arg(now);
    if (!cv::imwrite(filename.toStdString(), frame)) {
        std::cerr << "Failed to save photo!" << std::endl;
        return "";
    } else {
        std::cout << "Photo saved as " << filename.toStdString() << std::endl;
        return filename;
    }
}


/*
QString takePhoto(const cv::Mat& frame) {
    std::time_t now = std::time(nullptr);
    if (now - lastPhotoTime < 10) {
        std::cout << "Photo not taken: cooldown active ("
                  << (10 - (now - lastPhotoTime)) << "s remaining)." << std::endl;
        return "";  // Return empty string if no photo was taken
    }

    lastPhotoTime = now;

    // Use QDir to create the photos folder if it doesn't exist
    QDir dir("photos");
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            std::cerr << "Failed to create photos directory!" << std::endl;
            return "";
        }
    }

    // Construct filename
    std::stringstream filename;
    filename << "photos/motion_photo_" << now << ".jpg";
    std::string filepath = filename.str();

    // Save photo
    if (!cv::imwrite(filepath, frame)) {
        std::cerr << "Failed to save photo!" << std::endl;
        return "";
    } else {
        std::cout << "Photo saved as " << filepath << std::endl;
        return QString::fromStdString(filepath);
    }
}

*/
