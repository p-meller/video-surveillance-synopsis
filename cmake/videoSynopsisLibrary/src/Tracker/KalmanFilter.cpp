#include "KalmanFilter.h"
#include <cmath>

KalmanFilter::KalmanFilter(const cv::Point& point) : cv::KalmanFilter(4, 2)
{
	LastPrediction = point;

	{
		float data[16] = {
				1, 0, 1, 0,
				0, 1, 0, 1,
				0, 0, 1, 0,
				0, 0, 0, 1
		};
		transitionMatrix = cv::Mat(4, 4, CV_32F, data).clone();
	}

	{
		float data[4] = { static_cast<float>(point.x), static_cast<float>(point.y), 0, 0 };
		statePre = cv::Mat(4, 1, CV_32F, data).clone();
	}
	{
		float data[4] = { static_cast<float>(point.x), static_cast<float>(point.y), 0, 0 };
		statePost = cv::Mat(4, 1, CV_32F, data).clone();
	}

	measurementMatrix = cv::Mat::eye(2, 4, CV_32F);

	measurementNoiseCov = cv::Mat::eye(2, 2, CV_32F) * 0.01;
	errorCovPost = cv::Mat::eye(4, 4, CV_32F) * 0.1;
}

KalmanFilter::KalmanFilter(const cv::Point& point, float dt, float accelerationNoise) : KalmanFilter(point)
{
	float data[16] = {
			dt, 0, static_cast<float>(std::pow(dt, 3) / 2), 0,
			0, static_cast<float>(std::pow(dt, 4) / 4), 0, static_cast<float>(std::pow(dt, 3) / 2),
			static_cast<float>(std::pow(dt, 3) / 2), 0, static_cast<float>(std::pow(dt, 2)), 0,
			0, static_cast<float>(std::pow(dt, 3) / 2), 0, static_cast<float>(std::pow(dt, 2))
	};

	processNoiseCov = cv::Mat(4, 4, CV_32F, data).clone() * accelerationNoise;
}

cv::Point KalmanFilter::getPrediction()
{
	cv::Mat prediction = predict();
	auto* matPtr = prediction.ptr<float>(0);
	LastPrediction.x = matPtr[0];
	LastPrediction.y = matPtr[1];

	return LastPrediction;
}

cv::Point KalmanFilter::update(const cv::Point& point, bool withCorrection)
{
	cv::Mat measurement(2, 1, CV_32F);
	auto* data = measurement.ptr<float>(0);
	if (withCorrection)
	{
		data[0] = point.x;
		data[1] = point.y;
	}
	else
	{
		data[0] = LastPrediction.x;
		data[1] = LastPrediction.y;
	}

	cv::Mat estimation = correct(measurement);
	auto* matPtr = estimation.ptr<float>(0);
	LastPrediction.x = matPtr[0];
	LastPrediction.y = matPtr[1];

	return LastPrediction;
}
