#ifndef VIDEOSYNOPSIS_DETECTOR_H
#define VIDEOSYNOPSIS_DETECTOR_H

#include <opencv2/core.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/features2d.hpp>
#include "DetectorOutputTypeEnum.h"
#include "DetectorSettings.h"

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

	DetectorSettings settings_;

	bool forPreview_;

	void getMaskByContours(const cv::Mat& input);

public:
	explicit Detector(bool forPreview = false);

	void processFrame(const cv::Mat& inputFrame);

	const cv::Mat& getOutput(DetectorOutputTypeEnum outputType) const;

	const std::vector<cv::Rect>& getDetections() const;

	const DetectorSettings& GetSettings() const;

	void SetSettings(const DetectorSettings& settings);

	bool isForPreview() const;

	void setForPreview(bool forPreview);

};

#endif //VIDEOSYNOPSIS_DETECTOR_H
