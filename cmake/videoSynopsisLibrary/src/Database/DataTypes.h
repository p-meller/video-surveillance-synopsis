#ifndef VIDEOSYNOPSIS_DATATYPES_H
#define VIDEOSYNOPSIS_DATATYPES_H

struct DbDetection
{
	int recId;
	int trackId;
	int absolutFrame;
	int x;
	int y;
	int width;
	int height;

	static std::string getTableName()
	{
		return "detections";
	}
};



struct DbTrack
{
	enum Direction {
		None,
		Right,
		Up,
		Left,
		Down
	};
	int recId;
	int trackId;
	int appearFrame;
	int previewFrame;
	double fps;
	std::string previewImagePath;
	int direction;
	int synopsisAppearOrder;
	int nextTrackId=-1;
	int nextTrackFrameOffset;


	static std::string getTableName()
	{
		return "tracks";
	}
};


inline auto initTracksStorage(const std::string& path)
{
	using namespace sqlite_orm;
	return make_storage(path,
			make_table(DbTrack::getTableName(),
					make_column("recId", &DbTrack::recId, primary_key()),
					make_column("trackId", &DbTrack::trackId, unique()),
					make_column("appearFrame", &DbTrack::appearFrame),
					make_column("previewFrame", &DbTrack::previewFrame),
					make_column("fps", &DbTrack::fps),
					make_column("previewImagePath", &DbTrack::previewImagePath),
					make_column("direction", &DbTrack::direction, default_value(DbTrack::Direction::None)),
					make_column("synopsisAppearOrder", &DbTrack::synopsisAppearOrder, default_value(0)),
					make_column("nextTrackId", &DbTrack::nextTrackId, default_value(-1)),
					make_column("nextTrackFrameOffset", &DbTrack::nextTrackFrameOffset)
			));
}

inline auto initDetectionsStorage(const std::string& path)
{
	using namespace sqlite_orm;
	return make_storage(path,
			make_table(DbDetection::getTableName(),
					make_column("recId", &DbDetection::recId, primary_key()),
					make_column("trackId", &DbDetection::trackId),
					make_column("absoluteFrame", &DbDetection::absolutFrame),
					make_column("x", &DbDetection::x),
					make_column("y", &DbDetection::y),
					make_column("width", &DbDetection::width),
					make_column("height", &DbDetection::height)
			));
}

using TracksStorage = decltype(initTracksStorage(""));
using DetectionsStorage = decltype(initDetectionsStorage(""));

#endif //VIDEOSYNOPSIS_DATATYPES_H
