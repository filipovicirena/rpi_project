#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QLabel>
#include <opencv2/opencv.hpp>
#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateFrame();

 private:
    QLabel* videoLabel;
    cv::VideoCapture cap;
    QTimer *timer;

};
#endif // MAINWINDOW_H
