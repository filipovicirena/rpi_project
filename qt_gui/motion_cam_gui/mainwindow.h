#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QLabel>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateFrame();

private:
    QTimer *timer;
    QLabel *videoLabel;
    QLabel *latestPhotoLabel;
    QLabel *photoCaptionLabel;
    QLabel *timestampLabel;


    void updateLatestPhoto();
};

#endif // MAINWINDOW_H
