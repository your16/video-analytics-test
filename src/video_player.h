#pragma once
#include <iostream>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "interfaces/ifilter.h"

class VideoPlayer
{
public:
private:
	const char* MAIN_WINDOW_NAME = "Video Analytics, desu!";
	const unsigned int FRAME_RATE = 1000 / 60;

	bool _init = false;
	bool _debugMode = false;

	cv::VideoCapture _capture;

public:
	VideoPlayer(const char* fileName);
	VideoPlayer(const VideoPlayer& obj) = delete;
	VideoPlayer(VideoPlayer&& obj) noexcept = delete;
	~VideoPlayer() = default;

	VideoPlayer& operator = (const VideoPlayer& obj) = default;
	VideoPlayer& operator = (VideoPlayer&& obj) noexcept = default;

	bool ok() const;

	void Init(bool debug = false, const char* filename = nullptr);
	void SetDebugMode(bool value);
	void Play(IFilter* const filter = nullptr);

private:
	void _Update(cv::Mat& frame);
};