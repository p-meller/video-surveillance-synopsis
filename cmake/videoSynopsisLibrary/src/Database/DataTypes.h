#ifndef VIDEOSYNOPSIS_DATATYPES_H
#define VIDEOSYNOPSIS_DATATYPES_H

struct DbDetection{
	int recId;
	int trackId;
	int absolutFrame;
	int x;
	int y;
	int width;
	int height;
};

struct DbTrack {
	int recId;
	int trackId;
	std::string previewImagePath;
};


inline auto initTracksStorage(const std::string& path)
{
	using namespace sqlite_orm;
	return make_storage(path,
			make_table("tracks",
					make_column("recId", &DbTrack::recId, primary_key()),
					make_column("trackId", &DbTrack::trackId, unique()),
					make_column("previewImagePath", &DbTrack::previewImagePath)
			));
}

inline auto initDetectionsStorage(const std::string& path)
{
	using namespace sqlite_orm;
	return make_storage(path,
			make_table("detections",
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
