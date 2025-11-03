#include "camera_stream.h"

QString takePhoto(const cv::Mat& frame) {
    QDir dir("photos");
    if (!dir.exists() && !dir.mkpath(".")) {
        std::cerr << "Failed to create photo directory.";
        return "";
    }
    QString filename = QString("photos/motion_photo_%1.jpg").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss"));


    if (!cv::imwrite(filename.toStdString(), frame)) {
        std::cerr << "Failed to save photo.";
        return "";
    }

    return filename;
}
