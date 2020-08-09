#include "Tracker.h"
#include "Track.h"

//#include <ortools/base/logging.h>
#include <ortools/linear_solver/linear_solver.h>

std::vector<int> Tracker::assignTracks(const std::vector<cv::Rect>& detections)
{
	int workersNum;
	int tasksNum;
	std::vector<int> assignedTracks;

	if (detections.empty())
	{
		assignedTracks.resize(trackList.size(), -1);
		return assignedTracks;
	}
	else if (detections.size() == 1 && trackList.size() == 1)
	{
		assignedTracks.emplace_back(0);
		return assignedTracks;
	}
	else if (detections.size() == 1)
	{
		assignedTracks.resize(trackList.size(), -1);
		double minDistance = DBL_MAX;

		cv::Rect detRect = detections[0];
		int i = 0;
		for (int j = 0; trackList.size() > j; ++j)
		{
			cv::Rect elRect = trackList[j].getCurrentTrack();
			cv::Vec2d p1(detRect.x + detRect.width / 2.0, detRect.y + detRect.height / 2.0);
			cv::Vec2d p2(elRect.x + elRect.width / 2.0, elRect.y + elRect.height / 2.0);
			double distance = cv::norm(p1, p2);
			if (distance < minDistance)
			{
				i = j;
				minDistance = distance;
			}
		}
		assignedTracks[i] = 0;
		return assignedTracks;
	}
	else if (trackList.size() == 1)
	{
		double minDistance = DBL_MAX;

		cv::Rect detRect = trackList[0].getCurrentTrack();
		int i;
		for (int j = 0; detections.size() > j; ++j)
		{
			cv::Rect elRect = detections[j];
			cv::Vec2d p1(detRect.x + detRect.width / 2.0, detRect.y + detRect.height / 2.0);
			cv::Vec2d p2(elRect.x + elRect.width / 2.0, elRect.y + elRect.height / 2.0);
			double distance = cv::norm(p1, p2);
			if (distance < minDistance)
			{
				i = j;
				minDistance = distance;
			}
		}
		assignedTracks.emplace_back(i);
		return assignedTracks;
	}

	bool assignForTracks;

	if (trackList.size() >= detections.size())
	{
		assignForTracks = true;
		workersNum = trackList.size();
		tasksNum = detections.size();
	}
	else
	{
		assignForTracks = false;
		workersNum = detections.size();
		tasksNum = trackList.size();
	}



	//auto costMatrix = std::vector<std::vector<double>>(workersNum, std::vector<double>(tasksNum));
	cv::Mat costMatrix(workersNum, tasksNum, CV_64F);
	cv::Mat euclidCostMatrix(workersNum, tasksNum, CV_64F);

	double maxEuclid = 0;
	double maxSize = 0;
	//costMatrix.resize(currentTracksNum);
	for (int i = 0; i < workersNum; ++i)
	{
		//	costMatrix[i].resize(currentDetectionsNum);
		auto* costMatrixRow = costMatrix.ptr<double>(i);
		auto* euclidCostMatrixRow = euclidCostMatrix.ptr<double>(i);
		for (int j = 0; j < tasksNum; ++j)
		{
			cv::Rect workerRect;
			cv::Rect taskRect;

			if (assignForTracks)
			{
				workerRect = trackList[i].getCurrentTrack();
				taskRect = detections[j];
			}
			else
			{
				workerRect = detections[i];
				taskRect = trackList[j].getCurrentTrack();
			}

			cv::Vec2d workerPoint(workerRect.x + workerRect.width / 2.0, workerRect.y + workerRect.height / 2.0);
			cv::Vec2d taskPoint(taskRect.x + taskRect.width / 2.0, taskRect.y + taskRect.height / 2.0);
			cv::Vec2d workerSize(workerRect.width, workerRect.height);
			cv::Vec2d taskSize(taskRect.width, taskRect.height);
			euclidCostMatrixRow[j] = cv::norm(workerPoint, taskPoint);
			costMatrixRow[j] = cv::norm(workerSize, taskSize);
			maxEuclid = euclidCostMatrixRow[j] > maxEuclid ? euclidCostMatrixRow[j] : maxEuclid;
			maxSize = costMatrixRow[j] > maxSize ? costMatrixRow[j] : maxSize;
		}
	}

	costMatrix = costMatrix / maxSize * 0.5 + euclidCostMatrix / maxEuclid * 0.5;

	// Variables
	// x[i][j] is an array of 0-1 variables, which will be 1
	// if worker i is assigned to task j.
	std::vector<std::vector<const operations_research::MPVariable*>> x(
			workersNum, std::vector<const operations_research::MPVariable*>(tasksNum));
	for (int i = 0; i < workersNum; ++i)
	{
		for (int j = 0; j < tasksNum; ++j)
		{
			x[i][j] = assignmentSolver->MakeIntVar(0, 1, "");
		}
	}

	// Constraints
	// Each worker is assigned to at most one task.
	for (int i = 0; i < workersNum; ++i)
	{
		operations_research::LinearExpr worker_sum;
		for (int j = 0; j < tasksNum; ++j)
		{
			worker_sum += x[i][j];
		}
		assignmentSolver->MakeRowConstraint(worker_sum <= 1.0);
	}
	// Each task is assigned to exactly one worker.
	for (int j = 0; j < tasksNum; ++j)
	{
		operations_research::LinearExpr task_sum;
		for (int i = 0; i < workersNum; ++i)
		{
			task_sum += x[i][j];
		}
		assignmentSolver->MakeRowConstraint(task_sum == 1.0);
	}

	// Objective.
	operations_research::MPObjective* const objective = assignmentSolver->MutableObjective();
	for (int i = 0; i < workersNum; ++i)
	{
		auto* costMatrixRow = costMatrix.ptr<double>(i);
		for (int j = 0; j < tasksNum; ++j)
		{
			objective->SetCoefficient(x[i][j], costMatrixRow[j]);
		}
	}
	objective->SetMinimization();

	// Solve
	const operations_research::MPSolver::ResultStatus result_status = assignmentSolver->Solve();


	// Print solution.
	// Check that the problem has a feasible solution.
	if (operations_research::MPSolver::OPTIMAL != result_status &
		operations_research::MPSolver::FEASIBLE != result_status)
	{
		//LOG(WARNING) << "No solution found.";
	}

	//LOG(INFO) << "Total cost = " << objective->Value() << "\n\n";

	if (assignForTracks)
	{
		assignedTracks.resize(workersNum, -1);
	}
	else
	{
		assignedTracks.resize(tasksNum, -1);
	}

	for (int i = 0; i < workersNum; ++i)
	{
		for (int j = 0; j < tasksNum; ++j)
		{


			// Test if x[i][j] is 0 or 1 (with tolerance for floating point
			// arithmetic).
			if (x[i][j]->solution_value() > 0.5)
			{


				if (assignForTracks)
				{
					double cost = costMatrix.at<double>(i, j);
					//LOG(INFO) << "Worker " << i << " assigned to task " << j
					//	<< ".  Cost = " << cost;
					if (cost < maxEuclidDistance)
					{
						assignedTracks[i] = j;
					}
				}
				else
				{
					double cost = costMatrix.at<double>(j, i);
					//LOG(INFO) << "Worker " << j << " assigned to task " << i
					//	<< ".  Cost = " << cost;
					if (cost < maxEuclidDistance)
					{
						assignedTracks[j] = i;
					}
				}
			}
		}
	}

	assignmentSolver->Clear();
	return assignedTracks;
}

void Tracker::removeTracks(const std::vector<int>& tracksToRemove)
{
	for (unsigned int i = tracksToRemove.size(); i > 0; --i)
	{
		trackList.erase(trackList.begin() + tracksToRemove[i - 1]);
	}
}

Tracker::Tracker() : assignmentSolver(
		new operations_research::MPSolver("simple_mip_program", operations_research::MPSolver::GLOP_LINEAR_PROGRAMMING))
{
	trackList.reserve(maxSimultaneousTracks);
}

Tracker::~Tracker()
{
	delete assignmentSolver;
}

void Tracker::processDetections(const std::vector<cv::Rect>& detections)
{
	if (trackList.empty())
	{
		if (detections.empty())
		{
			return;
		}
		for (auto&& track : detections)
		{
			trackList.emplace_back(Track(track));
		}
	}

	auto assignedTracks = assignTracks(detections);

	std::vector<int> trackToRemove;
	trackToRemove.reserve(trackList.size());

	std::vector<int> notAssignedDetections(detections.size(), 1);


	for (auto i = 0u; i < assignedTracks.size(); ++i)
	{
		if (assignedTracks[i] != -1)
		{
			trackList[i].incrementDetectedFrames();
			trackList[i].update(detections[assignedTracks[i]]);
			if (!detections.empty())
			{
				notAssignedDetections[assignedTracks[i]] = 0;
			}
		}
		else
		{
			if (trackList[i].incrementLostFrames())
			{
				trackToRemove.emplace_back(i);
			}
			else
			{
				trackList[i].update();
			}
		}
	}


	removeTracks(trackToRemove);

	for (auto i = 0u; i < detections.size(); ++i)
	{
		if (notAssignedDetections[i])
		{
			trackList.emplace_back(Track(detections[i]));
		}
	}
}

void Tracker::drawTracks(cv::Mat& img)
{
	for (auto&& track : trackList)
	{
		if (track.isLongDetection())
		{
			cv::putText(img, std::to_string(track.getId()), { track.getCurrentTrack().x, track.getCurrentTrack().y },
					cv::FONT_HERSHEY_SIMPLEX, 1, { 255, 0, 0 });
			cv::rectangle(img, track.getCurrentTrack(), { 255, 0, 0 });
		}
		else
		{
			cv::putText(img, std::to_string(track.getId()), { track.getCurrentTrack().x, track.getCurrentTrack().y },
					cv::FONT_HERSHEY_SIMPLEX, 1, { 0, 0, 255 });
			cv::rectangle(img, track.getCurrentTrack(), { 0, 0, 255 });
		}
	}
}
