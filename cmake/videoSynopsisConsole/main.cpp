// VideoSynopsisConsole.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <opencv2/opencv.hpp>
#include <videoSynopsisLib.h>

int main()
{

	Detector detector(false);
	Tracker tracker(false);

	cv::VideoCapture video("1.mp4");

	int wait = 0;
	int add = 20;

	int i = 0;

	while (video.isOpened())
	{
		cv::Mat frame;
		video.read(frame);
		if (frame.empty())
		{
			break;
		}

		detector.processFrame(frame);
		if (i > 20)
		{
			tracker.processDetections(detector.getDetections(), i);
			tracker.drawTracks(frame);
		}
		for (auto&& element : detector.getDetections())
		{
			cv::rectangle(frame, element, { 0, 255, 0 });
		}

		cv::imshow("input", frame);
		cv::imshow("roi", detector.getOutput(DetectorOutputTypeEnum::DETECTIONS));
		int key = cv::waitKey(wait);
		if (key == 'w')
		{
			wait += add;
			add *= -1;
		}

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