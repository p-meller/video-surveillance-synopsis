#pragma once
#include <vector>
#include <opencv2/core.hpp>

namespace operations_research
{
	class MPSolver;
}

//namespace cv
//{
//	class SURF;
//	class DescriptorMatcher;
//}

class Track;

class Tracker
{
	constexpr static int maxSimultaneousTracks = 20;
	constexpr static int maxEuclidDistance = 200;

//	cv::Ptr<cv::SURF> featureDetector;
//	cv::Ptr<cv::DescriptorMatcher> matcher;

	std::vector<Track> trackList;
	operations_research::MPSolver* assignmentSolver;

	std::vector<int> assignTracks(const std::vector<cv::Rect>& detections);
	void removeTracks(const std::vector<int>& tracksToRemove);

public:
	Tracker();
	~Tracker();

	void processDetections(const std::vector<cv::Rect>& detections);

	void drawTracks(cv::Mat& img);
};

