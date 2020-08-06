// VideoSynopsisConsole.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#pragma warning(push, 0)
#include <iostream>
#include <Detector.h>
#include <opencv2/opencv.hpp>
#pragma warning(pop)

#include "Tracker.h"

int main()
{

	Detector detector;
	Tracker tracker("db");

	cv::VideoCapture video("1.mp4");

	int wait = 0;
	int add = 20;

	int i = 0;

	while (video.isOpened())
	{
		cv::Mat frame;
		video.read(frame);
		int nextFrame = static_cast<int>(video.get(cv::CAP_PROP_POS_FRAMES));

		cv::resize(frame, frame, { 1280,720 });

		detector.processFrame(frame);
		if (i > 20)
		{
			tracker.processDetections(detector.getDetections());
			tracker.drawTracks(frame);
			tracker.saveTracks(i);
		}
		for (auto&& element : detector.getDetections())
		{
			cv::rectangle(frame, element, { 0,255,0 });
		}

		cv::imshow("window", frame);
		cv::imshow("mask", detector.getOutput());
		int key = cv::waitKeyEx(wait);
		if (key == 'w')
		{
			wait += add;
			add *= -1;
		}
		else if (key == 2424832)//left
		{
			nextFrame = nextFrame < 1000 ? 0 : nextFrame - 1000;
			video.set(cv::CAP_PROP_POS_FRAMES, nextFrame);
		}
		else if (key == 2555904)//right
		{
			video.set(cv::CAP_PROP_POS_FRAMES, nextFrame + 1000);
		}

		printf("frameNum = %d, milliseconds = %3f\n", i, detector.Microseconds / 1000.0);
		++i;
	}


	std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file