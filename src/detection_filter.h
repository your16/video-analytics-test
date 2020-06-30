#pragma once
#include <vector>
#include <string>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "interfaces/ifilter.h"

class DetectionFilter : public IFilter
{
public:
	DetectionFilter() = default;

	DetectionFilter(int erodeIterations, cv::Mat erodeStruct,
		int dilateIterations, cv::Mat dilateStruct, cv::Size blur, double threshold);

	DetectionFilter(const DetectionFilter& obj) = delete;
	DetectionFilter(DetectionFilter&& obj) noexcept = delete;
	~DetectionFilter() = default;

	DetectionFilter& operator=(const DetectionFilter& obj) = delete;
	DetectionFilter& operator=(DetectionFilter&& obj) noexcept = delete;

	void FrameProcessing(cv::Mat& frame, bool debug = false) override;

private:
	struct DetectableObj
	{
		bool isChecked = false;
		int inactiveFrames = 0;
		std::vector<cv::Point> posBuffer;

		DetectableObj(cv::Point point);
		DetectableObj(const DetectableObj& obj) = default;
		DetectableObj(DetectableObj&& obj) noexcept = default;
		~DetectableObj() = default;

		DetectableObj& operator=(const DetectableObj& obj) = default;
		DetectableObj& operator=(DetectableObj&& obj) noexcept = default;
	};

	const int INFO_TEXT_POS_OFFSET = 25;
	const double INFO_TEXT_SCALE = 0.5;

	const int DEBUG_CROSS_OFFSET = 5;
	const int DEBUG_POS_INFO_OFFSET = 15;
	const double DEBUG_POS_INFO_TEXT_SCALE = 0.35;

	const cv::Scalar COLOR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
	const cv::Scalar COLOR_WHITE = cv::Scalar(255.0, 255.0, 255.0);

	const size_t MAX_POS_BUFFER_SIZE = 2;
	const unsigned int MAX_INACTIVE_FRAME = 8;
	const unsigned int MIN_RECT_W = 25;
	const unsigned int MIN_RECT_H = 25;
	const double MIN_DIST_BETWEEN_POINTS = 35.0;

	int _tCounter = 0;
	int _uCounter = 0;
	int _dCounter = 0;

	int _erodeIter = 1;
	int _dilateIter = 3;
	double _threshold = 75.0;
	cv::Size _blur = cv::Size(1, 1);
	cv::Mat _erodeStruct = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
	cv::Mat _dilateStruct = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
	cv::Mat _sub;

	std::vector<DetectableObj> _detectedObjs;

	void _FindObjects(const cv::Mat& mat, std::vector<DetectableObj>& frameObjs);
	void _TrackObjects(std::vector<DetectableObj>& frameObjs);
	void _CheckLineIntersection(const int hLine);
	void _Info(cv::Mat& frame);
	void _DebugInfo(cv::Mat& frame);
};
