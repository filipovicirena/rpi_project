#ifndef CAMERA_STREAM_H
#define CAMERA_STREAM_H

#include <opencv2/opencv.hpp>

// Shared VideoCapture object
extern cv::VideoCapture cap;

// Initializes the camera
void initCamera();

// Takes a photo with cooldown logic
void takePhoto(const cv::Mat& frame);

#endif // CAMERA_STREAM_H
