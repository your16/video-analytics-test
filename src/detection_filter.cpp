#include "detection_filter.h"

DetectionFilter::DetectableObj::DetectableObj(cv::Point point)
{
	posBuffer.push_back(point);
}

DetectionFilter::DetectionFilter(int erodeIterations, cv::Mat erodeStruct,
	int dilateIterations, cv::Mat dilateStruct, cv::Size blur, double threshold) :
	_erodeIter(erodeIterations), _erodeStruct(erodeStruct), _dilateIter(dilateIterations),
	_dilateStruct(dilateStruct), _blur(blur), _threshold(threshold) {}

void DetectionFilter::FrameProcessing(cv::Mat& frame, bool debug)
{
	cv::Mat proc;
	std::vector<DetectableObj> frameObjs;
	if (_sub.empty()) _sub = frame.clone();

	cv::absdiff(_sub, frame, proc);
	cv::cvtColor(proc, proc, cv::COLOR_RGB2GRAY);
	cv::blur(proc, proc, _blur);
	cv::threshold(proc, proc, _threshold, 255.0, cv::THRESH_BINARY);
	cv::erode(proc, proc, _erodeStruct, cv::Point(-1, -1), _erodeIter);
	cv::dilate(proc, proc, _dilateStruct, cv::Point(-1, -1), _dilateIter);

	_FindObjects(proc, frameObjs);
	_TrackObjects(frameObjs);
	_CheckLineIntersection(frame.rows / 2);
	_Info(frame);
	if (debug)
		_DebugInfo(frame);
}

void DetectionFilter::_FindObjects(const cv::Mat& mat, std::vector<DetectableObj>& frameObjs)
{
	if (!frameObjs.empty()) frameObjs.clear();

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(mat, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	if (!contours.empty())
	{
		if (frameObjs.capacity() < contours.size())
			frameObjs.reserve(contours.size());

		for (auto& contour : contours)
		{
			if (!contour.empty())
			{
				std::vector<cv::Point> hull;
				cv::convexHull(contour, hull);
				cv::Rect rect = cv::boundingRect(hull);
				if (rect.width >= MIN_RECT_W && rect.height >= MIN_RECT_H)
					frameObjs.emplace_back(DetectableObj(cv::Point(rect.x + (rect.width * 0.5),
						rect.y + (rect.height * 0.5))));
			}
		}
	}
}

void DetectionFilter::_TrackObjects(std::vector<DetectableObj>& frameObjs)
{
	if (_detectedObjs.empty() && !frameObjs.empty())
		_detectedObjs = frameObjs;
	else
	{
		for (auto obj = _detectedObjs.begin(); obj != _detectedObjs.end();)
		{
			double minDist = MIN_DIST_BETWEEN_POINTS;
			auto nearestObj = frameObjs.end();
			for (auto nobj = frameObjs.begin(); nobj != frameObjs.end(); nobj++)
			{
				double dist = sqrt(pow(abs(nobj->posBuffer.back().x - obj->posBuffer.back().x), 2) +
					pow(abs(nobj->posBuffer.back().y - obj->posBuffer.back().y), 2));
				if (dist < minDist)
				{
					nearestObj = nobj;
					minDist = dist;
				}
			}
			if (nearestObj != frameObjs.end())
			{
				if (obj->posBuffer.size() > MAX_POS_BUFFER_SIZE)
					obj->posBuffer.erase(obj->posBuffer.begin());
				obj->posBuffer.emplace_back(nearestObj->posBuffer.back());
				obj->inactiveFrames = 0;
				frameObjs.erase(nearestObj);
			}
			else
				obj->inactiveFrames++;
			obj = (obj->inactiveFrames >= MAX_INACTIVE_FRAME) ? _detectedObjs.erase(obj) : ++obj;
		}
		for (auto& nobj : frameObjs)
			_detectedObjs.emplace_back(nobj);
	}
}

void DetectionFilter::_CheckLineIntersection(const int hLine)
{
	for (auto& obj : _detectedObjs)
	{
		if (!obj.isChecked && obj.posBuffer.size() > 1)
		{
			bool ucheck = obj.posBuffer.front().y > hLine &&
				obj.posBuffer.back().y <= hLine;
			bool dcheck = obj.posBuffer.front().y < hLine&&
				obj.posBuffer.back().y >= hLine;

			_tCounter += (int)(ucheck || dcheck);
			_uCounter += (int)ucheck;
			_dCounter += (int)dcheck;

			obj.isChecked = (ucheck || dcheck);
		}
	}
}

void DetectionFilter::_Info(cv::Mat& frame)
{
	std::string infoTotal = "Total: " + std::to_string(_tCounter);
	std::string infoUp = "Up: " + std::to_string(_uCounter);
	std::string infoDown = "Down: " + std::to_string(_dCounter);

	cv::putText(frame, infoTotal, cv::Point(INFO_TEXT_POS_OFFSET, INFO_TEXT_POS_OFFSET), cv::FONT_HERSHEY_SIMPLEX, INFO_TEXT_SCALE, COLOR_BLACK);
	cv::putText(frame, infoUp, cv::Point(INFO_TEXT_POS_OFFSET, INFO_TEXT_POS_OFFSET * 2), cv::FONT_HERSHEY_SIMPLEX, INFO_TEXT_SCALE, COLOR_BLACK);
	cv::putText(frame, infoDown, cv::Point(INFO_TEXT_POS_OFFSET, INFO_TEXT_POS_OFFSET * 3), cv::FONT_HERSHEY_SIMPLEX, INFO_TEXT_SCALE, COLOR_BLACK);
}

void DetectionFilter::_DebugInfo(cv::Mat& frame)
{
	for (auto& object : _detectedObjs)
	{
		cv::Point pos = object.posBuffer.back();
		std::string posInfo = "[ " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + " ]";

		cv::circle(frame, pos, 2, COLOR_WHITE);
		cv::line(frame, cv::Point(pos.x + DEBUG_CROSS_OFFSET, pos.y),
			cv::Point(pos.x + DEBUG_CROSS_OFFSET * 2, pos.y), COLOR_WHITE);
		cv::line(frame, cv::Point(pos.x - DEBUG_CROSS_OFFSET, pos.y),
			cv::Point(pos.x - DEBUG_CROSS_OFFSET * 2, pos.y), COLOR_WHITE);
		cv::line(frame, cv::Point(pos.x, pos.y + DEBUG_CROSS_OFFSET),
			cv::Point(pos.x, pos.y + DEBUG_CROSS_OFFSET * 2), COLOR_WHITE);
		cv::line(frame, cv::Point(pos.x, pos.y - DEBUG_CROSS_OFFSET),
			cv::Point(pos.x, pos.y - DEBUG_CROSS_OFFSET * 2), COLOR_WHITE);

		cv::putText(frame, posInfo, cv::Point(pos.x + DEBUG_POS_INFO_OFFSET,
			pos.y - DEBUG_POS_INFO_OFFSET), cv::FONT_HERSHEY_SIMPLEX, DEBUG_POS_INFO_TEXT_SCALE, COLOR_WHITE);
	}
	cv::line(frame, cv::Point(0, frame.rows / 2), cv::Point(frame.cols, frame.rows / 2), COLOR_BLACK);
}