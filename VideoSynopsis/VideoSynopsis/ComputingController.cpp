#include "ComputingController.h"

void ComputingController::abort()
{
	mutex.lock();
	paused = true; // Worker starts waiting
	mutex.unlock();

	aborted = true; // Don't need to lock because Worker is waiting
	waitCond.wakeAll(); // Worker resumes loop
}

void ComputingController::startPreview()
{
	mutex.lock();
	if (!this->paused)
	{
		mutex.unlock();
		return;
	}
	this->paused = false;
	mutex.unlock();
	emit this->startWork();
}

void ComputingController::pausePreview()
{
	mutex.lock();
	paused = true;
	mutex.unlock();

	//this->abort();
}

void ComputingController::stopPreview()
{
}

void ComputingController::loadVideo(const QString videoPath)
{
	emit this->loadVideoSignal(videoPath);
}

void ComputingController::setFrameType(vs::FrameType::FrameTypeEnum frameType)
{
	mutex.lock();
	this->frameType = frameType;
	mutex.unlock();
}

void ComputingController::processFrame(const QVideoFrame& frame)
{
	/*auto format = frame.pixelFormat();

	cv::Mat fr = vs::image2Mat(frame.image());
    cv::Size size(frame.height(), frame.width());*/
	emit this->processFrameSignal(frame);
}

void ComputingController::getRenderImage(const QImage& image, int frame) 
{
	emit this->renderImage(image, frame);
}
