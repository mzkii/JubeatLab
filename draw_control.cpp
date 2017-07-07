#include "draw_control.h"
#include "DxLib.h"
#include "main_loop.h"


draw_control::draw_control()
{
	screen_data.top.img_handle = MakeScreen(GRAPH_SIZE_Y, GRAPH_SIZE_X - GRAPH_SIZE_Y);
	screen_data.bottom.img_handle = MakeScreen(GRAPH_SIZE_Y, GRAPH_SIZE_Y);
	fps_font.create_font_handle("meiryo ui", 25, 6, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	screen_data.wave_y = 500;

	press[0].load("rc/img/press.png");
	press[1].load("rc/img/pressed.png");

	GetGraphSize(screen_data.top.img_handle, &screen_data.top.x_size, &screen_data.top.y_size);
	GetGraphSize(screen_data.bottom.img_handle, &screen_data.bottom.x_size, &screen_data.bottom.y_size);

	cnt_rota = 1;
	rota_pattern = 0;
	window_rota.sin_set(30, (double)PI / 2);

	//--------------------------------------------<front_mask_handleの作成ここから
	front_mask_handle = MakeScreen(GRAPH_SIZE_X, GRAPH_SIZE_Y, true);
	SetDrawScreen(front_mask_handle);
	DrawBox(0, 0, GRAPH_SIZE_X, GRAPH_SIZE_Y, GetColor(255, 255, 255), true);
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			int x1 = j * _BUTTON_SIZE + LINE_WIDTH,
				y1 = i * _BUTTON_SIZE + LINE_WIDTH,
				x2 = (j + 1) * _BUTTON_SIZE - LINE_WIDTH,
				y2 = (i + 1) * _BUTTON_SIZE - LINE_WIDTH;
			DrawBox(x1, y1, x2, y2, GetColor(0, 0, 0), true);
		}
	}
/*
	DrawBox(GRAPH_SIZE_Y + LINE_TOP_WIDTH,
		LINE_WIDTH,
		GRAPH_SIZE_X - LINE_WIDTH,
		GRAPH_SIZE_Y - LINE_WIDTH,
		GetColor(0, 0, 0),
		true);
*/

	DrawBox(GRAPH_SIZE_Y + LINE_TOP_WIDTH,
		0,
		GRAPH_SIZE_X ,
		GRAPH_SIZE_Y ,
		GetColor(0, 0, 0),
		true);

	//--------------------------------------------<front_mask_handleの作成ここまで

	SetDrawScreen(DX_SCREEN_BACK);
}

draw_control::~draw_control()
{
	DeleteGraph(screen_data.top.img_handle);
	DeleteGraph(screen_data.bottom.img_handle);
	DeleteGraph(front_mask_handle);
}

void draw_control::processing(float fps)
{
	//top、bottomともに白で初期化
	SetDrawScreen(screen_data.top.img_handle);
	DrawBox(0, 0, screen_data.top.x_size, screen_data.top.y_size, GetColor(255, 255, 255), true);
	SetDrawScreen(screen_data.bottom.img_handle);
	DrawBox(0, 0, screen_data.bottom.x_size, screen_data.bottom.y_size, GetColor(0, 0, 0), true);


	//裏画面も白で初期化
	SetDrawScreen(DX_SCREEN_BACK);
	DrawBox(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y, GetColor(0, 0, 0), true);
	

	//実際の処理はシーンコントロールにお任せ
	scene_ctrl.scene_allocation(&screen_data);

	SetDrawScreen(screen_data.top.img_handle);
	DrawFormatStringToHandle(
		1000,
		20,
		GetColor(255, 255, 255),
		fps_font.get_font_handle(),
		"%04.1f", fps);
	SetDrawScreen(DX_SCREEN_BACK);


	//裏画面に設定して、top、bottomをそれぞれ描画
	SetDrawScreen(DX_SCREEN_BACK);

	SetDrawMode(DX_DRAWMODE_BILINEAR);
	//bottom
	switch (rota_pattern){
	case 0:
		DrawRotaGraph(
			WINDOW_SIZE_Y / 2,
			WINDOW_SIZE_Y / 2,
			(double)WINDOW_SIZE_Y/GRAPH_SIZE_Y,
			cnt_rota * PI / 2,
			screen_data.bottom.img_handle,
			false);
		if (keyBuff[KEY_INPUT_F2] == 1)rota_pattern = 1;
		break;

	case 1:
		window_rota.type1_sin_update_and_return();
		DrawRotaGraph(
			WINDOW_SIZE_Y / 2,
			WINDOW_SIZE_Y / 2,
			(double)WINDOW_SIZE_Y / GRAPH_SIZE_Y,
			cnt_rota * PI / 2 + window_rota.sin_check(),
			screen_data.bottom.img_handle,
			false);
		if (window_rota.sin_check() >= window_rota.get_domain()){
			window_rota.sin_set(30, (double)PI / 2);
			rota_pattern = 0;
			if (cnt_rota >= 3){
				cnt_rota = 0;
			}
			else{
				cnt_rota++;
			}
		}
		break;
	}


	//top
	DrawRotaGraph(
		WINDOW_SIZE_Y + (WINDOW_SIZE_X-WINDOW_SIZE_Y) / 2,
		WINDOW_SIZE_Y / 2,
		(double)WINDOW_SIZE_Y / GRAPH_SIZE_Y,
		PI / 2,
		screen_data.top.img_handle,
		false);


	//マスク処理
	SetDrawBlendMode(DX_BLENDMODE_SUB, 255);
	DrawExtendGraph(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y,front_mask_handle, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	SetDrawMode(DX_DRAWMODE_NEAREST);
}
