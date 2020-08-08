#pragma once
#include <opencv2/core.hpp>
#include "KalmanFilter.h"



class Track
{
	constexpr static int BUFFER_SIZE = 5;
	constexpr static int lostFrameThreshold = 5;
	constexpr static int minDetectedFrameThreshold = 5;

	static unsigned long long id_counter_;

	cv::Rect prev_tracks_[BUFFER_SIZE];
	cv::Rect current_track_;
	unsigned long long trackId;
	unsigned int prevTrackZeroIndex;
	int lostDetectionFrames;
	int detectedFrames;
	int trackedFrames;

	KalmanFilter kalmanFilter;

public:



	static unsigned long long generateID();


	explicit Track(const cv::Rect& currentTrack);

	void setCurrentTrack(const cv::Rect& currentTrack);
	cv::Rect getCurrentTrack() const;

	int getId() const;
	
	int isLongDetection() const;

	bool incrementLostFrames();
	void incrementDetectedFrames();

	void update();
	void update(const cv::Rect& rect);

	std::array<cv::Rect, BUFFER_SIZE> getPrevTracks() const;

	constexpr int getPrevTracksBufferSize() const;
};

