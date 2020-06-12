#pragma once
#include <QObject>
#include <QWaitCondition>
#include <QMutex>

#include "MotionExtractor.h"
#include <opencv2/videoio.hpp>
#include <QtMultimedia/QVideoFrame>
#include <string>

class ComputingWorker : public QObject
{
    Q_OBJECT

    bool& aborted, &paused;
    QWaitCondition& waitCond;
    QMutex& mutex;
    vs::FrameType::FrameTypeEnum& frameType;

    vs::MotionExtractor motionExtractor;
    cv::VideoCapture capture;
    

public:
    ComputingWorker(bool& aborted, bool& paused, QWaitCondition& waitCond, QMutex& mutex, vs::FrameType::FrameTypeEnum& frameType);
    ~ComputingWorker();

signals:
    void renderedImage(const QImage& image, int frame);
    void sendMessage(const QString& msg);
    void frameProcessed(const QVideoFrame& frame);
    void procesFrameFinished(const QImage& img);

public slots:
    void loadVideo(const QString videoPath);
    void doWork();
    void stop();
    void processFrame(const QVideoFrame& frame);


};

