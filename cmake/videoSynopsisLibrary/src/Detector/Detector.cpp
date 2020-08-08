#pragma warning(push, 0)
#include "Detector.h"
#include <opencv2/imgproc.hpp>
#include <chrono>
#include <opencv2/features2d.hpp>
#include <opencv2/core/operations.hpp>
//#include <opencv2/cudabgsegm.hpp>
//#include <opencv2/cudaarithm.hpp>
//#include <opencv2/cudafilters.hpp>
#pragma warning(pop)


void Detector::getMaskByContours(const cv::Mat& input, bool preview)
{
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(input, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);


	cv::Mat filterMask = cv::Mat::zeros(input.size(), CV_8UC1);
	for (int i = 0; i < contours.size(); i++)
	{
		double area = cv::contourArea(contours[i]);
		if (area > 20) {
			cv::drawContours(filterMask, contours, i, cv::Scalar(255), -1);
		}
	}

	cv::Mat fiiltered;

	auto structEleme3 = cv::getStructuringElement(cv::MorphShapes::MORPH_ELLIPSE, cv::Size(35, 35));
	cv::morphologyEx(filterMask, fiiltered, cv::MorphTypes::MORPH_CLOSE, structEleme3);

	mask_ = cv::Mat::zeros(input.size(), CV_8UC3);
	cv::findContours(fiiltered, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	detections_.clear();
	int j = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		double area = cv::contourArea(contours[i]);
		if (area > 20)
		{
			detections_.emplace_back(cv::boundingRect(contours[i]));
			cv::rectangle(mask_, detections_[j++], cv::Scalar(255, 255, 255), -1);
		}
	}
}

void Detector::getMaskByClustering(const cv::Mat& input)
{
	// Get all non black points
	std::vector<cv::Point> pts;
	findNonZero(input, pts);

	// Define the distance between clusters
	int euclidean_distance = 20;

	// Apply partition 
	// All pixels within the the given distance will belong to the same cluster

	std::vector<int> labels;

	// With functor
	//int n_labels = partition(pts, labels, EuclideanDistanceFunctor(euclidean_distance));

	// With lambda function
	int th2 = euclidean_distance * euclidean_distance;
	int n_labels = partition(pts, labels, [th2](const cv::Point& lhs, const cv::Point& rhs) {
		return ((lhs.x - rhs.x) * (lhs.x - rhs.x) + (lhs.y - rhs.y) * (lhs.y - rhs.y)) < th2;
		});


	// Store all points in same cluster, and compute centroids
	std::vector<std::vector<cv::Point>> clusters(n_labels);
	std::vector<cv::Point> centroids(n_labels, cv::Point(0, 0));

	for (int i = 0; i < pts.size(); ++i)
	{
		clusters[labels[i]].push_back(pts[i]);
		centroids[labels[i]] += pts[i];
	}
	for (int i = 0; i < n_labels; ++i)
	{
		centroids[i].x /= clusters[i].size();
		centroids[i].y /= clusters[i].size();
	}

	// Draw results

	// Build a vector of random color, one for each class (label)
	std::vector<cv::Vec3b> colors;
	for (int i = 0; i < n_labels; ++i)
	{
		colors.push_back(cv::Vec3b(rand() & 255, rand() & 255, rand() & 255));
	}

	// Draw the points
	mask_ = cv::Mat::zeros(input.size(), CV_8UC3);
	cv::Mat3b res(mask_.rows, mask_.cols, cv::Vec3b(0, 0, 0));
	
	for (int i = 0; i < pts.size(); ++i)
	{
		res(pts[i]) = colors[labels[i]];
	}

	// Draw centroids
	for (int i = 0; i < n_labels; ++i)
	{
		circle(res, centroids[i], 3, cv::Scalar(colors[i][0], colors[i][1], colors[i][2]), cv::FILLED);
		circle(res, centroids[i], 6, cv::Scalar(255 - colors[i][0], 255 - colors[i][1], 255 - colors[i][2]));
	}

	mask_ = res;
	
}

Detector::Detector()
{
	//bgSubtractor = cv::cuda::createBackgroundSubtractorMOG2();
	bg_subtractor_ = cv::createBackgroundSubtractorMOG2(500,64);
	detections_.reserve(50);
}

void Detector::processFrame(const cv::Mat& inputFrame, bool preview)
{
	auto start = std::chrono::high_resolution_clock::now();

	cv::Mat rawFg;
	bg_subtractor_->apply(inputFrame, rawFg);

	cv::Mat threshold;
	cv::threshold(rawFg, threshold, 127, 255, cv::ThresholdTypes::THRESH_BINARY);

	cv::Mat blur;
	cv::medianBlur(threshold, blur, 5);

	auto structEleme = cv::getStructuringElement(cv::MorphShapes::MORPH_RECT, cv::Size(13, 13));
	auto structEleme2 = cv::getStructuringElement(cv::MorphShapes::MORPH_ELLIPSE, cv::Size(15, 15));

	cv::Mat morph1;
	cv::Mat morph2;

	cv::morphologyEx(blur, morph1, cv::MorphTypes::MORPH_CLOSE, structEleme);
	cv::morphologyEx(morph1, morph2, cv::MorphTypes::MORPH_OPEN, structEleme2);

	getMaskByContours(morph2);
	//getMaskByClustering(morph2);



	cv::bitwise_and(inputFrame, mask_, output_);

	//output = rawFg;

	auto stop = std::chrono::high_resolution_clock::now();

	this->Microseconds = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
}

void Detector::processFrameGPU(const cv::Mat& inputFrame)
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

	//cv::Mat morph2Finished;
	//morph2.download(morph2Finished);

	//std::vector<std::vector<cv::Point> > contours;
	//std::vector<cv::Vec4i> hierarchy;
	//cv::findContours(morph2Finished, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);


	//cv::Mat filterMask = cv::Mat::zeros(inputFrame.size(), CV_8UC1);
	//for (int i = 0; i < contours.size(); i++)
	//{
	//	double area = cv::contourArea(contours[i]);
	//	if (area > 20) {
	//		cv::drawContours(filterMask, contours, i, cv::Scalar(255), -1);
	//	}
	//}

	//cv::Mat fiiltered;

	//auto structEleme3 = cv::getStructuringElement(cv::MorphShapes::MORPH_ELLIPSE, cv::Size(29, 29));
	//cv::morphologyEx(filterMask, fiiltered, cv::MorphTypes::MORPH_DILATE, structEleme3);

	//cv::Mat filterMask2 = cv::Mat::zeros(inputFrame.size(), CV_8UC3);
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

const cv::Mat& Detector::getOutput() const
{
	return output_;
}

const std::vector<cv::Rect>& Detector::getDetections() const
{
	return detections_;
}
