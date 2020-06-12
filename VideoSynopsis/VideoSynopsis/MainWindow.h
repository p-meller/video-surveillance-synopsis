#pragma once

#include <QtWidgets/QMainWindow>
#include <QThread>
#include "ui_MainWindow.h"
#include "ComputingController.h"
#include <QtMultimedia/QMediaPlayer>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);
    virtual ~MainWindow();

public slots:
    void loadImage_click();
    void renderedImage(const QImage& image, int frame);
    void errorSlot(QMediaPlayer::Error playerError);
    void procesFrameFinished(const QImage& img);


private:
    Ui::MainWindowClass ui;
    QMediaPlayer* player;
    //QVideoProbe* probe;

    //QThread* computingThread;

    //ComputingWorker* worker;

    ComputingController controller;
};
