#pragma once
#include <iostream>
#include <string>
#include "DxLib.h"
#include <vector>
#include <fstream>
#include "sin_wave.h"

using namespace std;

const int button[12] = {
	KEY_INPUT_1, KEY_INPUT_Q, KEY_INPUT_A,
	KEY_INPUT_2, KEY_INPUT_W, KEY_INPUT_S,
	KEY_INPUT_3, KEY_INPUT_E, KEY_INPUT_D,
	KEY_INPUT_4, KEY_INPUT_R, KEY_INPUT_F };



extern char keyBuff[256];
void getKey(void);


void MyOutputDebugString(LPCSTR pszFormat, ...);
void dmsg(LPCSTR pszFormat, ...);

// 1 <= pos <= 16
void GetButtonPos(int pos, int *x, int *y);



class img_load{

private:
	int gr_handle;
	int width;
	int height;
	string get_path;

public:
	void set_gr_handle(int handle);
	int load(string param_get_path);
	~img_load();
	int get_gr_handle(bool msg = true);
	int center_width(int window_width);
	int center_height(int window_height);
};








class ef_load{

private:
	int ef_handle;
	string get_path;
	int play_num;

public:
	void ef_load::load(string param_get_path);
	~ef_load();
	int play();
	int get_ef_handle();
	void set_ef_handle(int handle);
};




class Cfont_handle{
private:
	int font_handle;


public:
	Cfont_handle();
	void set_width(string str);
	void create_font_handle(string font_name, int size, int thick, int font_type);
	~Cfont_handle();
	int get_font_handle();
	int width;
};



class change_animation{

private:
	sin_wave<int> bright_value;
	int mode;

public:
	// 0...フェードイン 1...フェードアウト
	void sin_set(int frame,int mode_);
	void draw(int x_size, int y_size);
	void update();
	bool check();
	int get_value();

};


class Cbutton{

private:

	struct{
		int graph_handle;
		int x_size, y_size;
	}normal,blend;

	int ef_push;
	int blend_flag;
	int key_input;
	int frame;
	int extend_pattern;
	double rate;
	sin_wave<double> extend;
public:
	Cbutton();
	void set_ef_push(int _ef_handle);
	void set_blend_img(int _graph_handle);
	void set(int _frame,int _key_input,int _graph_handle,double _rate);
	void draw(int button_num);
};


typedef struct{
	int main_title_position;
	int main_title_width;
	int sub_title_position;
	int sub_title_width;

	bool show_flag;			//ソートで使用
	bool load_flag;			//ロードが完了しているか
	int pin;				//楽曲固有の識別番号
	int dif;				//現在の難易度
	int level;				//レベル
	int prevpos;			//プレビュー再生開始時間
	int jacket_handle;		//ジャケット画像ハンドル
	int sound_handle;		//サウンドハンドル
	streamoff txt_size;		//ファイルサイズ
	string title;			//タイトル（デフォルトはファイル名）
	string artist;			//（デフォルトはundefined.）
	string jacket_path;
	string sound_path;
	string txt_path;
}MUSIC_DATA;



class music_info{

private:
	MUSIC_DATA music_data_;

public:
	~music_info();
	void set_title_position(int main_font_handle,int sub_font_handle);
	void sub_title_position_update();
	int get_sub_title_position();



	int get_prevpos();
	bool get_show_flag();
	bool get_load_flag();
	int get_pin();
	string get_artist();
	string get_title();
	string get_fumen_path();

	void jacket_load();
	int get_jacket_handle();
	void jacket_delete();

	void sound_load();
	int get_sound_handle();
	void sound_delete();

	void set_show_flag(bool flag);
	int load_music_info(int get_pin, int get_dif, string txt_name);
};
