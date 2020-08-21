#ifndef VIDEOSYNOPSIS_VIDEOSYNOPSISCONTROLLER_H
#define VIDEOSYNOPSIS_VIDEOSYNOPSISCONTROLLER_H

#include <QObject>
#include <QtCore/QThread>
#include "videoSynopsisLib.h"

struct TrackImage{
	int currentFrame=0;
	std::vector<cv::Mat> images;
	std::vector<cv::Rect> rois;
};

class VideoSynopsisWorker : public QObject
{
Q_OBJECT
	Detector detector;
	Tracker tracker;

public:
	VideoSynopsisWorker();

public slots:

	void processVideo(const QString& videoPath);

	void createSimpleVideoSynopsis(const QString& videoPath);

	void createVideoSynopsis(const QString& videoPath);

signals:

	void progressMessage(QString message, int step, int stepCount);
};

class VideoSynopsisController : public QObject
{
Q_OBJECT
	QThread workerThread;

public:
	VideoSynopsisController()
	{
		VideoSynopsisWorker* worker = new VideoSynopsisWorker;
		worker->moveToThread(&workerThread);
		connect(worker, &VideoSynopsisWorker::progressMessage, this, &VideoSynopsisController::progressMessage);
		connect(this, &VideoSynopsisController::processVideo, worker, &VideoSynopsisWorker::processVideo);
		connect(this, &VideoSynopsisController::createSimpleVideoSynopsis, worker,
				&VideoSynopsisWorker::createSimpleVideoSynopsis);
		connect(this, &VideoSynopsisController::createVideoSynopsis, worker, &VideoSynopsisWorker::createVideoSynopsis);
		workerThread.start();
	}

	~VideoSynopsisController()
	{
		workerThread.quit();
		workerThread.wait();
	}

public slots:
	int getVideoMilisecondsForPreview(const QString& trackIdStr);

signals:

	void progressMessage(QString message, int step, int stepCount);

	void processVideo(const QString& videoPath);

	void createSimpleVideoSynopsis(const QString& videoPath);

	void createVideoSynopsis(const QString& videoPath);
};


#endif //VIDEOSYNOPSIS_VIDEOSYNOPSISCONTROLLER_H
