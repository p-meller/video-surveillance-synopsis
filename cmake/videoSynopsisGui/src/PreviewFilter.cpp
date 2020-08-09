#include "PreviewFilter.h"
#include "videoSynopsisLib.h"

#include <QDebug>
#include <opencv2/imgproc.hpp>
#include <memory>

QCvPreviewFilterRunnable::QCvPreviewFilterRunnable(PreviewFilter* creator) : filter(creator),
																			 detector(std::make_unique<Detector>(true))
{
}

QVideoFilterRunnable* PreviewFilter::createFilterRunnable()
{
	return new QCvPreviewFilterRunnable(this);
}

void PreviewFilter::setFgOutputFiltered(bool filtered)
{
	this->previewFgFiltered = filtered;
}

void PreviewFilter::setContoursOutputFiltered(bool filtered)
{
	this->previewContoursFiltered = filtered;
}

QVideoFrame QCvPreviewFilterRunnable::run(QVideoFrame* input, const QVideoSurfaceFormat& surfaceFormat, RunFlags flags)
{
	Q_UNUSED(flags);
	input->map(QAbstractVideoBuffer::ReadOnly);

	if (surfaceFormat.handleType() == QAbstractVideoBuffer::NoHandle)
	{
		//TODO: add other pixel format support
		cv::Mat picYUV420 = cv::Mat(input->height() * 3 / 2, input->width(), CV_8UC1, input->bits());

		if (!picYUV420.empty())
		{
			cv::Mat picBGR;
			cv::cvtColor(picYUV420, picBGR, cv::COLOR_YUV2BGR_I420);
			detector->processFrame(picBGR);

			{
				DetectorOutputTypeEnum outputType(
						this->filter->previewFgFiltered ? DetectorOutputTypeEnum::BG_SUBTRACTION_FILTERED
														: DetectorOutputTypeEnum::BG_SUBTRACTION);
				cv::Mat outputFrame = detector->getOutput(outputType);
				if (outputFrame.channels() == 1)
				{
					cv::cvtColor(outputFrame, outputFrame, cv::COLOR_GRAY2BGR);
				}
				QImage image = QImage((const uchar*)outputFrame.data, outputFrame.cols, outputFrame.rows,
						outputFrame.step,
						QImage::Format_BGR888);
				emit filter->fgImageUpdate(image.copy());
			}
			{
				DetectorOutputTypeEnum outputType(
						this->filter->previewContoursFiltered ? DetectorOutputTypeEnum::CONTOURS_FILTERED
															  : DetectorOutputTypeEnum::CONTOURS);
				cv::Mat outputFrame = detector->getOutput(outputType);
				if (outputFrame.channels() == 1)
				{
					cv::cvtColor(outputFrame, outputFrame, cv::COLOR_GRAY2BGR);
				}
				QImage image = QImage((const uchar*)outputFrame.data, outputFrame.cols, outputFrame.rows,
						outputFrame.step,
						QImage::Format_BGR888);
				emit filter->contoursImageUpdate(image.copy());
			}
			{
				DetectorOutputTypeEnum outputType(DetectorOutputTypeEnum::DETECTIONS);
				cv::Mat outputFrame = detector->getOutput(outputType);
				if (outputFrame.channels() == 1)
				{
					cv::cvtColor(outputFrame, outputFrame, cv::COLOR_GRAY2BGR);
				}
				QImage image = QImage((const uchar*)outputFrame.data, outputFrame.cols, outputFrame.rows,
						outputFrame.step,
						QImage::Format_BGR888);
				emit filter->detectionsImageUpdate(image.copy());
			}

		}

	}
	else
	{
		qDebug() << "Other surface formats are not supported yet!";
	}

	input->unmap();
	return *input;
}
