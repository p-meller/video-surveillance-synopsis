#ifndef VIDEOSYNOPSIS_UTILS_H
#define VIDEOSYNOPSIS_UTILS_H

#include <string>
#include "../Database/Database.h"
#include <opencv2/opencv.hpp>


void filterTracks()
{
	auto allTracks = Database::getInstance().getAllTracks();

	std::vector<int> trackIdsForRemove;

	for (auto&& track : allTracks)
	{
		auto allTrackDetection = Database::getInstance().getDetectionsByTrackId(track.trackId);

		cv::Vec2d centroid(0, 0);
		for (auto&& detection : allTrackDetection)
		{
			centroid.val[0] += detection.x;
			centroid.val[1] += detection.y;
		}
		centroid = centroid / static_cast<double>(allTrackDetection.size());

		double meanDistance = 0;

		for (auto&& detection : allTrackDetection)
		{
			cv::Vec2d point(detection.x, detection.y);
			meanDistance += cv::norm(centroid, point);
		}
		meanDistance = meanDistance / static_cast<float>(allTrackDetection.size());

		if (meanDistance < 100)
		{
			trackIdsForRemove.push_back(track.recId);
		}
	}

	if(!trackIdsForRemove.empty()){
		for (auto&& recId: trackIdsForRemove)
		{
			Database::getInstance().removeTrack(recId);
		}
	}
}

void savePreviewImagesForAllTracks(const std::string& videoPath)
{
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
	}

	cv::VideoCapture video(videoPath);

	int i = 0;

	while (video.isOpened())
	{
		bool found = false;
		cv::Mat frame;
		video.read(frame);

		if (frame.empty())
		{
			break;
		}

//		cv::Mat frameRect;
//		frame.copyTo(frameRect);

		auto it = previewImageFramesForTracks.find(i);
		if (it != previewImageFramesForTracks.end())
		{
			found = true;
			for (auto&& detection : it->second)
			{
				int width = detection.width;
				int height = detection.height;
//				width = width + detection.x >= 1280 ? width + (1280 - (width + detection.x + 1)) : width;
//				height = height + detection.y >= 720 ? height + (720 - (height + detection.y + 1)) : height;
				cv::Rect roi(detection.x, detection.y, width, height);
				cv::Mat previewImage = frame(roi);
//				cv::rectangle(frameRect,roi,{255,0,0});
				cv::imwrite("./preview/" + std::to_string(detection.trackId) + ".png", previewImage);
			}
		}
		i++;
//		cv::imshow("input", frameRect);
//		cv::waitKey(10);
//		int a=0;
	}
}

#endif //VIDEOSYNOPSIS_UTILS_H
