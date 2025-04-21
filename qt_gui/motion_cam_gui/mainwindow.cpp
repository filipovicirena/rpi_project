#include <pigpio.h>
#include "mainwindow.h"
#include <QDebug>
#include <opencv2/opencv.hpp>
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

    int pirPin = 17;
    gpioSetMode(pirPin, PI_INPUT);

    initCamera(); // Opens cap

    videoLabel = new QLabel(this);
    videoLabel->setFixedSize(320, 240);
    videoLabel->move(50, 50);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateFrame);
    timer->start(30);

    resize(660, 500);
}

MainWindow::~MainWindow()
{
    gpioTerminate(); // Cleanly shut down pigpio
}

void MainWindow::updateFrame() {
    int pirPin = 17;

    if (gpioRead(pirPin) == 1) {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) return;

        takePhoto(frame);

        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        videoLabel->setPixmap(QPixmap::fromImage(qimg).scaled(videoLabel->size(), Qt::KeepAspectRatio));
    } else {
        videoLabel->clear();
    }
}
