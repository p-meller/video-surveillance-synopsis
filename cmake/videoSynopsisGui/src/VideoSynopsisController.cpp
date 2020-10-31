#include "VideoSynopsisController.h"
#include <QDir>
#include <QDebug>

void VideoSynopsisWorker::processVideo(const QString& videoPath)
{
	Database::getInstance().clearTracks();
	Database::getInstance().clearDetections();


	{
		QDir dir("preview");
		dir.setNameFilters(QStringList() << "*.*");
		dir.setFilter(QDir::Files);
				foreach(auto&& dirFile, dir.entryList())
			{
				dir.remove(dirFile);
			}
	}
	{
		QDir dir("previewFull");
		dir.setNameFilters(QStringList() << "*.*");
		dir.setFilter(QDir::Files);
				foreach(auto&& dirFile, dir.entryList())
			{
				dir.remove(dirFile);
			}
	}
	{
		QDir dir("bg");
		dir.setNameFilters(QStringList() << "*.*");
		dir.setFilter(QDir::Files);
		for (auto&& dirFiles : dir.entryList())
		{
			dir.remove(dirFiles);
		}
	}
	QDir().mkdir("preview");
	QDir().mkdir("previewFull");
	QDir().mkdir("bg");

	std::string path = videoPath.toStdString();
	cv::VideoCapture video(path);
	int i = 0;
	bool backgroundReady = false;

	int frameCount = video.get(cv::CAP_PROP_FRAME_COUNT);

	while (video.isOpened())
	{
		cv::Mat frame;
		video.read(frame);
		if (frame.empty())
		{
			break;
		}

		if (!backgroundReady)
		{
			const int modelingFrameCount = 500;
			detector.backgroundModeling(frame);
			++i;
			if (i > modelingFrameCount)
			{
				i = 0;
				video.set(cv::CAP_PROP_POS_FRAMES, 0);
				backgroundReady = true;
				detector.saveBg();
				cv::imwrite("./bg/bg.png", detector.getBg());

			}
			emit progressMessage("Background modeling", i, modelingFrameCount);
			continue;
		}

		detector.processFrame(frame);
		if (i > 5)
		{
			tracker.processDetections(detector.getDetections(), i);
		}
		++i;
		emit progressMessage("Object detection and tracking", i, frameCount);
	}

	emit progressMessage("Tracks filtering", 1, 1);
	filterTracks();

	auto allTracks = Database::getInstance().getAllTracks();

	std::map<int, std::vector<DbDetection>> previewImageFramesForTracks;

	for (auto&& track : allTracks)
	{
		auto allTrackDetection = Database::getInstance().getDetectionsByTrackId(track.trackId);

		int maxArea = 0;
		int maxAreaFrame = 0;
		DbDetection maxAreaDetection;
		for (auto&& detection : allTrackDetection)
		{
			int area = detection.width * detection.height;
			bool valid = detection.x >= 0 && detection.y >= 0 && detection.x + detection.width - 1 < 1280 &&
						 detection.y + detection.height - 1 < 720;
			if (valid && area > maxArea)
			{
				maxArea = area;
				maxAreaFrame = detection.absolutFrame;
				maxAreaDetection = detection;
			}
		}

		auto it = previewImageFramesForTracks.find(maxAreaFrame);
		if (it != previewImageFramesForTracks.end())
		{
			it->second.push_back(maxAreaDetection);
		}
		else
		{
			previewImageFramesForTracks.emplace(
					std::make_pair(maxAreaFrame, std::vector<DbDetection>(1, maxAreaDetection)));
		}

		track.previewFrame=maxAreaFrame;
		Database::getInstance().updateTrack(track);
	}


	emit progressMessage("Tracks direction calculating", 1, 1);
	calculateDirectionsForAllTrack();

	video.set(cv::CAP_PROP_POS_FRAMES, 0);
	i = 0;

	double fps = video.get(cv::CAP_PROP_FPS);
	long frameLengthInMicroseconds = 1000000 / fps;

	emit progressMessage("Tracks preview saving", 0, frameCount);
	while (video.isOpened())
	{
		bool found = false;
		cv::Mat frame;
		video.read(frame);

		if (frame.empty())
		{
			break;
		}

		auto it = previewImageFramesForTracks.find(i);
		if (it != previewImageFramesForTracks.end())
		{
			found = true;
			for (auto&& detection : it->second)
			{
				cv::Size expandSize(detection.width * 0.2, detection.height * 0.2);
				cv::Rect roi(detection.x, detection.y, detection.width, detection.height);

				roi.x -= expandSize.width / 2;
				roi.width += expandSize.width / 2;
				roi.y -= expandSize.height / 2;
				roi.height += expandSize.height / 2;

				if (roi.x < 0)
				{
					roi.x -= roi.x;
					roi.width += roi.x;
				}
				if (roi.y < 0)
				{
					roi.y -= roi.y;
					roi.height += roi.y;
				}

				cv::Size frameSize(frame.cols, frame.rows);
				roi.width = roi.x + roi.width > frameSize.width ? roi.width - ((roi.width + roi.x) - frameSize.width)
																: roi.width;
				roi.height =
						roi.y + roi.height > frameSize.height ? roi.height - ((roi.height + roi.y) - frameSize.height)
															  : roi.height;

				cv::Mat previewImage = frame(roi);

				auto seconds = std::to_string((i * frameLengthInMicroseconds / 1000000) % 60);
				auto minutes = std::to_string(i * frameLengthInMicroseconds / (60 * 1000000));

				std::string time =
						(minutes.length() == 1 ? "0" + minutes : minutes) + ":" +
						(seconds.length() == 1 ? "0" + seconds : seconds);

				cv::imwrite("./preview/" + std::to_string(detection.trackId) + "_" + time + "_.png", previewImage);
				cv::imwrite("./previewFull/" + std::to_string(detection.trackId) + "_" + time + "_.png", frame);


				auto track = Database::getInstance().getTrackByTrackId(detection.trackId);
				if (track.fps != fps)
				{
					track.fps = fps;
					Database::getInstance().updateTrack(track);
				}
			}
			emit progressMessage("Tracks preview saving", i, frameCount);
		}
		i++;
	}

	emit progressMessage("Finish", 0, 0);
}

void VideoSynopsisWorker::createSimpleVideoSynopsis(const QString& videoPath)
{
	QDir("synopsis").removeRecursively();
	QDir().mkdir("synopsis");

	cv::VideoCapture inputVideo(videoPath.toStdString());

	int frameCount = inputVideo.get(cv::CAP_PROP_FRAME_COUNT);

	cv::VideoWriter outputVideo;
	cv::Size S(static_cast<int>(inputVideo.get(cv::CAP_PROP_FRAME_WIDTH)),
			static_cast<int>(inputVideo.get(cv::CAP_PROP_FRAME_HEIGHT)));
	int fourcc = cv::VideoWriter::fourcc('X', '2', '6', '4');
	double fps = inputVideo.get(cv::CAP_PROP_FPS);
	long frameLengthInMicroseconds = 1000000 / fps;
	outputVideo.open("./synopsis/simpleSynopsis.avi", fourcc, fps, S, true);

	//cv::Mat bg = cv::imread(QDir("bg").absoluteFilePath("bg.png").toStdString());

	int i = 0;
	while (inputVideo.isOpened())
	{


		cv::Mat inputFrame;
		inputVideo.read(inputFrame);
		if (inputFrame.empty())
		{
			break;
		}

		auto detections = Database::getInstance().getDetectionsByFrameNum(i - 1);

		i++;
		emit progressMessage("Creating synopsis", i, frameCount);

		if (detections.empty())
		{
			continue;
		}



		//cv::Mat outputFrame;

		//inputFrame.copyTo(outputFrame);

		for (auto&& detection : detections)
		{
			cv::Rect roi(detection.x, detection.y, detection.width, detection.height);

			cv::Size frameSize(inputFrame.cols, inputFrame.rows);
			roi.width = roi.x + roi.width > frameSize.width ? roi.width - ((roi.width + roi.x) - frameSize.width)
															: roi.width;
			roi.height =
					roi.y + roi.height > frameSize.height ? roi.height - ((roi.height + roi.y) - frameSize.height)
														  : roi.height;

			if (roi.x < 0 || roi.y < 0 || roi.x + roi.width > frameSize.width ||
				roi.y + roi.height > frameSize.height || roi.width < 1 || roi.height < 1)
			{
				continue;
			}


			auto seconds = std::to_string((i * frameLengthInMicroseconds / 1000000) % 60);
			auto minutes = std::to_string(i * frameLengthInMicroseconds / (60 * 1000000));

			std::string time =
					(minutes.length() == 1 ? "0" + minutes : minutes) + ":" +
					(seconds.length() == 1 ? "0" + seconds : seconds);

            cv::putText(inputFrame, std::to_string(detection.trackId) + "   " + time, { roi.x, roi.y },
                        cv::FONT_HERSHEY_SIMPLEX, 0.7, { 0, 0, 0 },3);
            cv::putText(inputFrame, std::to_string(detection.trackId) + "   " + time, { roi.x, roi.y },
                        cv::FONT_HERSHEY_SIMPLEX, 0.7, { 255, 255, 255 },2);

            qDebug() << ("Frame num:" + std::to_string(i) + "  trackId: " + std::to_string(detection.trackId)).c_str();
		}

		outputVideo.write(inputFrame);

	}

	emit progressMessage("Finish", 0, 0);

}

void VideoSynopsisWorker::createVideoSynopsis(const QString& videoPath)
{
	QDir("synopsis").removeRecursively();
	QDir().mkdir("synopsis");

	cv::VideoCapture inputVideo(videoPath.toStdString());

	int frameCount = inputVideo.get(cv::CAP_PROP_FRAME_COUNT);

	cv::VideoWriter outputVideo;
	cv::Size S(static_cast<int>(inputVideo.get(cv::CAP_PROP_FRAME_WIDTH)),
			static_cast<int>(inputVideo.get(cv::CAP_PROP_FRAME_HEIGHT)));
	int fourcc = cv::VideoWriter::fourcc('X', '2', '6', '4');
	double fps = inputVideo.get(cv::CAP_PROP_FPS);
	long frameLengthInMicroseconds = 1000000 / fps;
	outputVideo.open("./synopsis/synopsis.avi", fourcc, fps, S, true);

	cv::Mat bg = cv::imread(QDir("bg").absoluteFilePath("bg.png").toStdString());

	for (auto&& track : Database::getInstance().getAllTracks())
	{
		track.nextTrackId = -1;
		track.synopsisAppearOrder = 0;
		track.nextTrackFrameOffset = 0;
		Database::getInstance().updateTrack(track);
	}

	for (int i = 1; i < 5; ++i)
	{
		auto tracks = Database::getInstance().getAllTracksByDirection(DbTrack::Direction(i));
		if (!tracks.empty())
		{
			for (int j = 0; j < tracks.size() - 1; ++j)
			{
				calculateSynopsisOrderForNextTracks(tracks[j].trackId);
				emit progressMessage("Calculating colisions", j, tracks.size());
			}
		}
	}

	int i = 0;

	std::map<int, TrackImage> trackImages;
	while (inputVideo.isOpened())
	{

		cv::Mat inputFrame;
		inputVideo.read(inputFrame);
		if (inputFrame.empty())
		{
			break;
		}

		auto detections = Database::getInstance().getDetectionsByFrameNum(i - 1);

		i++;
		emit progressMessage("Creating synopsis", i, frameCount);

		if (detections.empty())
		{
			continue;
		}

		for (auto&& detection : detections)
		{

			cv::Rect roi(detection.x, detection.y, detection.width, detection.height);

			cv::Size frameSize(inputFrame.cols, inputFrame.rows);
			roi.width = roi.x + roi.width > frameSize.width ? roi.width - ((roi.width + roi.x) - frameSize.width)
															: roi.width;
			roi.height =
					roi.y + roi.height > frameSize.height ? roi.height - ((roi.height + roi.y) - frameSize.height)
														  : roi.height;

			if (roi.x < 0 || roi.y < 0 || roi.x + roi.width > frameSize.width ||
				roi.y + roi.height > frameSize.height || roi.width < 1 || roi.height < 1)
			{
				continue;
			}

			cv::Mat frameRoi = inputFrame(roi);

			auto it = trackImages.find(detection.trackId);
			if (it == trackImages.end())
			{
				auto currentTrack = Database::getInstance().getTrackByTrackId(detection.trackId);
				trackImages.emplace(std::make_pair(detection.trackId, TrackImage()));
				it = trackImages.find(detection.trackId);
			}
			it->second.images.push_back(frameRoi.clone());
			it->second.rois.push_back(roi);
		}

	}


	int orderCount = 0;
	auto trackList = Database::getInstance().getAllTracksBySynopsisAppearOrder(0);

	int frameNum = 0;

	int tracksToSave = trackImages.size();

    std::vector<std::pair<std::string, cv::Rect>> textList;
    textList.reserve(trackList.size());
	while (!trackImages.empty())
	{
		cv::Mat outputFrame = bg.clone();

		for (int i = 0; i < trackList.size(); ++i)
		{
            qDebug() << (std::to_string(i) + "   " + std::to_string(trackList[i].trackId)).c_str();

            auto it = trackImages.find(trackList[i].trackId);
			if (it != trackImages.end())
			{
				auto trackImage = it->second;
				auto roi = trackImage.rois[trackImage.currentFrame];
				auto frameRoi = outputFrame(roi);

				auto seconds = std::to_string(
						((trackList[i].appearFrame + trackImage.currentFrame) * frameLengthInMicroseconds / 1000000) %
						60);
				auto minutes = std::to_string(
						(trackList[i].appearFrame + trackImage.currentFrame) * frameLengthInMicroseconds /
						(60 * 1000000));

				std::string time =
						(minutes.length() == 1 ? "0" + minutes : minutes) + ":" +
						(seconds.length() == 1 ? "0" + seconds : seconds);

				int trackId = trackList[i].trackId;
				if(trackId==13){
                    qDebug() << "Dodaje 13";
				}

				textList.emplace_back(std::to_string(trackList[i].trackId) + "   " + time, roi);

				trackImage.images[trackImage.currentFrame].copyTo(frameRoi);
				it->second.currentFrame += 1;
				if (it->second.currentFrame == trackList[i].nextTrackFrameOffset)
				{
					if (trackList[i].nextTrackId != -1)
					{
                        if(trackId==13){
                            qDebug() << "Dodaje 13 bo offset";
                        }
						trackList.push_back(Database::getInstance().getTrackByTrackId(trackList[i].nextTrackId));
						trackList[i].nextTrackId = -1;
					}
				}
				if (it->second.currentFrame == trackImage.images.size())
				{
					if (trackList[i].nextTrackId != -1)
					{
                        if(trackId==13){
                            qDebug() << "Dodaje 13 bo koniec";
                        }
						trackList.push_back(Database::getInstance().getTrackByTrackId(trackList[i].nextTrackId));
						trackList[i].nextTrackId = -1;
					}
					trackImages.erase(it);
				}
				//it->second.
			}
		}
		for(auto &&text : textList){
            cv::putText(outputFrame, text.first, { text.second.x, text.second.y },
                        cv::FONT_HERSHEY_SIMPLEX, 0.7, { 0, 0, 0 },4);
            cv::putText(outputFrame, text.first, { text.second.x, text.second.y },
                        cv::FONT_HERSHEY_SIMPLEX, 0.7, { 255, 255, 255 },2);
		}
		textList.clear();
		emit progressMessage("Saving synopsis", tracksToSave - trackImages.size(), tracksToSave);
		outputVideo.write(outputFrame);
		qDebug() << ("Frame num:" + std::to_string(frameNum++) + "  trackImagesSize: " +
					 std::to_string(trackImages.size())).c_str();
	}


	emit progressMessage("Finish", 0, 0);
}

VideoSynopsisWorker::VideoSynopsisWorker() : detector(), tracker(true)
{

}

int VideoSynopsisController::getVideoMilisecondsForPreview(const QString& trackIdStr)
{
	int trackId = std::stoi(trackIdStr.toStdString());

	auto track = Database::getInstance().getTrackByTrackId(trackId);
	int ms = 1.0 / track.fps * 1000 * (track.previewFrame+1);
	return ms;
}
