#include "Detector.h"
#include <opencv2/imgproc.hpp>
#include <chrono>

void Detector::getMaskByContours(const cv::Mat& input)
{
	std::vector<std::vector<cv::Point> > contourList;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(input, contourList, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	this->contours_ = cv::Mat::zeros(input.size(), CV_8UC1);
	for (int i = 0; i < contourList.size(); i++)
	{
		double area = cv::contourArea(contourList[i]);
		if (area > 20)
		{
			cv::drawContours(this->contours_, contourList, i, cv::Scalar(255), -1);
		}
	}

	cv::Mat fiiltered;

	auto structEleme3 = cv::getStructuringElement(cv::MorphShapes::MORPH_ELLIPSE, cv::Size(35, 35));
	cv::morphologyEx(this->contours_, fiiltered, cv::MorphTypes::MORPH_CLOSE, structEleme3);

	mask_ = cv::Mat::zeros(input.size(), CV_8UC3);
	cv::findContours(fiiltered, contourList, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	detections_.clear();
	if (forPreview_)
	{
		this->contoursFiltered_ = cv::Mat::zeros(input.size(), CV_8UC1);
	}

	int j = 0;
	for (int i = 0; i < contourList.size(); i++)
	{
		double area = cv::contourArea(contourList[i]);
		if (area > 20)
		{
			detections_.emplace_back(cv::boundingRect(contourList[i]));
			if (forPreview_)
			{
				cv::drawContours(this->contoursFiltered_, contourList, i, cv::Scalar(255), -1);
			}
			cv::rectangle(mask_, detections_[j++], cv::Scalar(255, 255, 255), -1);
		}
	}
}

Detector::Detector(bool forPreview)
{
	//bgSubtractor = cv::cuda::createBackgroundSubtractorMOG2();
	bgSubtractor_ = cv::createBackgroundSubtractorMOG2(500, 64);
	detections_.reserve(50);
	forPreview_ = forPreview;
}

void Detector::processFrame(const cv::Mat& inputFrame)
{
	bgSubtractor_->apply(inputFrame, this->foreground_);

	cv::Mat threshold;
	cv::threshold(this->foreground_, threshold, 127, 255, cv::ThresholdTypes::THRESH_BINARY);

	cv::Mat blur;
	cv::medianBlur(threshold, blur, 5);

	auto structEleme = cv::getStructuringElement(cv::MorphShapes::MORPH_RECT, cv::Size(13, 13));
	auto structEleme2 = cv::getStructuringElement(cv::MorphShapes::MORPH_ELLIPSE, cv::Size(15, 15));

	cv::Mat morph1;

	cv::morphologyEx(blur, morph1, cv::MorphTypes::MORPH_CLOSE, structEleme);
	cv::morphologyEx(morph1, this->foregroundFiltered_, cv::MorphTypes::MORPH_OPEN, structEleme2);

	getMaskByContours(this->foregroundFiltered_);

	cv::bitwise_and(inputFrame, mask_, roi_);

	//output = rawFg;
}

const cv::Mat& Detector::getOutput(DetectorOutputTypeEnum outputType) const
{
	switch (outputType)
	{
	case DetectorOutputTypeEnum::BG_SUBTRACTION:
		return this->foreground_;
	case DetectorOutputTypeEnum::BG_SUBTRACTION_FILTERED:
		return this->foregroundFiltered_;
	case DetectorOutputTypeEnum::CONTOURS:
		return this->contours_;
	case DetectorOutputTypeEnum::CONTOURS_FILTERED:
		return this->contoursFiltered_;
	case DetectorOutputTypeEnum::DETECTIONS:
		return this->roi_;
	}
}

const std::vector<cv::Rect>& Detector::getDetections() const
{
	return detections_;
}

bool Detector::isForPreview() const
{
	return forPreview_;
}

void Detector::setForPreview(bool forPreview)
{
	forPreview_ = forPreview;
}
