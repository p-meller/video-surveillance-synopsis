#ifndef VIDEOSYNOPSIS_TRACK_H
#define VIDEOSYNOPSIS_TRACK_H

#include <opencv2/core.hpp>
#include "KalmanFilter.h"


class Track
{
	constexpr static int BUFFER_SIZE = 5;
	constexpr static int lostFrameThreshold = 5;
	constexpr static int minDetectedFrameThreshold = 5;

	static unsigned int id_counter_;

	//cv::Rect prev_tracks_[BUFFER_SIZE];
	std::vector<cv::Rect> prev_tracks_;
	cv::Rect current_track_;
	int trackId;
	//unsigned int prevTrackZeroIndex;
	int lostDetectionFrames;
	int detectedFrames;
	int trackedFrames;
	bool valid;

	KalmanFilter kalmanFilter;

public:


	static int generateID();


	explicit Track(const cv::Rect& currentTrack);

	void setCurrentTrack(const cv::Rect& currentTrack);

	cv::Rect getCurrentTrack() const;

	int getId() const;

	int isLongDetection() const;

	bool incrementLostFrames();

	void incrementDetectedFrames();

	void update();

	void update(const cv::Rect& rect);

	const std::vector<cv::Rect>& getPrevTracks() const;

	static constexpr int getPrevTracksBufferSize();

	bool isValid() const;

	int getLostFrames();
};

#endif //VIDEOSYNOPSIS_TRACK_H
