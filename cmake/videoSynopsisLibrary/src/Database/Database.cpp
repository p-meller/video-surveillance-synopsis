#include "Database.h"
#include "DataTypes.h"


Database::Database(std::string dbPath) : trackStorage(initTracksStorage(dbPath)),
                                         detectionStorage(initDetectionsStorage(dbPath)) {
    trackStorage.sync_schema();
    detectionStorage.sync_schema();
}

void Database::setDb(std::string dbPath) {

}

void Database::addTracksToDb(std::vector<Track> trackList, int currentFrame) {
    //trackStorage.begin_transaction();

    for (auto &&track : trackList) {
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

        if (notDetectedFramesCount == 0) {
            --notDetectedFramesCount;
            detectionStorage.insert(dbDetection);
        }
        auto prevTracks = track.getPrevTracks();

        if (prevTracks.size() - notDetectedFramesCount <= 0) {
            continue;
        }
        frame = frame - notDetectedFramesCount;

        std::vector<DbDetection> prevDetections(prevTracks.size() - notDetectedFramesCount);
        for (int i = prevDetections.size() - 1; i > -1; --i) {
            const cv::Rect prev = prevTracks[i];
            prevDetections[i].trackId = track.getId();
            prevDetections[i].absolutFrame = --frame;
            prevDetections[i].x = prev.x;
            prevDetections[i].y = prev.y;
            prevDetections[i].height = prev.height;
            prevDetections[i].width = prev.width;
        }

        dbTrack.appearFrame = frame;
        trackStorage.insert(dbTrack);
        detectionStorage.insert_range(prevDetections.begin(), prevDetections.end());
    }

    //trackStorage.commit();

}

std::vector<DbTrack> Database::getAllTracks() {
    using namespace sqlite_orm;
    return trackStorage.get_all<DbTrack>(order_by(&DbTrack::trackId));
}

DbTrack Database::getTrackByTrackId(int trackId) {
    using namespace sqlite_orm;
    return trackStorage.get_all<DbTrack>(where(c(&DbTrack::trackId) == trackId))[0];
}


std::vector<DbTrack> Database::getAllTracksByDirection(DbTrack::Direction direction) {
    using namespace sqlite_orm;
    return trackStorage.get_all<DbTrack>(where(c(&DbTrack::direction) == static_cast<int>(direction)),
                                         order_by(&DbTrack::trackId));
}

std::vector<DbTrack> Database::getAllTracksAfterTrackId(int trackId, DbTrack::Direction direction) {
    using namespace sqlite_orm;
    return trackStorage.get_all<DbTrack>(
            where(c(&DbTrack::trackId) > trackId && c(&DbTrack::direction) == static_cast<int>(direction)),
            order_by(&DbTrack::trackId));
}

void Database::updateTrack(const DbTrack &track) {
    trackStorage.update(track);
}

void Database::removeTrack(int recId) {
    trackStorage.remove<DbTrack>(recId);
}

std::vector<DbDetection> Database::getAllDetections() {
    return detectionStorage.get_all<DbDetection>();
}

std::vector<DbDetection> Database::getDetectionsByTrackId(int trackId) {
    using namespace sqlite_orm;
    return detectionStorage.get_all<DbDetection>(where(c(&DbDetection::trackId) == trackId));
}

std::vector<DbDetection> Database::getDetectionsByFrameNum(int frameNum) {
    using namespace sqlite_orm;
    return detectionStorage.get_all<DbDetection>(where(c(&DbDetection::absolutFrame) == frameNum));
}

void Database::removeDetectionsByTrackId(int trackId) {
    using namespace sqlite_orm;
    detectionStorage.remove_all<DbDetection>(where(c(&DbDetection::trackId) == trackId));
}

void Database::clearTracks() {
    trackStorage.drop_table(DbTrack::getTableName());
    trackStorage.sync_schema();
}

void Database::clearDetections() {
    detectionStorage.drop_table(DbDetection::getTableName());
    detectionStorage.sync_schema();
}

std::vector<DbTrack> Database::getAllTracksBySynopsisAppearOrder(int synopsisAppearOrder) {
    using namespace sqlite_orm;
    return trackStorage.get_all<DbTrack>(where(c(&DbTrack::synopsisAppearOrder) == synopsisAppearOrder),
                                         order_by(&DbTrack::trackId));
}
