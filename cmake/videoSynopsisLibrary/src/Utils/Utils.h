#ifndef VIDEOSYNOPSIS_UTILS_H
#define VIDEOSYNOPSIS_UTILS_H

#include <string>
#include "../Database/Database.h"
#include <opencv2/opencv.hpp>
#include <cmath>

inline void filterTracks()
{
	auto allTracks = Database::getInstance().getAllTracks();

	std::vector<DbTrack> trackIdsForRemove;

	for (auto&& track : allTracks)
	{
		auto allTrackDetection = Database::getInstance().getDetectionsByTrackId(track.trackId);

		if (allTrackDetection.size() < 5)
		{
			trackIdsForRemove.push_back(track);
			continue;
		}

		bool remove = false;

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
			remove = detection.x < 0 || detection.y < 0 ? true : remove;
			cv::Vec2d point(detection.x, detection.y);
			meanDistance += cv::norm(centroid, point);
		}
		meanDistance = meanDistance / static_cast<float>(allTrackDetection.size());

		if (meanDistance < 50 || remove)
		{
			trackIdsForRemove.push_back(track);
		}
	}

	if (!trackIdsForRemove.empty())
	{
		for (auto&& track: trackIdsForRemove)
		{
			Database::getInstance().removeTrack(track.recId);
			Database::getInstance().removeDetectionsByTrackId(track.trackId);
		}
	}
}

inline void calculateDirectionsForAllTrack()
{
	auto allTracks = Database::getInstance().getAllTracks();

	std::vector<int> trackIdsForRemove;

	for (auto&& track : allTracks)
	{
		bool first = true;
		int x = 0;
		int y = 0;

		int right = 0, up = 0, left = 0, down = 0;

		auto allTrackDetection = Database::getInstance().getDetectionsByTrackId(track.trackId);

		for (auto&& detection : allTrackDetection)
		{
			if (first)
			{
				x = detection.x;
				y = detection.y;
				first = false;
			}
			else
			{
				int dirX = detection.x - x;
				int dirY = y - detection.y;
				x = detection.x;
				y = detection.y;
				if (dirX == 0 && dirY == 0)
				{
					continue;
				}
				int direction = atan2(dirX, dirY);

				if (direction < M_PI / 4.0 && direction > -M_PI / 4.0)
				{
					++right;
				}
				else if (direction < 3 * M_PI / 4.0 && direction > M_PI / 4.0)
				{
					++up;
				}
				else if (direction > 3 * M_PI / 4.0 && direction < -3 * M_PI / 4.0)
				{
					++left;
				}
				else
				{
					++down;
				}
			}
		}

		int max = 0;
		if (right > max)
		{
			track.direction = DbTrack::Right;
			max = right;
		}
		if (up > max)
		{
			track.direction = DbTrack::Up;
			max = up;
		}
		if (left > max)
		{
			track.direction = DbTrack::Left;
			max = left;
		}
		if (down > max)
		{
			track.direction = DbTrack::Down;
		}

		Database::getInstance().updateTrack(track);
	}
}

inline void calculateSynopsisOrderForNextTracks(int currentTrackId)
{
	auto currentTrack = Database::getInstance().getTrackByTrackId(currentTrackId);

	auto ATrackDetections = Database::getInstance().getDetectionsByTrackId(currentTrack.trackId);

	auto nextTracks = Database::getInstance().getAllTracksAfterTrackId(currentTrack.trackId);

	//bool overlap = ((A & B).area() > 0);

	bool overlap = false;

	for (auto&& track : nextTracks)
	{
		auto BTrackDetections = Database::getInstance().getDetectionsByTrackId(track.trackId);


		for (int i = 0; i < ATrackDetections.size(); ++i)
		{
			auto roiA = ATrackDetections[i];
			cv::Rect rectA(roiA.x, roiA.y, roiA.width, roiA.height);
			for (int j = 0; i < BTrackDetections.size(); ++i)
			{
				auto roiB = BTrackDetections[j];
				cv::Rect rectB(roiB.x, roiB.y, roiB.width, roiB.height);
				if ((rectA & rectB).area() > 0)
				{
					overlap = true;
					break;
				}
			}
			if (overlap)
			{
				break;
			}
		}
		if (overlap)
		{
			currentTrack.nextTrackFrameOffset = 100;
			currentTrack.nextTrackId = track.trackId;
			track.synopsisAppearOrder = currentTrack.synopsisAppearOrder + 1;
			Database::getInstance().updateTrack(currentTrack);
			Database::getInstance().updateTrack(track);
			break;
		}
	}
}

inline void savePreviewImagesForAllTracks(const std::string& videoPath)
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
