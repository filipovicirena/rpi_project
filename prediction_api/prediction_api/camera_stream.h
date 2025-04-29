#ifndef CAMERA_STREAM_H
#define CAMERA_STREAM_H

#include <opencv2/opencv.hpp>

void initCamera();
cv::Mat getFrame();

#endif // CAMERA_STREAM_H
