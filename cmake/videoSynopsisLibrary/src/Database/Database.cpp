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
		trackStorage.insert(dbTrack);

		int frame = currentFrame;

		DbDetection dbDetection{};
		dbDetection.trackId = track.getId();
		dbDetection.absolutFrame = frame;
		dbDetection.x = track.getCurrentTrack().x;
		dbDetection.y = track.getCurrentTrack().y;
		dbDetection.width = track.getCurrentTrack().width;
		dbDetection.height = track.getCurrentTrack().height;

		detectionStorage.insert(dbDetection);
		auto prevTracks = track.getPrevTracks();

		std::vector<DbDetection> prevDetections(prevTracks.size());
		for (int i = 0; i < prevTracks.size(); ++i)
		{
			const cv::Rect prev = prevTracks[i];
			prevDetections[i].trackId = track.getId();
			prevDetections[i].absolutFrame = --frame;
			prevDetections[i].x = prev.x;
			prevDetections[i].y = prev.y;
			prevDetections[i].height = prev.height;
			prevDetections[i].width = prev.width;
		}

		detectionStorage.insert_range(prevDetections.begin(),prevDetections.end());
	}

	//trackStorage.commit();

}


