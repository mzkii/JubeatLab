#include "scene_select_music.h"
#include "system_tools.h"
#include <fstream>
#include <iostream>


scene_select_music::scene_select_music()
{
	select.ef_enter.load("rc/ef/ef_enter.mp3");
	load_data.fumen_path[0] = "workspace/fumen/basic//*.txt";
	load_data.fumen_path[1] = "workspace/fumen/advanced//*.txt";
	load_data.fumen_path[2] = "workspace/fumen/extreme//*.txt";
	LoadDivGraph("rc/img/img_buttons.png", 6, 6, 1, 214, 214, select.img_buttons);
	LoadDivGraph("rc/img/img_cd.png", 3, 3, 1, 214, 214, select.img_cd);
	select.jacket_back.load("rc/img/img_jacket_back.png");
	select.ef_level_voice[0].load("rc/ef/ef_bsc.mp3");
	select.ef_level_voice[1].load("rc/ef/ef_adv.mp3");
	select.ef_level_voice[2].load("rc/ef/ef_ext.mp3");
	select.ef_jacket_move.load("rc/ef/ef_jacket_move.mp3");
	select.ef_jacket_select.load("rc/ef/ef_jacket_select.mp3");
	select.select_music.load("rc/ef/select_music.mp3");
	select.sub_font.create_font_handle("meiryo ui", SUB_FONT_SIZE, 6, DX_FONTTYPE_ANTIALIASING_8X8);
	select.main_font.create_font_handle("meiryo ui", MAIN_FONT_SIZE, 10, DX_FONTTYPE_ANTIALIASING_8X8);
	select.artist_font.create_font_handle("meiryo ui", ARTIST_FONT_SIZE, 10, DX_FONTTYPE_ANTIALIASING_8X8);
	loading.loading_pattern = 0;
	select.select_pattern = 0;
	select.now_music_index = 0;

	//メモリー確保に移行
	setState(&scene_select_music::make_memory);
}

scene_select_music::~scene_select_music()
{
	if (fumen_play.fumen != NULL)delete fumen_play.fumen;
}

SCENE_STATE scene_select_music::update(SCREEN_DATA *screen_data)
{
	my_scr = screen_data;
	(this->*func_)();    // メソッドポインタを実行
	return scene_continue;
}

void scene_select_music::make_memory()
{
	//とりあえずあるだけ読み込む
	for (int i = 0; i < 3; i++)
	{
		load_data.hFind = FindFirstFile(load_data.fumen_path[i].c_str(), &load_data.find);
		if (load_data.hFind == INVALID_HANDLE_VALUE)
		{
			continue;
		}
		do {

			string temp = load_data.find.cFileName;

			string name = temp.substr(0, temp.find_last_of("."));

			music_data.emplace_back();

			music_data[music_data.size() - 1].load_music_info(music_data.size() - 1, i, name);

			music_data[music_data.size() - 1].set_title_position(
				select.main_font.get_font_handle(),
				select.sub_font.get_font_handle());


		} while (FindNextFile(load_data.hFind, &load_data.find));

		FindClose(load_data.hFind);
	}

	if ((int)music_data.size() % 3 != 0)
	{
		int max = 3 - (int)music_data.size() % 3;
		for (int i = 0; i < max; i++)
		{
			music_data.emplace_back();
		}
	}

	SetDrawScreen(my_scr->top.img_handle);
	DrawBox(0, 0, my_scr->top.x_size, my_scr->top.y_size, GetColor(0, 0, 0), true);
	SetDrawScreen(my_scr->bottom.img_handle);
	DrawBox(0, 0, my_scr->bottom.x_size, my_scr->bottom.y_size, GetColor(0, 0, 0), true);
	setState(&scene_select_music::loading_rc);
}

void scene_select_music::loading_rc()
{
	SetDrawScreen(my_scr->top.img_handle);
	DrawBox(0, 0, my_scr->top.x_size, my_scr->top.y_size, GetColor(0, 0, 0), true);

	switch (loading.loading_pattern){

	case 0:
		SetUseASyncLoadFlag(true);
		for (int i = 0; i < (int)music_data.size(); i++){
			music_data[i].jacket_load();
		}


		for (int i = 0; i < 12; i++)
		{
			music_data.emplace_back();
		}

		for (int i = 0; i < 12; i++)
		{
			music_data[music_data.size() - 12 + i] = music_data[i];
		}


		SetUseASyncLoadFlag(false);
		loading.load_num = GetASyncLoadNum();
		loading.loading_pattern++;
		break;

	case 1:
		DrawFormatString(
			100,
			100,
			GetColor(255, 255, 255),
			"Now Loading... ( %d %%)",
			(int)((double)(loading.load_num - GetASyncLoadNum()) / loading.load_num * 100));

		DrawLine(
			100,
			200,
			(int)((double)(loading.load_num - GetASyncLoadNum()) / loading.load_num * 980),
			200,
			GetColor(255, 255, 255), 2);

		if ((int)((double)(loading.load_num - GetASyncLoadNum()) / loading.load_num * 100) >= 100){
			loading.loading_pattern++;
		}
		break;

	case 2:
		setState(&scene_select_music::select_music);
		break;
	}


	SetDrawScreen(my_scr->bottom.img_handle);
	DrawBox(0, 0, my_scr->bottom.x_size, my_scr->bottom.y_size, GetColor(0, 0, 0), true);
}

void scene_select_music::init()
{
	fumen_play.fumen = NULL;
	fumen_play.pattern = 0;
	fade.sin_set(60, 0);
	select.jacket_position = 0;
	select.now_music_index = 0;
	select.jacket_change.sin_set(15, 100 - 30);
	select.draw_buttons[0].set(4, KEY_INPUT_Z, select.img_buttons[0], 0.90);
	select.draw_buttons[0].set_blend_img(select.img_buttons[2]);
	select.draw_buttons[1].set(4, KEY_INPUT_X, select.img_buttons[1], 0.90);
	select.draw_buttons[1].set_blend_img(select.img_buttons[3]);
	select.draw_buttons[2].set(4, KEY_INPUT_C, select.img_buttons[4], 0.90);
	select.draw_buttons[3].set(4, KEY_INPUT_V, select.img_buttons[5], 0.90);
	select.jacket_move_pattern = 0;
	select.now_dif_index = 2;
	select.cnt_cd_rota = 0;
	select.select_pattern = 0;
	select.select_flag = true;

	SetUseASyncLoadFlag(TRUE);
	SetCreateSoundDataType(DX_SOUNDDATATYPE_FILE);
	music_data[select.now_music_index].sound_load();
	SetCreateSoundDataType(DX_SOUNDDATATYPE_MEMNOPRESS);
	SetUseASyncLoadFlag(FALSE);

	select.play_pattern = 0;
	select.title_position = 0;
	select.title_width = GetDrawFormatStringWidthToHandle(
		select.main_font.get_font_handle(),
		music_data[select.now_music_index].get_title().c_str());

	select.select_music.play();
	select.animation_pattern = 0;

}

void scene_select_music::select_music()
{


	switch (select.select_pattern){

	case 0:
		init();
		select.select_pattern++;
		break;

	case 1:
		if (fade.check())
		{
			select.select_pattern++;
		}
		break;

	case 2:
		if (keyBuff[KEY_INPUT_V] == 1)
		{
			fade.sin_set(60, 1);
			select.select_flag = false;
			select.ef_enter.play();
			select.volume.sin_set(60, 255);
			select.select_pattern++;
		}
		break;

	case 3:
		ChangeVolumeSoundMem(
			255 - select.volume.sin_check(),
			music_data[select.now_music_index].get_sound_handle());

		select.volume.type1_sin_update_and_return();

		if (fade.check())
		{
			StopSoundMem(music_data[select.now_music_index].get_sound_handle());
			select.select_pattern++;
		}
		break;

	case 4:
		setState(&scene_select_music::play);
		music_data[select.now_music_index].sound_delete();
		select.select_pattern = 0;
		break;
	}

	//-----------------------------------------------上画面
	SetDrawScreen(my_scr->top.img_handle);

	top_draw();

	fade.draw(my_scr->top.x_size, my_scr->top.y_size);

	//-----------------------------------------------下画面
	SetDrawScreen(my_scr->bottom.img_handle);
	DrawBox(0, 0, my_scr->bottom.x_size, my_scr->bottom.y_size, GetColor(255, 200, 0), true);

	select.draw_buttons[0].draw(13);
	select.draw_buttons[1].draw(14);
	select.draw_buttons[2].draw(15);
	select.draw_buttons[3].draw(16);
	jacket_draw();
	jacket_move();

	if (select.select_flag)
	{
		check_jaclet_select();
	}

	select_animation();

	fade.draw(my_scr->bottom.x_size, my_scr->bottom.y_size);
}

void scene_select_music::top_draw()
{
	int level_color[3] = { GetColor(0, 255, 0), GetColor(255, 128, 0), GetColor(255, 0, 0) };

	DrawLine(0, my_scr->top.y_size / 3, my_scr->top.x_size, my_scr->top.y_size / 3, GetColor(0, 0, 0), 2);
	DrawBox(0, 540, my_scr->top.x_size, 540 + 80, GetColor(0, 0, 0), true);
	DrawLine(0, 540, my_scr->top.x_size, 540, level_color[select.now_dif_index], 5);
	DrawLine(0, 540 + 80, my_scr->top.x_size, 540 + 80, level_color[select.now_dif_index], 5);

	DrawFormatStringToHandle(
		50,
		540 + 27,
		level_color[select.now_dif_index],
		select.artist_font.get_font_handle(),
		"START");

	int width = GetDrawFormatStringWidthToHandle(
		select.artist_font.get_font_handle(),
		"END");

	DrawFormatStringToHandle(
		GRAPH_SIZE_Y - 50 - width,
		540 + 27,
		level_color[select.now_dif_index],
		select.artist_font.get_font_handle(),
		"END");

	SetDrawBlendMode(
		DX_BLENDMODE_ALPHA,
		(int)(255 * select.jacket_change.sin_check() / select.jacket_change.get_domain()));

	DrawExtendGraph(
		50,
		100,
		50 + 400,
		100 + 400,
		music_data[select.now_music_index].get_jacket_handle(),
		false);

	SetDrawBlendMode(
		DX_BLENDMODE_NOBLEND,
		255);

	SetDrawArea(
		550,
		my_scr->top.y_size / 3 - 52,
		my_scr->top.x_size,
		my_scr->top.y_size / 3 - 52 + 50);

	DrawStringToHandle(
		550 - select.title_position,
		my_scr->top.y_size / 3 - 52 + 50 -
		(int)(50 * select.jacket_change.sin_check() / select.jacket_change.get_domain()),
		music_data[select.now_music_index].get_title().c_str(),
		GetColor(0, 0, 0),
		select.main_font.get_font_handle());

	SetDrawArea(
		550,
		my_scr->top.y_size / 3 + 2,
		my_scr->top.x_size,
		my_scr->top.y_size / 3 + 2 + ARTIST_FONT_SIZE + 5);

	DrawFormatStringToHandle(
		550,
		my_scr->top.y_size / 3 + 2 - ARTIST_FONT_SIZE +
		(int)(ARTIST_FONT_SIZE * select.jacket_change.sin_check() / select.jacket_change.get_domain()),
		GetColor(0, 0, 0),
		select.artist_font.get_font_handle(),
		"%s", music_data[select.now_music_index].get_artist().c_str());

	SetDrawAreaFull();
}

void scene_select_music::matching()
{

}

void scene_select_music::sort(SORTTYPE param)
{
	for (int i = 0; i < (int)music_data.size(); i++){
		switch (param){

		case SORTTYPE_ALL:
			//すべて表示させる
			music_data[i].set_show_flag(true);
			break;

		}
	}
}

void scene_select_music::play()
{
	SetDrawScreen(my_scr->bottom.img_handle);
	DrawBox(0, 0, my_scr->bottom.x_size, my_scr->bottom.y_size, GetColor(50, 50, 50), true);

	switch (fumen_play.pattern)
	{

	case 0:
		fade.sin_set(60, 0);
		debug_time = GetNowCount();
		fumen_play.fumen = new Cfumen_analyzer;

		if (fumen_play.fumen->fumen_analyze(&music_data[select.now_music_index]) == true)
		{
			dmsg("load time %d", GetNowCount() - debug_time);
			fumen_play.pattern = 1;

		}
		else
		{
			fumen_play.pattern = 100;
		}
		break;

	case 1:
		if (fade.check())
		{
			fumen_play.pattern = 2;
		}
		break;

	case 2:
		fumen_play.fumen->draw_fumen();
		if (keyBuff[KEY_INPUT_D] == 1)
		{
			fade.sin_set(60, 1);
			fumen_play.pattern = 3;
		}
		break;

	case 3:
		fumen_play.fumen->draw_fumen();
		if (fade.check())
		{
			fumen_play.pattern = 100;
		}
		break;

	case 100:
		delete fumen_play.fumen;
		fumen_play.fumen = NULL;
		setState(&scene_select_music::select_music);
		break;
	}

	fade.draw(my_scr->bottom.x_size, my_scr->bottom.y_size);

	SetDrawScreen(my_scr->top.img_handle);
	fade.draw(my_scr->bottom.x_size, my_scr->bottom.y_size);
}

void scene_select_music::jacket_draw()
{

	SetDrawMode(DX_DRAWMODE_BILINEAR);
	int loop_x = select.jacket_position + LINE_WIDTH;
	int loop_y = LINE_WIDTH;
	int graph_x;
	int graph_y;


	for (int index = 0; index < (int)music_data.size(); index++)
	{
		music_data[index].sub_title_position_update();

		if (loop_x > -_BUTTON_SIZE && loop_x < 1080)
		{
			if (music_data[index].get_load_flag() == true && music_data[index].get_show_flag() == true)
			{
				SetDrawArea(loop_x + 25, loop_y + 10, loop_x + BUTTON_SIZE, loop_y + SUB_FONT_SIZE + 20);

				DrawStringToHandle(
					loop_x + 25 + music_data[index].get_sub_title_position(),
					loop_y + 10,
					music_data[index].get_title().c_str(),
					GetColor(0, 0, 0),
					select.sub_font.get_font_handle()
					);

				SetDrawAreaFull();

				if (music_data[index].get_pin() == music_data[select.now_music_index].get_pin())
				{
					GetGraphSize(
						select.img_cd[select.now_dif_index],
						&graph_x,
						&graph_y);

					SetDrawBlendMode(DX_BLENDMODE_ALPHA, select.cd_alpha);

					DrawRotaGraph3(
						loop_x + BUTTON_SIZE / 2,
						loop_y + BUTTON_SIZE / 2 + 10,
						graph_x / 2,
						graph_y / 2,
						((double)(BUTTON_SIZE - 50) / graph_x)*select.cd_rate,
						((double)(BUTTON_SIZE - 50) / graph_y)*select.cd_rate,
						(double)PI / 180 * select.cnt_cd_rota * 5,
						select.img_cd[select.now_dif_index],
						true);

					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);


					SetDrawBlendMode(DX_BLENDMODE_ALPHA, select.jacket_alpha);


					GetGraphSize(
						music_data[index].get_jacket_handle(),
						&graph_x,
						&graph_y);

					DrawRotaGraph3(
						loop_x + BUTTON_SIZE / 2 + 5,
						loop_y + BUTTON_SIZE / 2 + 10,
						graph_x / 2,
						graph_y / 2,
						((double)(BUTTON_SIZE - 70) / graph_x)*select.jacket_rate,
						((double)(BUTTON_SIZE - 70) / graph_y)*select.jacket_rate,
						0.0,
						music_data[index].get_jacket_handle(),
						false);


					GetGraphSize(
						select.jacket_back.get_gr_handle(),
						&graph_x,
						&graph_y);

					DrawRotaGraph3(
						loop_x + BUTTON_SIZE / 2,
						loop_y + BUTTON_SIZE / 2 + 10,
						graph_x / 2,
						graph_y / 2,
						((double)(BUTTON_SIZE - 40) / graph_x)*select.jacket_rate,
						((double)(BUTTON_SIZE - 40) / graph_y)*select.jacket_rate,
						0.0,
						select.jacket_back.get_gr_handle(),
						true);

					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

				}

				else
				{
					GetGraphSize(
						music_data[index].get_jacket_handle(),
						&graph_x,
						&graph_y);

					DrawRotaGraph3(
						loop_x + BUTTON_SIZE / 2 + 5,
						loop_y + BUTTON_SIZE / 2 + 10,
						graph_x / 2,
						graph_y / 2,
						((double)(BUTTON_SIZE - 70) / graph_x)*select.cd_rate,
						((double)(BUTTON_SIZE - 70) / graph_y)*select.cd_rate,
						0.0,
						music_data[index].get_jacket_handle(),
						false);


					GetGraphSize(
						select.jacket_back.get_gr_handle(),
						&graph_x,
						&graph_y);

					DrawRotaGraph3(
						loop_x + BUTTON_SIZE / 2,
						loop_y + BUTTON_SIZE / 2 + 10,
						graph_x / 2,
						graph_y / 2,
						((double)(BUTTON_SIZE - 40) / graph_x)*select.cd_rate,
						((double)(BUTTON_SIZE - 40) / graph_y)*select.cd_rate,
						0.0,
						select.jacket_back.get_gr_handle(),
						true);
				}
			}
		}
		if ((index + 1) % 3 == 0)
		{
			loop_x += _BUTTON_SIZE;
			loop_y = LINE_WIDTH;
		}
		else
		{
			loop_y += _BUTTON_SIZE;
		}
	}

	select.cnt_cd_rota++;
	SetDrawMode(DX_DRAWMODE_NEAREST);
}

void scene_select_music::jacket_select(int new_index, int old_index)
{
	if (music_data[new_index].get_pin() != music_data[old_index].get_pin())
	{
		music_data[select.now_music_index].sound_delete();
		select.now_music_index = new_index;

		SetUseASyncLoadFlag(TRUE);
		SetCreateSoundDataType(DX_SOUNDDATATYPE_FILE);
		music_data[new_index].sound_load();
		SetCreateSoundDataType(DX_SOUNDDATATYPE_MEMNOPRESS);
		SetUseASyncLoadFlag(FALSE);
		select.play_pattern = 0;

		select.title_position = 0;
		select.title_width = GetDrawFormatStringWidthToHandle(
			select.main_font.get_font_handle(),
			music_data[select.now_music_index].get_title().c_str());

		select.animation_pattern = 200;
	}
	else
	{
		select.animation_pattern = 100;
		select.now_dif_index++;
		if (select.now_dif_index >= 3)select.now_dif_index = 0;
		select.ef_level_voice[select.now_dif_index].play();
	}
	PlaySoundMem(select.ef_jacket_select.get_ef_handle(), DX_PLAYTYPE_BACK);
}

void scene_select_music::setState(void (scene_select_music::*func)())
{
	func_ = func;
}

void scene_select_music::jacket_move()
{

#define FAST_MOVE	45
#define LONG_PUSH	150
#define ADD_START	45
#define ADD_LIM		10
#define ADD_DOWN	5

	if (select.jacket_position > 0)
	{
		select.jacket_position = -((int)music_data.size() / 3)*_BUTTON_SIZE + _BUTTON_SIZE * 4;
		select.jaclet_add = ADD_START;
	}

	else if (select.jacket_position <= -((int)music_data.size() / 3)*_BUTTON_SIZE + _BUTTON_SIZE * 4)
	{
		select.jacket_position = 0;
		select.jaclet_add = ADD_START;
	}


	switch (select.jacket_move_pattern){


	case 0:
		if (keyBuff[KEY_INPUT_Z] != 0 && keyBuff[KEY_INPUT_X] != 0)
		{
			break;
		}

		if (keyBuff[KEY_INPUT_Z] != 0)
		{
			select.ef_jacket_move.play();
			select.jacket_move_pattern = 100;
			select.jaclet_add = ADD_START;
			break;
		}

		if (keyBuff[KEY_INPUT_X] != 0)
		{
			select.ef_jacket_move.play();
			select.jacket_move_pattern = 200;
			select.jaclet_add = ADD_START;
			break;
		}
		break;

	case 100:

		if (keyBuff[KEY_INPUT_Z] == 1)
		{
			select.ef_jacket_move.play();
			select.jacket_position += -select.jacket_position % 270;
			select.jaclet_add = ADD_START;
			break;
		}

		select.jacket_position += select.jaclet_add;

		if (select.jaclet_add > ADD_LIM)
		{
			select.jaclet_add -= ADD_DOWN;
		}

		else if (select.jaclet_add > 1)
		{
			select.jaclet_add--;
		}

		if (select.jacket_position%_BUTTON_SIZE == 0)
		{
			select.jaclet_add = 0;
			select.cnt_push = GetNowCount();
			select.jacket_move_pattern = 110;
		}
		break;

	case 110:
		if (keyBuff[KEY_INPUT_Z] == 0)
		{
			select.jacket_move_pattern = 0;
		}
		else if (GetNowCount() - select.cnt_push > LONG_PUSH)
		{
			select.jacket_move_pattern = 120;
		}
		break;

	case 120:
		select.jacket_position += FAST_MOVE;
		if (select.jacket_position%_BUTTON_SIZE == 0)
		{
			if (keyBuff[KEY_INPUT_Z] == 0)
			{
				select.jacket_move_pattern = 100;
				select.jacket_position += -select.jacket_position % 270;
				select.jaclet_add = ADD_START;
			}
		}
		break;

	case 200:
		if (keyBuff[KEY_INPUT_X] == 1)
		{
			select.ef_jacket_move.play();
			select.jacket_position -= 270 + select.jacket_position % 270;
			select.jaclet_add = ADD_START;
			break;
		}

		select.jacket_position -= select.jaclet_add;

		if (select.jaclet_add > ADD_LIM)
		{
			select.jaclet_add -= ADD_DOWN;
		}

		else if (select.jaclet_add > 1)
		{
			select.jaclet_add--;
		}

		if (select.jacket_position%_BUTTON_SIZE == 0)
		{
			select.jaclet_add = 0;
			select.jacket_move_pattern = 210;
			select.cnt_push = GetNowCount();
		}
		break;

	case 210:
		if (keyBuff[KEY_INPUT_X] == 0)
		{
			select.jacket_move_pattern = 0;
		}
		else if (GetNowCount() - select.cnt_push > LONG_PUSH)
		{
			select.jacket_move_pattern = 220;
		}
		break;

	case 220:
		select.jacket_position -= FAST_MOVE;
		if (select.jacket_position%_BUTTON_SIZE == 0)
		{
			if (keyBuff[KEY_INPUT_X] == 0)
			{
				select.jacket_move_pattern = 200;
				select.jacket_position -= select.jacket_position % 270;
				select.jaclet_add = ADD_START;
			}
		}
		break;
	}
}

void scene_select_music::check_jaclet_select()
{
	//読み込みが完了したら選択可能にする
	if (!CheckHandleASyncLoad(music_data[select.now_music_index].get_sound_handle()))
	{
		switch (select.play_pattern)
		{
		case 0:
			ChangeVolumeSoundMem(255, music_data[select.now_music_index].get_sound_handle());

			PlaySoundMem(music_data[select.now_music_index].get_sound_handle(), DX_PLAYTYPE_BACK,TRUE);

			select.play_time = GetNowCount();
			select.play_pattern++;
			break;

		case 1:
			if (GetNowCount() - select.play_time > 10000)
			{
				select.play_pattern++;
				select.volume.sin_set(120, 255);
			}
			break;

		case 2:
			ChangeVolumeSoundMem(
				255 - select.volume.sin_check(),
				music_data[select.now_music_index].get_sound_handle());

			if (select.volume.sin_check() < select.volume.get_domain())
			{
				select.volume.type1_sin_update_and_return();
			}
			else
			{
				StopSoundMem(music_data[select.now_music_index].get_sound_handle());
				select.play_pattern = 0;
			}
			break;
		}

		for (int i = 0; i < 12; i++)
		{
			int key_index = i - (select.jacket_position / _BUTTON_SIZE) * 3;

			if (keyBuff[button[i]] == 1 && music_data[key_index].get_load_flag() == true)
			{
				jacket_select(key_index, select.now_music_index);
			}
		}
	}
}

void scene_select_music::select_animation()
{
	//animation
	switch (select.animation_pattern)
	{

	case 0:
		select.jacket_alpha = 0;
		select.cd_alpha = 255;
		select.jacket_rate = 1.0;
		select.cd_rate = 1.0;
		break;

		//レベルチェンジアニメーション
	case 100:
		select.jacket_alpha = 0;
		select.cd_alpha = 255;
		select.level_change.sin_set(10, 0.05);
		select.jacket_rate = 0.9 + select.level_change.sin_check();
		select.cd_rate = 0.9 + select.level_change.sin_check();
		select.animation_pattern++;
		break;

	case 101:
		select.jacket_rate = 0.95 + select.level_change.sin_check();
		select.cd_rate = 0.95 + select.level_change.sin_check();
		if (select.level_change.sin_check() < select.level_change.get_domain())
		{
			select.level_change.type2_sin_update_and_return();
		}
		else
		{
			select.animation_pattern = 0;
		}
		break;

	case 200:
		select.jacket_alpha = 255;
		select.cd_alpha = 0;
		select.jacket_rate = 1.0;
		select.cd_rate = 1.0;
		select.jacket_change.sin_set(15, 100 - 30);
		select.animation_pattern++;
		break;

	case 201:
		select.jacket_alpha = 255 - 255 * select.jacket_change.sin_check() / select.jacket_change.get_domain();
		select.cd_alpha = 255 * select.jacket_change.sin_check() / select.jacket_change.get_domain();
		select.jacket_rate = 0.7 + 0.4 * select.jacket_change.sin_check() / select.jacket_change.get_domain();

		if (select.jacket_change.sin_check() >= select.jacket_change.get_domain())
		{
			select.animation_pattern = 0;
		}
		break;
	}


	if (select.jacket_change.sin_check() < select.jacket_change.get_domain())
	{
		select.jacket_change.type2_sin_update_and_return();
	}
	else
	{
		if (select.title_width > my_scr->top.x_size - 550)
		{
			if (select.title_position < select.title_width)select.title_position++;
			else select.title_position = -(my_scr->top.x_size - 550);
		}
	}
}
