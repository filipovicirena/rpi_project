#include "mainwindow.h"
#include <QDebug>
#include <QLabel>   // For QLabel
#include <opencv2/opencv.hpp>
#include <QTimer>
#include <QImage>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Create the video label with parent 'this' (MainWindow)
    videoLabel = new QLabel(this);
    videoLabel->setFixedSize(320, 240);
    videoLabel->move(50, 50);

    // Open the camera (0 = default camera)
    cap.open(0);
    if (!cap.isOpened()) {
        qDebug() << "Failed to open the camera!";
        return;
    }

    // Create and configure the timer with parent 'this' (MainWindow)
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateFrame);
    timer->start(30);  // 30 ms interval = ~33 fps

    // Resize the window to a reasonable size
    resize(660, 500);
}

MainWindow::~MainWindow()
{
}

void MainWindow::updateFrame() {
    cv::Mat frame;
    cap >> frame;
    cv::resize(frame, frame, cv::Size(640, 480));

    if (frame.empty()) return;

    // Convert BGR to RGB (Qt uses RGB)
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

    QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    videoLabel->setPixmap(QPixmap::fromImage(qimg).scaled(videoLabel->size(), Qt::KeepAspectRatio));
}
