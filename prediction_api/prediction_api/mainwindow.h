#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMainWindow>
#include <QTimer>
#include <QLabel>

class MainWindow : public QMainWindow
{
    Q_OBJECT

std::time_t lastPredictionTime = 0;
const int predictionCooldown = 15; // seconds between predictions

signals:
    void photoTaken(const QString &photoPath);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateFrame();
    void onPredictionResult(QNetworkReply* reply);
    void onPhotoTaken(const QString &photoPath);

private:
    QTimer *timer;
    QLabel *videoLabel;
    QLabel *streamCaptionLabel;
    QLabel *latestPhotoLabel;
    QLabel *photoCaptionLabel;
    QLabel *timestampLabel;
    QNetworkAccessManager* networkManager;
    QString predictionKey;
    QString predictionUrl;

    void sendPhotoForPrediction(const QString& imagePath);
    void updateLatestPhoto();
};

#endif // MAINWINDOW_H
