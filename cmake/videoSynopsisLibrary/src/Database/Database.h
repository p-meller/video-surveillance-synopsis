#ifndef VIDEOSYNOPSIS_DATABASE_H
#define VIDEOSYNOPSIS_DATABASE_H

#include "../Tracker/Track.h"
#include "../Utils/sqlite_orm.h"
#include "DataTypes.h"

class Database
{
public:
	static Database& getInstance(std::string dbPath)
	{
		static Database instance(dbPath);
		return instance;
	}

	static Database& getInstance()
	{
		return getInstance("default.sqlite");
	}

private:
	explicit Database(std::string dbPath);

	TracksStorage trackStorage;
	DetectionsStorage detectionStorage;

public:
	Database(Database const&) = delete;

	void operator=(Database const&) = delete;

	void setDb(std::string dbPath);

	void addTracksToDb(std::vector<Track> trackList, int currentFrame);

	void updateTrack(const DbTrack& track);

	void removeTrack(int recId);

	std::vector<DbTrack> getAllTracks();

	DbTrack getTrackByTrackId(int trackId);

	std::vector<DbTrack> getAllTracksByDirection(DbTrack::Direction direction);

	std::vector<DbTrack> getAllTracksAfterTrackId(int trackId, DbTrack::Direction direction);

	std::vector<DbTrack> getAllTracksBySynopsisAppearOrder(int synopsisAppearOrder);

	std::vector<DbDetection> getAllDetections();

	std::vector<DbDetection> getDetectionsByTrackId(int trackId);

	std::vector<DbDetection> getDetectionsByFrameNum(int frameNum);

	void removeDetectionsByTrackId(int trackId);

	void clearTracks();

	void clearDetections();

};

#endif //VIDEOSYNOPSIS_DATABASE_H
