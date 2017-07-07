#pragma once
#include "DxLib.h"
#include "system_tools.h"
#include "main_loop.h"

char keyBuff[256];

void getKey(void)
{
	char tempKey[256];
	GetHitKeyStateAll(tempKey);
	for (int i = 0; i < 256; i++){
		if (tempKey[i] != 0){
			keyBuff[i]++;
			if (keyBuff[i] <= 0)keyBuff[i] = 2;
		}
		else{
			keyBuff[i] = 0;
		}
	}
}

// 1 <= pos <= 16
void GetButtonPos(int pos, int *x, int *y)
{
	*x = (int)(LINE_WIDTH + _BUTTON_SIZE * ((pos - 1) % 4));
	*y = (int)(LINE_WIDTH + _BUTTON_SIZE * ((pos - 1) / 4));
}

int img_load::load(string param_get_path)
{
	get_path = param_get_path;
	gr_handle = LoadGraph(get_path.c_str());
	GetGraphSize(gr_handle, &width, &height);
	return gr_handle;
}

void img_load::set_gr_handle(int handle)
{
	gr_handle = handle;
}

img_load::~img_load()
{
	DeleteGraph(gr_handle);
}

int img_load::center_width(int window_width)
{
	return window_width / 2 - width / 2;
}

int img_load::center_height(int window_height)
{
	return window_height / 2 - height / 2;

}

int img_load::get_gr_handle(bool msg)
{
	if (gr_handle == -1 && msg)dmsg("グラフィックファイルのロードに失敗しています。\n指定されたパス:%s", get_path.c_str());
	return gr_handle;
}

int ef_load::play()
{
	PlaySoundMem(ef_handle, DX_PLAYTYPE_BACK);
	return 0;
}

void ef_load::load(string param_get_path)
{
	//dmsg("constructor:::::  load   path : %s    num : %d", param_get_path.c_str(), get_load_num);
	play_num = 0;
	get_path = param_get_path;
	ef_handle = LoadSoundMem(param_get_path.c_str());
	if (ef_handle == -1)dmsg("サウンドファイルのロードに失敗しています。\n指定されたパス:%s", get_path.c_str());
}

ef_load::~ef_load()
{
	DeleteSoundMem(ef_handle);
}


int ef_load::get_ef_handle()
{
	return ef_handle;
}


void ef_load::set_ef_handle(int handle)
{
	ef_handle = handle;
}


void Cfont_handle::create_font_handle(string font_name, int size, int thick, int font_type)
{
	font_handle = CreateFontToHandle(font_name.c_str(), size, thick, font_type);
}

Cfont_handle::~Cfont_handle()
{
	DeleteFontToHandle(font_handle);
}

int Cfont_handle::get_font_handle()
{
	return font_handle;
}

Cfont_handle::Cfont_handle()
{
	width = 0;
}

void Cfont_handle::set_width(string str)
{
	width = GetDrawStringWidthToHandle(str.c_str(), strlen(str.c_str()),
		font_handle);
}



void change_animation::sin_set(int frame, int mode_)
{
	bright_value.sin_set(frame, 255);
	mode = mode_;
}



bool change_animation::check()
{
	if (bright_value.sin_check() >= bright_value.get_domain()){
		return true;
	}
	update();
	return false;
}

void change_animation::update()
{
	bright_value.type1_sin_update_and_return();
}

void change_animation::draw(int x_size, int y_size)
{
	if (mode == 0){
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - bright_value.sin_check());
	}
	if (mode == 1){
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, bright_value.sin_check());
	}
	DrawBox(0, 0, x_size, y_size, GetColor(0, 0, 0), true);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

}

int change_animation::get_value()
{
	if (mode == 1){
		return 255 - bright_value.sin_check();
	}

	return bright_value.sin_check();
}

void Cbutton::set(int _frame, int _key_input, int _graph_handle, double _rate)
{
	normal.graph_handle = _graph_handle;
	GetGraphSize(normal.graph_handle, &normal.x_size, &normal.y_size);


	frame = _frame;
	key_input = _key_input;
	rate = _rate;
	extend_pattern = 0;

	if (rate > 0 && rate < 1.0){
		extend.sin_set(frame, 1.0 - _rate);
	}
	else if (rate > 1.0){
		extend.sin_set(frame, _rate - 1.0);
	}

}

void Cbutton::set_blend_img(int _graph_handle)
{
	blend_flag = true;
	blend.graph_handle = _graph_handle;
	GetGraphSize(blend.graph_handle, &blend.x_size, &blend.y_size);
}

void Cbutton::draw(int button_num)
{
	int x_pos, y_pos;
	double exrate = 1.0;
	double alpha = 0.0;


	GetButtonPos(button_num, &x_pos, &y_pos);


	switch (extend_pattern){


	case 0:
		if (CheckHitKey(key_input) == 1){
			if (ef_push != -1)PlaySoundMem(ef_push, DX_PLAYTYPE_BACK);
			extend_pattern = 1;
		}
		break;


	case 1:
		if (rate > 0 && rate < 1.0){
			exrate = 1.0 - extend.sin_check();
		}

		else if (rate > 1.0){
			exrate = 1.0 + extend.sin_check();
		}

		alpha = extend.sin_check();

		if (extend.sin_check() < extend.get_domain()){
			extend.type1_sin_update_and_return();
		}

		else if (CheckHitKey(key_input) == 0)
		{
			extend.sin_reset();
			extend_pattern = 2;
		}
		break;


	case 2:
		if (rate > 0 && rate < 1.0){
			exrate = 1.0 - extend.get_domain() + extend.sin_check();
		}

		else if (rate > 1.0){
			exrate = 1.0 + extend.get_domain() - extend.sin_check();
		}

		alpha = extend.get_domain() - extend.sin_check();

		if (extend.sin_check() < extend.get_domain()){
			extend.type1_sin_update_and_return();
		}

		else{
			extend.sin_reset();
			extend_pattern = 0;
		}

		break;

	}


	if (blend_flag){


		SetDrawBlendMode(
			DX_BLENDMODE_ALPHA,
			(int)(alpha * 255 / extend.get_domain()));

		DrawRotaGraph3(
			x_pos + BUTTON_SIZE / 2,
			y_pos + BUTTON_SIZE / 2,
			blend.x_size / 2,
			blend.y_size / 2,
			((double)BUTTON_SIZE / blend.x_size)*exrate,
			((double)BUTTON_SIZE / blend.y_size)*exrate,
			0.0,
			blend.graph_handle,
			true);

		SetDrawBlendMode(
			DX_BLENDMODE_ALPHA,
			255 - (int)(alpha * 255 / extend.get_domain()));

		DrawRotaGraph3(
			x_pos + BUTTON_SIZE / 2,
			y_pos + BUTTON_SIZE / 2,
			normal.x_size / 2,
			normal.y_size / 2,
			((double)BUTTON_SIZE / normal.x_size)*exrate,
			((double)BUTTON_SIZE / normal.y_size)*exrate,
			0.0,
			normal.graph_handle,
			true);

		SetDrawBlendMode(
			DX_BLENDMODE_NOBLEND,
			255);
	}
	else{

		DrawRotaGraph3(
			x_pos + BUTTON_SIZE / 2,
			y_pos + BUTTON_SIZE / 2,
			normal.x_size / 2,
			normal.y_size / 2,
			((double)BUTTON_SIZE / normal.x_size)*exrate,
			((double)BUTTON_SIZE / normal.y_size)*exrate,
			0.0,
			normal.graph_handle,
			true);
	}/*

	DrawFormatString(
	x_pos,
	y_pos,
	GetColor(255, 255, 255),
	"[ %d ]",
	(int)(alpha * 255 / extend.get_domain()));*/
}

Cbutton::Cbutton(){
	blend_flag = false;
	ef_push = -1;
}

void Cbutton::set_ef_push(int _ef_handle){
	ef_push = _ef_handle;
}

int music_info::load_music_info(int get_pin, int get_dif, string txt_name){

	string make_path;


	switch (get_dif){
	case 0:
		make_path = "workspace/fumen/basic/" + txt_name + ".txt";
		break;
	case 1:
		make_path = "workspace/fumen/advanced/" + txt_name + ".txt";
		break;
	case 2:
		make_path = "workspace/fumen/extreme/" + txt_name + ".txt";
		break;
	}


	ifstream ifs(make_path.c_str());
	std::string load_line_str;
	if (ifs.fail())
	{
		MyOutputDebugString("[load_fumen]  %s の読み込みに失敗しました。", make_path.c_str());
		music_data_.txt_path = "";
		return -1;
	}

	music_data_.txt_path = make_path;

	music_data_.load_flag = true;
	music_data_.pin = get_pin;
	music_data_.dif = get_dif;
	music_data_.artist = "undefined.";
	music_data_.jacket_path = "workspace/jacket/" + txt_name + ".png";
	music_data_.sound_path = "workspace/mp3/" + txt_name + ".mp3";
	music_data_.level = 1;
	music_data_.prevpos = 0;
	music_data_.show_flag = true;

	ifs.seekg(0, fstream::end);
	streamoff eofPos = ifs.tellg();
	ifs.clear();
	ifs.seekg(0, fstream::beg);
	streamoff begPos = ifs.tellg();
	music_data_.txt_size = eofPos - begPos;


	music_data_.title = txt_name;


	while (getline(ifs, load_line_str))
	{
		
		int get_option_num;
		if (strstr(load_line_str.c_str(), "#title=") != NULL){
			load_line_str.erase(0, 7);
			music_data_.title = load_line_str;
			//dmsg("artist %s", artist.c_str());
			continue;
		}
		if (strstr(load_line_str.c_str(), "#artist=") != NULL){
			load_line_str.erase(0, 8);
			music_data_.artist = load_line_str;
			//dmsg("artist %s", artist.c_str());
			continue;
		}
		if (strstr(load_line_str.c_str(), "#jacket=") != NULL){
			load_line_str.erase(0, 8);
			music_data_.jacket_path = load_line_str;
			//dmsg("artist %s", jacket_path.c_str());
		}
		if (strstr(load_line_str.c_str(), "#music=") != NULL){
			load_line_str.erase(0, 7);
			music_data_.sound_path = load_line_str;
			//dmsg("artist %s", sound_path.c_str());
			continue;
		}
		if (strstr(load_line_str.c_str(), "#level=") != NULL){
			sscanf_s(load_line_str.c_str(), "#level=%d", &get_option_num, (unsigned)sizeof(get_option_num));
			music_data_.level = get_option_num;
			//dmsg("level %d", get_option_num);
			continue;
		}
		if (strstr(load_line_str.c_str(), "#prevpos=") != NULL){
			sscanf_s(load_line_str.c_str(), "#prevpos=%d", &get_option_num, (unsigned)sizeof(get_option_num));
			music_data_.prevpos = get_option_num;
			//dmsg("prevpos %d", get_option_num);
			continue;
		}
	}
	return 0;
}

int music_info::get_prevpos()
{
	return music_data_.prevpos;
}

music_info::~music_info()
{

}
string music_info::get_fumen_path()
{
	return music_data_.txt_path;
}

bool music_info::get_show_flag()
{
	return music_data_.show_flag;
}

string music_info::get_artist()
{
	return music_data_.artist;
}

string music_info::get_title()
{
	return music_data_.title;
}

bool music_info::get_load_flag()
{
	return music_data_.load_flag;
}

void music_info::jacket_load()
{
	music_data_.jacket_handle = LoadGraph(music_data_.jacket_path.c_str());
}

int music_info::get_jacket_handle()
{
	return music_data_.jacket_handle;
}

void music_info::jacket_delete()
{
	DeleteGraph(music_data_.jacket_handle);
}

void music_info::sound_load()
{
	music_data_.sound_handle = LoadSoundMem(music_data_.sound_path.c_str());
}

int music_info::get_sound_handle()
{
	return music_data_.sound_handle;
}

void music_info::sound_delete()
{
	DeleteSoundMem(music_data_.sound_handle);
}

int music_info::get_pin()
{
	return music_data_.pin;
}

void music_info::set_show_flag(bool flag)
{
	music_data_.show_flag = flag;
}

int music_info::get_sub_title_position()
{
	return  music_data_.sub_title_position;
}

void music_info::set_title_position(int main_font_handle, int sub_font_handle)
{
	music_data_.main_title_position = 0;
	music_data_.sub_title_position = 0;

	music_data_.main_title_width = GetDrawFormatStringWidthToHandle(
		main_font_handle,
		"%s",
		music_data_.title.c_str());

	music_data_.sub_title_width = GetDrawFormatStringWidthToHandle(
		sub_font_handle,
		"%s",
		music_data_.title.c_str());
}

void music_info::sub_title_position_update()
{
	if (music_data_.sub_title_width > BUTTON_SIZE - 25){

		if (music_data_.sub_title_position <= -music_data_.sub_title_width){

			music_data_.sub_title_position = BUTTON_SIZE;
		}
		else{

			music_data_.sub_title_position--;
		}
	}
}
