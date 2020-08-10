#ifndef VIDEOSYNOPSIS_TRACKER_H
#define VIDEOSYNOPSIS_TRACKER_H

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

	bool saveToDb_;
	int frame;
	void removeTracks(const std::vector<int>& tracksToRemove);

public:
	explicit Tracker(bool saveToDb = false);

	~Tracker();

	void processDetections(const std::vector<cv::Rect>& detections, int currentFrame=-1);

	void drawTracks(cv::Mat& img);
};

#endif //VIDEOSYNOPSIS_TRACKER_H
