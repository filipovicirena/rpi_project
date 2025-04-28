#include <pigpio.h>
#include "mainwindow.h"
#include <iostream>
#include <QDebug>
#include <QDir>
#include <QLabel>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include "camera_stream.h"
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    if (gpioInitialise() < 0) {
        qDebug() << "Failed to initialize pigpio!";
        return;
    }

    int pirPin = 17;
    gpioSetMode(pirPin, PI_INPUT);

    initCamera();

    // Live stream label
    videoLabel = new QLabel(this);
    videoLabel->setFixedSize(320, 240);
    videoLabel->move(30, 50);
    videoLabel->setStyleSheet("border: 1px solid black");

    // Stream capion
    streamCaptionLabel = new QLabel("Stream:", this);
    streamCaptionLabel->setFixedWidth(100);
    streamCaptionLabel->setAlignment(Qt::AlignCenter);
    streamCaptionLabel->move(40, 20);
    streamCaptionLabel->setStyleSheet("font-weight: bold; font-size: 14px");

    // Caption for latest photo
    photoCaptionLabel = new QLabel("Last photo taken:", this);
    photoCaptionLabel->setFixedWidth(150);  // Hardcoded width
    photoCaptionLabel->setAlignment(Qt::AlignCenter);
    photoCaptionLabel->move(390, 20);       // Adjust as needed to look centered
    photoCaptionLabel->setStyleSheet("font-weight: bold; font-size: 14px");

    // Latest photo label
    latestPhotoLabel = new QLabel(this);
    latestPhotoLabel->setFixedSize(320, 240);
    latestPhotoLabel->move(380, 50);
    latestPhotoLabel->setStyleSheet("border: 1px solid black");

    // Tmestamp label for latest photo
    timestampLabel = new QLabel(this);
    timestampLabel->move(400, 330);  // Position it below latestPhotoLabel
    timestampLabel->setText("Last photo time:");
    timestampLabel->adjustSize();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateFrame);
    timer->start(30);

    resize(750, 350);
}

MainWindow::~MainWindow()
{
}

void MainWindow::updateFrame()
{
    int pirPin = 17;
    if (gpioRead(pirPin) == 1) {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) return;

        takePhoto(frame);
        updateLatestPhoto();  // refresh photo label

        std::time_t photoTime = getLastPhotoTime();  //get photo timestamp
        QString timestamp = QDateTime::fromSecsSinceEpoch(photoTime).toString("yyyy-MM-dd hh:mm:ss");
        timestampLabel->setText("Last photo time: " + timestamp);
        timestampLabel->adjustSize();

        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        videoLabel->setPixmap(QPixmap::fromImage(qimg).scaled(videoLabel->size(), Qt::KeepAspectRatio));
    } else {
        videoLabel->clear();
    }
}

void MainWindow::updateLatestPhoto() {
    QDir dir("photos");  // Look in the 'photos' folder
    QStringList filters;
    filters << "motion_photo_*.jpg";
    dir.setNameFilters(filters);
    dir.setSorting(QDir::Time | QDir::Reversed);  // newest first
    QStringList photos = dir.entryList();

    if (!photos.isEmpty()) {
        QPixmap pix("photos/" + photos.last());  // Use the correct path
        latestPhotoLabel->setPixmap(pix.scaled(latestPhotoLabel->size(), Qt::KeepAspectRatio));
    }
}


