#ifndef CAMERA_STREAM_H
#define CAMERA_STREAM_H
#include <QString>
#include <QDir>
#include <QDateTime>
#include <opencv2/opencv.hpp>



QString takePhoto(const cv::Mat& frame);

#endif // CAMERA_STREAM_H
