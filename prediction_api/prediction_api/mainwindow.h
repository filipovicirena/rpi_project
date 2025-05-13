#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <opencv2/opencv.hpp>
#include <pigpio.h>

// Define this above the class
struct ConfigData {
    QString endpoint;
    QString apiKey;
    QString url;
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateFrame();
    void onPredictionResult(QNetworkReply* reply);

private:
    QLabel *videoLabel;
    QLabel *streamCaptionLabel;
    QLabel *latestPhotoLabel;
    QLabel *photoCaptionLabel;
    QLabel *timestampLabel;
    QLabel *predictionResultLabel;

    QTimer *timer;
    QNetworkAccessManager *networkManager;

    cv::VideoCapture cap;
    bool apiRequestInProgress = false;
    std::time_t lastRequestTime = 0;


    QString predictionKey;
    QString predictionUrl;
    QString logicAppUrl;

    ConfigData config;

    QString takePhoto(const cv::Mat& frame);
    void sendPhotoForPrediction(const QString& path);
    void sendToLogicApp(const QString& tag, double probability);
    void updateLatestPhoto(const QString& photoPath);
};

#endif // MAINWINDOW_H
