#pragma once
#include <QObject>
#include <QWaitCondition>
#include <QMutex>
#include <QThread>
#include <QTimer>
#include "ComputingWorker.h"


class ComputingController :public QObject
{
    Q_OBJECT

    bool aborted = true, paused = true;
    QWaitCondition waitCond;
    QMutex mutex;
    vs::FrameType::FrameTypeEnum frameType;
    QThread workerThread;

    QTimer qtimer;


public:
    ComputingController() : QObject() {
        frameType = vs::FrameType::INPUT;
        ComputingWorker* worker = new ComputingWorker(aborted, paused, waitCond, mutex, frameType);
        worker->moveToThread(&workerThread);
        connect(&workerThread, &QThread::finished, worker, &ComputingWorker::deleteLater);
        connect(this, &ComputingController::startWork, worker, &ComputingWorker::doWork);
        connect(worker, &ComputingWorker::renderedImage, this, &ComputingController::getRenderImage);
        connect(this, &ComputingController::loadVideoSignal ,worker, &ComputingWorker::loadVideo);
        connect(this, &ComputingController::processFrameSignal, &ComputingWorker::processFrame);
        //connect(worker, &ComputingWorker::procesFrameFinished, this, &ComputingController::procesFrameFinishedTemp);
        workerThread.start();
    }

    void abort();

    void startPreview();
    void pausePreview();
    void stopPreview();
    void loadVideo(const QString videoPath);
    void setFrameType(vs::FrameType::FrameTypeEnum frameType);

    public slots:
    void getRenderImage(const QImage& image, int frame);  
    void processFrame(const QVideoFrame& frame);

    void procesFrameFinishedTemp(const QImage& img) {
        emit this->procesFrameFinished(img);
    }

signals:
    void renderImage(const QImage& image, int frame);
    void loadVideoSignal(const QString videoPath);
    void processFrameSignal(const QVideoFrame& frame);
    void procesFrameFinished(const QImage& img);
    void startWork();
};

