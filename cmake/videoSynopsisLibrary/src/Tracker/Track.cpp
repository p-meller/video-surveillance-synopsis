#include "Track.h"

unsigned int Track::id_counter_ = 0;

constexpr int Track::getPrevTracksBufferSize()
{
	return BUFFER_SIZE;
}

int Track::generateID()
{
	return id_counter_++;
}

Track::Track(const cv::Rect& currentTrack) :
		current_track_(currentTrack),
		trackId(Track::generateID()),
		//prevTrackZeroIndex(0),
		lostDetectionFrames(0),
		detectedFrames(1),
		trackedFrames(1),
		valid(false),
		kalmanFilter({ currentTrack.x + currentTrack.width / 2, currentTrack.y + currentTrack.height })
{
}

void Track::setCurrentTrack(const cv::Rect& currentTrack)
{
	prev_tracks_.push_back(current_track_);
	this->current_track_ = currentTrack;
//	prevTrackZeroIndex = (++prevTrackZeroIndex) % BUFFER_SIZE;
//	prev_tracks_[prevTrackZeroIndex] = current_track_;
//	current_track_ = currentTrack;
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
	if(!valid && detectedFrames >= minDetectedFrameThreshold)
	{
		valid=true;
	}

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
	kalmanFilter.update({ rect.x + rect.width / 2, rect.y + rect.height / 2 }, true);

	const cv::Point newPoint = kalmanFilter.getPrediction();

	cv::Rect newRect(rect);
	newRect.x = newPoint.x - newRect.width / 2;
	newRect.y = newPoint.y - newRect.height / 2;
	setCurrentTrack(newRect);
}

const std::vector<cv::Rect>& Track::getPrevTracks() const
{
	return prev_tracks_;
}

bool Track::isValid() const
{
	return valid;
}

int Track::getLostFrames()
{
	return lostDetectionFrames;
}
