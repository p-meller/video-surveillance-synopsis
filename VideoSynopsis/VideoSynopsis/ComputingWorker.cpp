#include "ComputingWorker.h"

#include <QImage>
#include <QThread>
#include <QPixmap>
#include <cmath>

#include "cvmatandqimage.h"

ComputingWorker::ComputingWorker(bool& aborted, bool& paused, QWaitCondition& waitCond, QMutex& mutex, vs::FrameType::FrameTypeEnum& frameType):
	QObject(), aborted(aborted), paused(paused), waitCond(waitCond), mutex(mutex), frameType(frameType)
{
}

ComputingWorker::~ComputingWorker()
{
	//mutex.lock();
	//abort = true;
	//condition.wakeOne();
	//mutex.unlock();

	//wait();
}

void ComputingWorker::doWork()
{
	while (capture.isOpened())// && !this->aborted)
	{
		cv::Mat capframe;
		capture.read(capframe);
		if (capframe.empty()) 
		{
			return;
		}

		motionExtractor.apply(capframe);

		mutex.lock();
		cv::Mat frame = motionExtractor.getFrame(frameType);
		mutex.unlock();

		QImage::Format format;
		if (frame.channels() == 1)
		{
			format = QImage::Format::Format_Grayscale8;
		}
		else 
		{
			format = QImage::Format::Format_BGR888;
		}

		QImage qImage = QImage((unsigned char*)frame.data, frame.cols, frame.rows, format);

		emit renderedImage(qImage, capture.get(cv::CAP_PROP_POS_FRAMES));

		QThread::msleep(20);

		mutex.lock();
		if (paused) {

			mutex.unlock();
			break;
			//waitCond.wait(&mutex);
			//paused = false;
		}
		mutex.unlock();
	}
}

void ComputingWorker::stop() 
{
	//doWork = false;
}

void ComputingWorker::processFrame(const QVideoFrame& frame)
{
	//auto format = frame.pixelFormat();
	//cv::Mat fr(frame.bits(),cv::);

	cv::Mat inputTemp = vs::image2Mat_shared(frame.image());
	cv::Mat input;
	cv::cvtColor(inputTemp, input, cv::COLOR_BGRA2BGR);

	motionExtractor.apply(input);

	mutex.lock();
	cv::Mat output = motionExtractor.getFrame(frameType);
	mutex.unlock();

	QImage img = vs::mat2Image_shared(output);

	emit this->procesFrameFinished(img);


}

void ComputingWorker::loadVideo(const QString videoPath)
{
	this->capture.open(videoPath.toUtf8().constData());
	this->stop();
}
