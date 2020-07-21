#pragma warning(push, 0)
#include "Detector.h"
#include <opencv2/imgproc.hpp>
#include <chrono>
//#include <opencv2/cudabgsegm.hpp>
//#include <opencv2/cudaarithm.hpp>
//#include <opencv2/cudafilters.hpp>
#pragma warning(pop)


Detector::Detector()
{
	//bgSubtractor = cv::cuda::createBackgroundSubtractorMOG2();
	bgSubtractor = cv::createBackgroundSubtractorMOG2();
}

void Detector::processFrame(const cv::UMat& inputFrame)
{
	auto start = std::chrono::high_resolution_clock::now();

	//cv::Mat NinputFrame = inputFrame.getMat(cv::ACCESS_READ);

	cv::UMat rawFg;
	bgSubtractor->apply(inputFrame, rawFg);

	cv::UMat threshold;
	cv::threshold(rawFg, threshold, 127, 255, cv::ThresholdTypes::THRESH_BINARY);

	cv::UMat blur;
	cv::medianBlur(threshold, blur, 5);

	auto structEleme = cv::getStructuringElement(cv::MorphShapes::MORPH_RECT, cv::Size(5, 5));
	auto structEleme2 = cv::getStructuringElement(cv::MorphShapes::MORPH_RECT, cv::Size(9, 9));

	cv::UMat morph1;
	cv::UMat morph2;

	cv::morphologyEx(blur, morph1, cv::MorphTypes::MORPH_CLOSE, structEleme);
	cv::morphologyEx(morph1, morph2, cv::MorphTypes::MORPH_OPEN, structEleme2);

	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(morph2, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);


	cv::Mat filterMask = cv::Mat::zeros(inputFrame.size(), CV_8UC1);
	for (int i = 0; i < contours.size(); i++)
	{
		double area = cv::contourArea(contours[i]);
		if (area > 20) {
			cv::drawContours(filterMask, contours, i, cv::Scalar(255), -1);
		}
	}

	cv::Mat fiiltered;

	auto structEleme3 = cv::getStructuringElement(cv::MorphShapes::MORPH_ELLIPSE, cv::Size(29, 29));
	cv::morphologyEx(filterMask, fiiltered, cv::MorphTypes::MORPH_DILATE, structEleme3);

	cv::Mat filterMask2 = cv::Mat::zeros(inputFrame.size(), CV_8UC3);
	cv::findContours(fiiltered, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < contours.size(); i++)
	{
		double area = cv::contourArea(contours[i]);
		if (area > 20)
		{
			cv::rectangle(filterMask2, cv::boundingRect(contours[i]), cv::Scalar(255,255,255), -1);
		}
	}


	cv::bitwise_and(inputFrame, filterMask2, output);

	//output = rawFg;

	auto stop = std::chrono::high_resolution_clock::now();

	this->microseconds = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
}

void Detector::processFrameGPU(const cv::UMat& inputFrame)
{
	//auto start = std::chrono::high_resolution_clock::now();

	//cv::cuda::GpuMat input;

	//input.upload(inputFrame);

	//cv::cuda::GpuMat rawFg;
	//bgSubtractor->apply(input, rawFg);

	//cv::cuda::GpuMat threshold;
	//cv::cuda::threshold(rawFg, threshold, 127, 255, cv::ThresholdTypes::THRESH_BINARY);


	//cv::Ptr<cv::cuda::Filter> median = cv::cuda::createMedianFilter(CV_8UC1, 5);
	//cv::cuda::GpuMat blur;
	//median->apply(threshold, blur);

	//auto structEleme = cv::getStructuringElement(cv::MorphShapes::MORPH_RECT, cv::Size(5, 5));
	//auto structEleme2 = cv::getStructuringElement(cv::MorphShapes::MORPH_RECT, cv::Size(9, 9));

	//cv::cuda::GpuMat morph1;
	//cv::cuda::GpuMat morph2;
	//cv::Ptr<cv::cuda::Filter> morphEx1 = cv::cuda::createMorphologyFilter(cv::MorphTypes::MORPH_CLOSE, CV_8UC1, structEleme);
	//morphEx1->apply(blur, morph1);
	//cv::Ptr<cv::cuda::Filter> morphEx2 = cv::cuda::createMorphologyFilter(cv::MorphTypes::MORPH_OPEN, CV_8UC1, structEleme2);
	//morphEx1->apply(morph1, morph2);

	//cv::UMat morph2Finished;
	//morph2.download(morph2Finished);

	//std::vector<std::vector<cv::Point> > contours;
	//std::vector<cv::Vec4i> hierarchy;
	//cv::findContours(morph2Finished, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);


	//cv::UMat filterMask = cv::UMat::zeros(inputFrame.size(), CV_8UC1);
	//for (int i = 0; i < contours.size(); i++)
	//{
	//	double area = cv::contourArea(contours[i]);
	//	if (area > 20) {
	//		cv::drawContours(filterMask, contours, i, cv::Scalar(255), -1);
	//	}
	//}

	//cv::UMat fiiltered;

	//auto structEleme3 = cv::getStructuringElement(cv::MorphShapes::MORPH_ELLIPSE, cv::Size(29, 29));
	//cv::morphologyEx(filterMask, fiiltered, cv::MorphTypes::MORPH_DILATE, structEleme3);

	//cv::UMat filterMask2 = cv::UMat::zeros(inputFrame.size(), CV_8UC3);
	//cv::findContours(fiiltered, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	//for (int i = 0; i < contours.size(); i++)
	//{
	//	double area = cv::contourArea(contours[i]);
	//	if (area > 20)
	//	{
	//		cv::rectangle(filterMask2, cv::boundingRect(contours[i]), cv::Scalar(255, 255, 255), -1);
	//	}
	//}

	//cv::bitwise_and(inputFrame, filterMask2, output);

	////output = rawFg;

	//auto stop = std::chrono::high_resolution_clock::now();

	//this->microseconds = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
}

cv::UMat& Detector::getOutput()
{
	return output;
}
