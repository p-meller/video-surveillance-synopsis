#pragma once
#include <vector>
#include <boost/container/static_vector.hpp>
#include <opencv2/core.hpp>
#include "KalmanFilter.h"
#include "Track.h"
#include <sqlite_modern_cpp.h>



namespace operations_research
{
	class MPSolver;
}

namespace cv
{
	class SURF;
	class DescriptorMatcher;
}

class Tracker
{
	constexpr static int maxSimultaneousTracks = 20;
	constexpr static int maxEuclidDistance = 200;

	cv::Ptr<cv::SURF> featureDetector;
	cv::Ptr<cv::DescriptorMatcher> matcher;

	std::vector<Track> trackList;
	operations_research::MPSolver* assignmentSolver;
	//boost::container::static_vector<boost::container::static_vector<double, maxSimultaneousTracks>, maxSimultaneousTracks> costMatrix;

	std::vector<int> assignTracks(const std::vector<cv::Rect>& detections);
	void removeTracks(const std::vector<int>& tracksToRemove);

	sqlite::database db;
public:

	
	Tracker(const std::string& database);
	~Tracker();

	void processDetections(const std::vector<cv::Rect>& detections);
	void saveTracks(int frameNum);

	void drawTracks(cv::Mat& img);
};

