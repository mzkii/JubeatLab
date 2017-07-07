#include "scene_demo.h"

scene_demo::scene_demo(){
	OutputDebugString("create scene_demo.\n");
	state = 0;
	logo.load("rc/img/img_jubeat.png");
	touch_icon.load("rc/img/touch_knt.png");
	welcome.load("rc/ef/ef_welcome.mp3");
	background.load("rc/ef/ef_title.mp3");
	enter.load("rc/ef/ef_enter.mp3");
	fade.sin_set(120, 0);
	touch_fade.sin_set(50, 200);
}

scene_demo::~scene_demo(){
	OutputDebugString("delete scene_demo.\n");
}


SCENE_STATE scene_demo::update(SCREEN_DATA *screen_data){

	scr_data = screen_data;

	SetDrawScreen(screen_data->top.img_handle);
	top_draw(scr_data->top.x_size, scr_data->top.y_size);

	SetDrawScreen(screen_data->bottom.img_handle);
	bottom_draw(scr_data->bottom.x_size, scr_data->bottom.y_size);

	switch (state){

	case 0:
		PlaySoundMem(welcome.get_ef_handle(), DX_PLAYTYPE_BACK);
		PlaySoundMem(background.get_ef_handle(), DX_PLAYTYPE_LOOP);
		state++;
		break;

	case 1:
		if (fade.check())state++;
		break;

	case 2:
		if (CheckHitKeyAll()){
			PlaySoundMem(enter.get_ef_handle(), DX_PLAYTYPE_BACK);
			fade.sin_set(120, 1);
			touch_fade.sin_set(5, 200);
			state++;
		}
		break;

	case 3:
		ChangeVolumeSoundMem(fade.get_value(), background.get_ef_handle());
		if (fade.check())return scene_finish;
		break;

	}


	return scene_continue;
}


void scene_demo::bottom_draw(int x_size, int y_size){
	DrawBox(0, 0, x_size, y_size, GetColor(250, 200, 0), true);
	fade.draw(x_size, y_size);
}


void scene_demo::top_draw(int x_size, int y_size){


	switch (state){

	case 0:
	case 1:
		DrawRotaGraph(x_size / 2, y_size / 2 - 100, 1.0, 0.0, logo.get_gr_handle(), true);
		DrawRotaGraph(x_size / 2, y_size / 2 + 200, 1.0, 0.0, touch_icon.get_gr_handle(), true);
		fade.draw(x_size, y_size);
		break;

	case 2:
		touch_fade.type2_sin_update_and_return();
		DrawRotaGraph(x_size / 2, y_size / 2 - 100, 1.0, 0.0, logo.get_gr_handle(), true);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - touch_fade.sin_check());
		DrawRotaGraph(x_size / 2, y_size / 2 + 200, 1.0, 0.0, touch_icon.get_gr_handle(), true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		break;

	case 3:
		touch_fade.type2_sin_update_and_return();
		DrawRotaGraph(x_size / 2, y_size / 2 - 100, 1.0, 0.0, logo.get_gr_handle(), true);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - touch_fade.sin_check());
		DrawRotaGraph(x_size / 2, y_size / 2 + 200, 1.0, 0.0, touch_icon.get_gr_handle(), true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		fade.draw(x_size, y_size);
		break;

	}
}