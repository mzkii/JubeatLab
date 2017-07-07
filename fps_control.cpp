#pragma once
#include "main_loop.h"
#include "fps_control.h"
#include <DxLib.h>



fps_control::fps_control(){
	mStartTime = 0;
	mCount = 0;
	mFps = 0;
}

bool fps_control::Update(){
	if (mCount == 0){ //1フレーム目なら時刻を記憶
		mStartTime = GetNowCount();
	}
	if (mCount == FPS){ //60フレーム目なら平均を計算する
		int t = GetNowCount();
		mFps = 1000.f / ((t - mStartTime) / (float)FPS_AVE);
		mCount = 0;
		mStartTime = t;
	}
	mCount++;
	return true;
}

void fps_control::Wait(){
	int tookTime = GetNowCount() - mStartTime;	//かかった時間
	int waitTime = mCount * 1000 / FPS - tookTime;	//待つべき時間
	if (waitTime > 0){
		Sleep(waitTime);	//待機
	}
}

void fps_control::Draw(){
	DrawFormatString(10, 10, GetColor(255, 255, 255), "%04.1f", mFps);
}


float fps_control::get_fps(){
	return mFps;
}
