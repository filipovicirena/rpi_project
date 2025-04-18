#ifndef CAMERA_STREAM_H
#define CAMERA_STREAM_H

#include <opencv2/opencv.hpp>

extern cv::VideoCapture cap;

void initCamera();
void startStreaming(int pirPin);
void takePhoto(const cv::Mat& frame);  // make takePhoto usable from other files

#endif
