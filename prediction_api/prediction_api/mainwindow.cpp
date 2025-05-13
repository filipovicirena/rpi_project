#include "mainwindow.h"
#include <QDateTime>
#include <QDir>
#include <QPixmap>
#include <QNetworkRequest>
#include <QFile>
#include <QHttpMultiPart>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>



ConfigData loadConfig(const QString& path) {
    QFile configFile(path);
    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cerr << "Error: Could not open config file.\n";
        return {};
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(configFile.readAll(), &parseError);
    configFile.close();

    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        std::cerr << "Error: Invalid JSON format.\n";
        return {};
    }

    QJsonObject obj = doc.object();

    ConfigData config;
    config.endpoint = obj.value("customVisionEndpoint").toString();
    config.apiKey   = obj.value("customVisionApiKey").toString();
    config.url      = obj.value("logicAppUrl").toString();

    if (config.endpoint.isEmpty() || config.apiKey.isEmpty() || config.url.isEmpty()) {
        std::cerr << "Error: Missing one or more required fields in config.\n";
        return {};
    }

    return config;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    if (gpioInitialise() < 0) {
        qDebug() << "Failed to initialize pigpio!";
        return;
    }

    int pirPin = 17;
    gpioSetMode(pirPin, PI_INPUT);

    // Camera
    cap.open(0);

    // GUI setup
    videoLabel = new QLabel(this);
    videoLabel->setFixedSize(320, 240);
    videoLabel->move(30, 50);
    videoLabel->setStyleSheet("border: 1px solid black");

    streamCaptionLabel = new QLabel("Stream:", this);
    streamCaptionLabel->setFixedWidth(100);
    streamCaptionLabel->setAlignment(Qt::AlignCenter);
    streamCaptionLabel->move(40, 20);
    streamCaptionLabel->setStyleSheet("font-weight: bold; font-size: 14px");

    photoCaptionLabel = new QLabel("Last photo taken:", this);
    photoCaptionLabel->setFixedWidth(150);
    photoCaptionLabel->setAlignment(Qt::AlignCenter);
    photoCaptionLabel->move(390, 20);
    photoCaptionLabel->setStyleSheet("font-weight: bold; font-size: 14px");

    latestPhotoLabel = new QLabel(this);
    latestPhotoLabel->setFixedSize(320, 240);
    latestPhotoLabel->move(380, 50);
    latestPhotoLabel->setStyleSheet("border: 1px solid black");

    timestampLabel = new QLabel(this);
    timestampLabel->move(400, 330);
    timestampLabel->setText("Last photo time:");
    timestampLabel->adjustSize();

    // Prediction result label
    predictionResultLabel = new QLabel(this);
    predictionResultLabel->move(400,300);  // Adjust position as needed
    predictionResultLabel->setText("Prediction: ");
    predictionResultLabel->setStyleSheet("font-style: italic;");
    predictionResultLabel->adjustSize();


    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateFrame);
    timer->start(30);

    resize(750, 350);

    predictionNM = new QNetworkAccessManager(this);
    logicAppNM = new QNetworkAccessManager(this);
    connect(predictionNM, &QNetworkAccessManager::finished, this, &MainWindow::onPredictionResult);

    ConfigData config = loadConfig("config.json");
    if (config.endpoint.isEmpty() || config.apiKey.isEmpty()) {
        std::cerr << "Config loading failed. API access will be disabled.\n";
        return;
    }

    predictionKey = config.apiKey;
    predictionUrl = config.endpoint;
    logicAppUrl   = config.url;

}

MainWindow::~MainWindow() {}

void MainWindow::updateFrame() {
    int pirPin = 17;
    if (gpioRead(pirPin) == 1) {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) return;

        // Stream display
        cv::Mat rgb;
        cv::cvtColor(frame, rgb, cv::COLOR_BGR2RGB);
        QImage qimg(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);
        videoLabel->setPixmap(QPixmap::fromImage(qimg).scaled(videoLabel->size(), Qt::KeepAspectRatio));

        std::time_t now = std::time(nullptr);
        if (!apiRequestInProgress && now - lastRequestTime >= 15) {
            QString path = takePhoto(frame);
            if (!path.isEmpty()) {
                sendPhotoForPrediction(path);
                updateLatestPhoto(path);
                lastRequestTime = now;
            }
        }
    } else {
        videoLabel->clear();
    }
}

QString MainWindow::takePhoto(const cv::Mat& frame) {
    QDir dir("photos");
    if (!dir.exists() && !dir.mkpath(".")) {
        qDebug() << "Failed to create photo directory.";
        return "";
    }

    QString filename = QString("photos/motion_photo_%1.jpg").arg(QDateTime::currentSecsSinceEpoch());
    if (!cv::imwrite(filename.toStdString(), frame)) {
        qDebug() << "Failed to save photo.";
        return "";
    }

    return filename;
}

void MainWindow::sendPhotoForPrediction(const QString& path) {
    QFile *file = new QFile(path);
    if (!file->open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file for prediction.";
        return;
    }

    QNetworkRequest request{QUrl(predictionUrl)};
    request.setRawHeader("Prediction-Key", predictionKey.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");

    apiRequestInProgress = true;
    predictionNM->post(request, file->readAll());
    file->deleteLater();
}

void MainWindow::onPredictionResult(QNetworkReply* reply) {
    apiRequestInProgress = false;

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Prediction API error:" << reply->errorString();
        predictionResultLabel->setText("Prediction: Error");
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();
    qDebug() << "Prediction response:" << response;

    QJsonDocument doc = QJsonDocument::fromJson(response);
    if (!doc.isObject()) {
        predictionResultLabel->setText("Prediction: Invalid response");
        reply->deleteLater();
        return;
    }

    QJsonObject obj = doc.object();
    if (!obj.contains("predictions") || !obj["predictions"].isArray()) {
        predictionResultLabel->setText("Prediction: No predictions");
        reply->deleteLater();
        return;
    }

    QJsonArray predictions = obj["predictions"].toArray();
    if (predictions.isEmpty()) {
        predictionResultLabel->setText("Prediction: None");
        reply->deleteLater();
        return;
    }

    QJsonObject top = predictions.first().toObject();
    QString tag = top["tagName"].toString();
    double probability = top["probability"].toDouble() * 100.0;

    QString resultText = QString("Prediction: %1 (%2%)")
                             .arg(tag)
                             .arg(QString::number(probability, 'f', 1));
    predictionResultLabel->setText(resultText);

    if (tag.toLower() == "intruder") {
        sendToLogicApp(tag, probability);
    }

    predictionResultLabel->adjustSize();

    reply->deleteLater();
}

void MainWindow::sendToLogicApp(const QString& tag, double probability) {
    QJsonObject payload;
    payload["tag"] = tag;
    payload["probability"] = QString::number(probability, 'f', 2);
    payload["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QJsonDocument doc(payload);
    QByteArray jsonData = doc.toJson();

    QNetworkRequest request{ QUrl(logicAppUrl) };
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    logicAppNM->post(request, jsonData);

    qDebug() << "Sent data to Logic App: " << jsonData;
}


void MainWindow::updateLatestPhoto(const QString& path) {
    QPixmap pix(path);
    latestPhotoLabel->setPixmap(pix.scaled(latestPhotoLabel->size(), Qt::KeepAspectRatio));
    timestampLabel->setText("Last photo time: " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    timestampLabel->adjustSize();
}
