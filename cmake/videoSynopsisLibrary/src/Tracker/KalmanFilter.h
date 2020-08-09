#ifndef VIDEOSYNOPSIS_KALMANFILTER_H
#define VIDEOSYNOPSIS_KALMANFILTER_H

#include <opencv2/video/tracking.hpp>

class KalmanFilter : public cv::KalmanFilter
{
	cv::Point2f LastPrediction;
public:

	KalmanFilter() = delete;

	explicit KalmanFilter(const cv::Point& point);

	KalmanFilter(const cv::Point& point, float dt, float accelerationNoise);

	cv::Point getPrediction();

	cv::Point update(const cv::Point& point, bool withCorrection);
};

#endif //VIDEOSYNOPSIS_KALMANFILTER_H
