#pragma once

#pragma warning(push, 0)
#include <opencv2/core.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/features2d.hpp>
#pragma warning(pop)


class Detector
{
	cv::Ptr<cv::BackgroundSubtractor> bg_subtractor_;
	cv::Ptr<cv::SimpleBlobDetector> blob_detector_;
	std::vector<cv::Rect> detections_;
	cv::Mat input_;
	cv::Mat mask_;
	cv::Mat output_;


	void getMaskByContours(const cv::Mat& input);
	void getMaskByClustering(const cv::Mat& input);

public:
	long long Microseconds;
	Detector();
	void processFrame(const cv::Mat& inputFrame);
	void processFrameGPU(const cv::Mat& inputFrame);
	const cv::Mat& getOutput() const;
	const std::vector<cv::Rect>& getDetections() const;
};

