#pragma once
#include <opencv2/core.hpp>

class IFilter
{
public:
	virtual void FrameProcessing(cv::Mat& frame, bool debug) = 0;
	virtual ~IFilter() {}
};