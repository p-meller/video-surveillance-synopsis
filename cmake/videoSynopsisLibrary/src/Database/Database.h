#ifndef VIDEOSYNOPSIS_DATABASE_H
#define VIDEOSYNOPSIS_DATABASE_H

#include "../Tracker/Track.h"
#include <sqlite_orm/sqlite_orm.h>
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

	std::vector<DbDetection> getAllDetections();

	std::vector<DbDetection> getDetectionsByTrackId(int trackId);

};

#endif //VIDEOSYNOPSIS_DATABASE_H
