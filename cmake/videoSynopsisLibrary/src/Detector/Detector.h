#pragma once

#pragma warning(push, 0)
#include <opencv2/core.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/features2d.hpp>
#pragma warning(pop)


class Detector
{
	cv::Ptr<cv::BackgroundSubtractor> bgSubtractor_;
	std::vector<cv::Rect> detections_;
	cv::Mat foreground_;
	cv::Mat foregroundFiltered_;
	cv::Mat contours_;
	cv::Mat contoursFiltered_;
	cv::Mat mask_;
	cv::Mat roi_;


	void getMaskByContours(const cv::Mat& input, bool preview = false);
	void getMaskByClustering(const cv::Mat& input);

public:
	long long Microseconds;
	Detector();
	void processFrame(const cv::Mat& inputFrame, bool preview = false);
	void processFrameGPU(const cv::Mat& inputFrame);
	const cv::Mat& getOutput() const;
	const std::vector<cv::Rect>& getDetections() const;
};

