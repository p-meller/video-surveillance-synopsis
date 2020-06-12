#include "MotionExtractor.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/bgsegm.hpp>
namespace vs {
	void MotionExtractor::preExtraction()
	{
		//cv::GaussianBlur(this->input, this->blur, this->blurKernelSize, this->blurSigmaX, this->blurSigmaY);
		//this->forProcessing = this->blur;
		this->forProcessing = this->input;
	}
	void MotionExtractor::postExtraction()
	{
		cv::morphologyEx(this->fgMask, this->morph, cv::MorphTypes::MORPH_DILATE, this->morphKernel, this->morphAnchor, this->morphIteration);
	}

	void MotionExtractor::init()
	{
		setBlur(cv::Size(3, 3), 0, 0);
		serMorph(cv::MorphShapes::MORPH_CROSS, cv::Size(3, 3), cv::Point(-1, -1), 1);
	}

	MotionExtractor::MotionExtractor() : MotionExtractor::MotionExtractor(ExtractionAlgorithm::MOG2)
	{
		init();
	}

	MotionExtractor::MotionExtractor(ExtractionAlgorithm::ExtractionAlgorithmEnum algorithm)
	{
		init();
		switch (algorithm)
		{
		case ExtractionAlgorithm::KNN:
			this->bgSub = cv::createBackgroundSubtractorKNN();
			break;
		case ExtractionAlgorithm::MOG2:
			this->bgSub = cv::createBackgroundSubtractorMOG2();
			break;
		case ExtractionAlgorithm::CNT:
			this->bgSub = cv::bgsegm::createBackgroundSubtractorCNT();
			break;
		case ExtractionAlgorithm::GMG:
			this->bgSub = cv::bgsegm::createBackgroundSubtractorGMG();
			break;
		case ExtractionAlgorithm::GSOC:
			this->bgSub = cv::bgsegm::createBackgroundSubtractorGMG();
			break;
		case ExtractionAlgorithm::LSBP:
			this->bgSub = cv::bgsegm::createBackgroundSubtractorGMG();
			break;
		case ExtractionAlgorithm::MOG:
			this->bgSub = cv::bgsegm::createBackgroundSubtractorGMG();
			break;
		default:
			break;
		}
	}

	void MotionExtractor::setBlur(cv::Size blurKernelSize, double blurSigmaX, double blurSigmaY)
	{
		this->blurKernelSize = blurKernelSize;
		this->blurSigmaX = blurSigmaX;
		this->blurSigmaY = blurSigmaY;
	}

	void MotionExtractor::serMorph(cv::MorphShapes morphShape, cv::Size morphKernelSize, cv::Point morphAnchor, int morphIteration)
	{

		this->morphKernel = cv::getStructuringElement(morphShape, morphKernelSize, morphAnchor);
		this->morphAnchor = morphAnchor;
		this->morphIteration = morphIteration;
	}

	void MotionExtractor::apply(const cv::Mat& inputFrame)
	{
		this->input = inputFrame;
		preExtraction();
		bgSub->apply(this->forProcessing, this->fgMask);
		bgSub->getBackgroundImage(this->bg);
		postExtraction();
	}
	cv::Mat& MotionExtractor::getFrame(FrameType::FrameTypeEnum frameType)
	{
		switch (frameType)
		{
		case FrameType::INPUT:
			return this->input;
			break;
		case FrameType::BLUR:
			return this->blur;
			break;
		case FrameType::MORPH:
			return this->morph;
			break;
		case FrameType::BG:
			return this->bg;
			break;
		case FrameType::FG_MASK:
			return this->fgMask;
			break;
		default:
			throw std::exception("Get frame error");
			break;
		}
	}
}
