#include "video_player.h"

VideoPlayer::VideoPlayer(const char* filename) : _capture(filename)
{
	_init = _capture.isOpened() && _capture.get(cv::CAP_PROP_FRAME_COUNT) > 2;
}

bool VideoPlayer::ok() const
{
	return _init;
}

void VideoPlayer::Init(bool debug, const char* filename)
{
	if (filename)
		_capture.open(filename);
	_init = _capture.isOpened() || _capture.get(cv::CAP_PROP_FRAME_COUNT) > 2;
	_debugMode = debug;
}

void VideoPlayer::SetDebugMode(bool value)
{
	_debugMode = value;
}

void VideoPlayer::Play(IFilter* const filter)
{
	if (_init)
	{
		int keyCode = 0;
		bool playbackPaused = false;
		bool playbackFinished = false;

		cv::Mat currentFrame;
		_capture.read(currentFrame);

		while (!playbackFinished)
		{
			if (!playbackPaused)
			{
				if (filter)
					filter->FrameProcessing(currentFrame, _debugMode);
				_Update(currentFrame);
			}

			keyCode = cv::waitKey(FRAME_RATE);
			playbackPaused = (keyCode == 32) ? !playbackPaused : playbackPaused;
			playbackFinished = (keyCode == 27) || (_capture.get(cv::CAP_PROP_POS_FRAMES) == _capture.get(cv::CAP_PROP_FRAME_COUNT));
		}
	}
}

void VideoPlayer::_Update(cv::Mat& frame)
{
	cv::imshow(MAIN_WINDOW_NAME, frame);
	_capture.read(frame);
}
