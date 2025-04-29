#ifndef CAMERA_STREAM_H
#define CAMERA_STREAM_H

#include <opencv2/opencv.hpp>
#include <QString>

// Shared VideoCapture object
extern cv::VideoCapture cap;

// Initializes the camera
void initCamera();

// Takes a photo with cooldown logic
QString takePhoto(const cv::Mat& frame);


// Getter ffor providing access to timestamp of latest photo
std::time_t getLastPhotoTime();


#endif // CAMERA_STREAM_H
