#pragma once
#include <memory>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/bgsegm.hpp>
#include "ExtractionAlgorithmEnum.h"
#include "FrameTypeEnum.h"

namespace vs {


	class MotionExtractor
	{
	protected:
		cv::Mat input;
		cv::Mat blur;
		cv::Mat morph;
		cv::Mat forProcessing;
		cv::Mat bg;
		cv::Mat fgMask;
		std::shared_ptr<cv::BackgroundSubtractor> bgSub;

		cv::Size blurKernelSize;
		double blurSigmaX;
		double blurSigmaY;

		cv::Mat morphKernel;
		cv::Point morphAnchor;
		int morphIteration;


		virtual void preExtraction();
		virtual void postExtraction();

		void init();

	public:

		MotionExtractor();
		MotionExtractor(ExtractionAlgorithm::ExtractionAlgorithmEnum algorithm);

		void setBlur(cv::Size blurKernelSize, double blurSigmaX, double blurSigmaY);
		void serMorph(cv::MorphShapes morphShape, cv::Size morphKernelSize, cv::Point morphAnchor, int morphIteration);

		virtual void apply(const cv::Mat& inputFrame);
		cv::Mat& getFrame(FrameType::FrameTypeEnum frameType);
	};

}