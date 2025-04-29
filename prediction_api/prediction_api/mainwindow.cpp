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
#include <QNetworkRequest>
#include <QFile>
#include <QHttpMultiPart>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


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

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onPredictionResult);

    // Replace these with your real values later
    predictionKey = "GJFKKmZh8FETOTu99EHm2XVTebk0Y7JpydKFbPxQas9aVyF58eHaJQQJ99BDACi5YpzXJ3w3AAAIACOG8MzH";
    predictionUrl = "https://model021-prediction.cognitiveservices.azure.com/customvision/v3.0/Prediction/1554ff92-88aa-4394-9787-1444573fe27c/classify/iterations/V1/image";

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

        QString photoPath = takePhoto(frame);
        if (!photoPath.isEmpty()) {
            emit photoTaken(photoPath);  // this signal will go to MainWindow
        }


        /*QString path = takePhoto(frame);
        if (!path.isEmpty()) {
            sendPhotoForPrediction(path);
        }*/

        //takePhoto(frame);
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
    connect(this, &MainWindow::photoTaken, this, &MainWindow::onPhotoTaken);

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

void MainWindow::sendPhotoForPrediction(const QString& imagePath)
{
    QFile* file = new QFile(imagePath);
    if (!file->open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open image file:" << imagePath;
        delete file;
        return;
    }

    QNetworkRequest request{ QUrl(predictionUrl) };

    request.setRawHeader("Prediction-Key", predictionKey.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");

    networkManager->post(request, file->readAll());
    file->close();
    file->deleteLater();
}

void MainWindow::onPredictionResult(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Prediction API error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();
    qDebug() << "Prediction API Response:" << response;
    // You can parse JSON response here if you want

    reply->deleteLater();
}

void MainWindow::onPhotoTaken(const QString &photoPath)
{
    std::time_t now = std::time(nullptr);
    if (now - lastPredictionTime < predictionCooldown) {
       /* qDebug() << "Prediction skipped: cooldown active ("
                 << (predictionCooldown - (now - lastPredictionTime)) << "s remaining).";
        */return;
    }

    lastPredictionTime = now;
    sendPhotoForPrediction(photoPath);
}

