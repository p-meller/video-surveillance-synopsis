#include "Track.h"

unsigned long long Track::id_counter_ = 0;

constexpr int Track::getPrevTracksBufferSize() const
{
	return BUFFER_SIZE;
}

unsigned long long Track::generateID()
{
	return id_counter_++;
}

Track::Track(const cv::Rect& currentTrack) :
	current_track_(currentTrack),
	trackId(Track::generateID()),
	prevTrackZeroIndex(0),
	lostDetectionFrames(0),
	detectedFrames(1),
	trackedFrames(1),
	kalmanFilter({ currentTrack.x + currentTrack.width / 2, currentTrack.y + currentTrack.height })
{
}

void Track::setCurrentTrack(const cv::Rect& currentTrack)
{
	prevTrackZeroIndex = (++prevTrackZeroIndex) % BUFFER_SIZE;
	prev_tracks_[prevTrackZeroIndex] = current_track_;
	current_track_ = currentTrack;
}

cv::Rect Track::getCurrentTrack() const
{
	return current_track_;
}

int Track::getId() const
{
	return trackId;
}

int Track::isLongDetection() const
{
	return detectedFrames >= minDetectedFrameThreshold;
}


bool Track::incrementLostFrames()
{
	++lostDetectionFrames;
	++trackedFrames;
	return lostDetectionFrames >= lostFrameThreshold || detectedFrames < minDetectedFrameThreshold;
}

void Track::incrementDetectedFrames()
{
	lostDetectionFrames = 0;
	++detectedFrames;
	++trackedFrames;
}

void Track::update()
{
	kalmanFilter.update(cv::Point(), false);

	const cv::Point newPoint = kalmanFilter.getPrediction();

	cv::Rect newRect(current_track_);
	newRect.x = newPoint.x - newRect.width / 2;
	newRect.y = newPoint.x - newRect.width / 2;
	setCurrentTrack(newRect);
}

void Track::update(const cv::Rect& rect)
{
	kalmanFilter.update({ rect.x + rect.width / 2,rect.y + rect.height / 2 }, true);

	const cv::Point newPoint = kalmanFilter.getPrediction();

	cv::Rect newRect(rect);
	newRect.x = newPoint.x - newRect.width / 2;
	newRect.y = newPoint.y - newRect.height / 2;
	setCurrentTrack(newRect);
}

std::array<cv::Rect, Track::BUFFER_SIZE> Track::getPrevTracks() const
{
	std::array<cv::Rect, BUFFER_SIZE> temp;
	for (int i = 0; i < BUFFER_SIZE; i++)
	{
		temp[i] = prev_tracks_[(i + prevTrackZeroIndex) % BUFFER_SIZE];
	}
	return  temp;
}
