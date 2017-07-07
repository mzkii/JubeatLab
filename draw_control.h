#include "DxLib.h"
#include "main_loop.h"
#include "system_tools.h"
#include "scene_control.h"
#include "sin_wave.h"

const int buttons[16] = {
	KEY_INPUT_1, KEY_INPUT_Q, KEY_INPUT_A, KEY_INPUT_Z,
	KEY_INPUT_2, KEY_INPUT_W, KEY_INPUT_S, KEY_INPUT_X,
	KEY_INPUT_3, KEY_INPUT_E, KEY_INPUT_D, KEY_INPUT_C,
	KEY_INPUT_4, KEY_INPUT_R, KEY_INPUT_F, KEY_INPUT_V };



class draw_control
{
private:
	img_load press[2];
	scene_control scene_ctrl;
	Cfont_handle fps_font;
	sin_wave<double> window_rota;
	SCREEN_DATA screen_data;
	int front_mask_handle;
	int cnt_rota;
	int rota_pattern;

public:
	draw_control();
	~draw_control();
	void processing(float fps);
};
