#include "Database.h"
#include "DataTypes.h"


Database::Database(std::string dbPath) : trackStorage(initTracksStorage(dbPath)),
										 detectionStorage(initDetectionsStorage(dbPath))
{
	trackStorage.sync_schema();
	detectionStorage.sync_schema();
}

void Database::setDb(std::string dbPath)
{

}

void Database::addTracksToDb(std::vector<Track> trackList, int currentFrame)
{
	//trackStorage.begin_transaction();

	for (auto&& track : trackList)
	{
		DbTrack dbTrack{};
		dbTrack.trackId = track.getId();

		int frame = currentFrame;

		DbDetection dbDetection{};
		dbDetection.trackId = track.getId();
		dbDetection.absolutFrame = frame;
		dbDetection.x = track.getCurrentTrack().x;
		dbDetection.y = track.getCurrentTrack().y;
		dbDetection.width = track.getCurrentTrack().width;
		dbDetection.height = track.getCurrentTrack().height;

		int notDetectedFramesCount = track.getLostFrames();

		if (notDetectedFramesCount == 0)
		{
			--notDetectedFramesCount;
			detectionStorage.insert(dbDetection);
		}
		auto prevTracks = track.getPrevTracks();

		if (prevTracks.size() - notDetectedFramesCount <= 0)
		{
			continue;
		}
		frame = frame - notDetectedFramesCount;

		std::vector<DbDetection> prevDetections(prevTracks.size() - notDetectedFramesCount);
		for (int i = prevDetections.size() - 1; i > -1; --i)
		{
			const cv::Rect prev = prevTracks[i];
			prevDetections[i].trackId = track.getId();
			prevDetections[i].absolutFrame = --frame;
			prevDetections[i].x = prev.x;
			prevDetections[i].y = prev.y;
			prevDetections[i].height = prev.height;
			prevDetections[i].width = prev.width;
		}

		trackStorage.insert(dbTrack);
		detectionStorage.insert_range(prevDetections.begin(), prevDetections.end());
	}

	//trackStorage.commit();

}

std::vector<DbTrack> Database::getAllTracks()
{
	return trackStorage.get_all<DbTrack>();
}

void Database::updateTrack(const DbTrack& track)
{
	trackStorage.update(track);
}

void Database::removeTrack(int recId)
{
	trackStorage.remove<DbTrack>(recId);
}

std::vector<DbDetection> Database::getAllDetections()
{
	return detectionStorage.get_all<DbDetection>();
}

std::vector<DbDetection> Database::getDetectionsByTrackId(int trackId)
{
	using namespace sqlite_orm;
	return detectionStorage.get_all<DbDetection>(where(c(&DbDetection::trackId) == trackId));
}
