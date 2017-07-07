#include "scene_control.h"
#include "DxLib.h"

scene_control::scene_control()
{
	credit.create_font_handle("meiryo ui", 25, 6, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);

	pattern = SELECT_MUSIC;
}

void scene_control::scene_allocation(SCREEN_DATA *screen_data)
{
	SetDrawScreen(screen_data->top.img_handle);
	wave_animation.draw(
		screen_data->wave_y,
		screen_data->top.x_size,
		screen_data->top.y_size
		);

	switch (pattern){

	case DEMO:
		if (scene_demo_.update(screen_data) == scene_finish){
			pattern = SELECT_MUSIC;
		}
		break;

	case SELECT_MUSIC:
	{
		DrawFormatStringToHandle(
			80,
			660,
			GetColor(255, 255, 255),
			credit.get_font_handle(),
			"PASELI  NO ACCOUNT");


		int width = GetDrawFormatStringWidthToHandle(
			credit.get_font_handle(),
			"CREDIT  %d", 2);


		DrawFormatStringToHandle(
			GRAPH_SIZE_Y / 2 - width / 2,
			660,
			GetColor(255, 255, 255),
			credit.get_font_handle(),
			"CREDIT  %d", 2);
		if (scene_select_music_.update(screen_data) == scene_finish){
			pattern = DEMO;
		}
	}
		break;

	default:
		OutputDebugString("[scene_control] default pattern.");
		break;

	}
}
