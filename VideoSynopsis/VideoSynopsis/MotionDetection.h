#pragma once
#include <opencv2/core.hpp>
namespace vs {

	class MotionDetection
	{


	public:
		void getMotion(const cv::Mat& inputFrame, cv::Mat& outputFrame);
	};
}