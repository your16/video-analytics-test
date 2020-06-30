#include "video_player.h"
#include "detection_filter.h"

const char* FILE_NAME = "res/test.mp4";

int main()
{
	VideoPlayer player(FILE_NAME);
	if (player.ok())
	{
		player.SetDebugMode(true);
		player.Play(&DetectionFilter());
	}
}