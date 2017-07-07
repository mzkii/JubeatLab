#pragma once
#include <DxLib.h>

class fps_control{
	int font_handle;
	int mStartTime;
	int mCount;
	float mFps;

public:
	fps_control();
	bool Update();
	void Draw();
	float get_fps();
	void Wait();
};
