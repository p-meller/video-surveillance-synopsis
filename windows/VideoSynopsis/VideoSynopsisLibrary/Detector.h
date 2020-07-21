#pragma once

#include <opencv2/core.hpp>
#include <opencv2/video/background_segm.hpp>

class Detector
{
	cv::Ptr<cv::BackgroundSubtractor> bgSubtractor;
	cv::UMat input;
	cv::UMat output;
	

public:
	long long microseconds;
	Detector();
	void processFrame(const cv::UMat& inputFrame);
	void processFrameGPU(const cv::UMat& inputFrame);
	cv::UMat& getOutput();
};

