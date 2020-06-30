#include "video_player.h"
#include "detection_filter.h"

const char* file_name = "res/test.mp4";

int main()
{
	VideoPlayer player(file_name);
	if (player.ok())
	{
		player.SetDebugMode(true);
		player.Play(&DetectionFilter());
	}
}