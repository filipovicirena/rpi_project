#include <pigpio.h>
#include "mainwindow.h"
#include <iostream>
#include <QDebug>
#include <QLabel>   // For QLabel
#include <opencv2/opencv.hpp>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include "camera_stream.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    if (gpioInitialise() < 0) {
        qDebug() << "Failed to initialize pigpio!";
        return;
    }
    int pirPin = 11;
    gpioSetMode(pirPin, PI_INPUT);

    initCamera();         // initialize camera (opens cap)

    // Create the video label
    videoLabel = new QLabel(this);
    videoLabel->setFixedSize(320, 240);
    videoLabel->move(50, 50);

    // No need to open cap again here

    // Timer setup
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateFrame);
    timer->start(30);  // update every 30ms

    // Resize the window
    resize(660, 500);
}

MainWindow::~MainWindow()
{
}
void MainWindow::updateFrame() {
    int pirPin = 4;  // same as above
    if (gpioRead(pirPin) == 1) {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) return;

        takePhoto(frame);  // your existing cooldown logic

        // Convert and show in QLabel
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        videoLabel->setPixmap(QPixmap::fromImage(qimg).scaled(videoLabel->size(), Qt::KeepAspectRatio));
    } else {
        videoLabel->clear();  // hide image when no motion
    }
}
