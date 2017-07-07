#include "main_loop.h"
#include <string>
#include <vector>
#include "system_tools.h"

#include "fumen_analyzer.h"


using namespace std;


#define MAIN_FONT_SIZE		40
#define SUB_FONT_SIZE		17
#define ARTIST_FONT_SIZE	30


enum SORTTYPE{
	SORTTYPE_ALL,
	SORTTYPE_LEVEL_1,
	SORTTYPE_LEVEL_2,
	SORTTYPE_LEVEL_3,
	SORTTYPE_LEVEL_4,
	SORTTYPE_LEVEL_5,
	SORTTYPE_LEVEL_6,
	SORTTYPE_LEVEL_7,
	SORTTYPE_LEVEL_8,
	SORTTYPE_LEVEL_9,
	SORTTYPE_LEVEL_10,
	SORTTYPE_DIF_BASIC,
	SORTTYPE_DIF_ADVANCED,
	SORTTYPE_DIF_EXTREME,
	SORTTYPE_HISTORY
};

enum level{
	BASIC, ADVANCED, EXTREME
};



class scene_select_music
{

private:
	void (scene_select_music::*func_)();				// メソッドポインタ
	void setState(void (scene_select_music::*func)());  // ステート設定
	void make_memory();
	void loading_rc();
	void select_music();
	void matching();
	void play();

	SCREEN_DATA *my_scr;

	MUSIC_DATA *key;
	MUSIC_DATA *now;

	vector<music_info> music_data;
	change_animation fade;

	struct{
		HANDLE hFind;
		WIN32_FIND_DATA find;
		string fumen_path[3];
	}load_data;


	struct{
		int loading_pattern;
		int load_num;
	}loading;


	struct{
		int title_position;
		int title_width;
		int play_pattern;
		int play_time;
		int jacket_position;
		int jaclet_add;
		int cnt_push;
		int jacket_move_temp;
		int jacket_move_pattern;
		int now_music_index;
		int now_dif_index;
		int cnt_cd_rota;
		int select_pattern;
		int animation_pattern;
		int jacket_alpha;
		int cd_alpha;
		int img_cd[3];
		int img_buttons[6];

		double jacket_rate;
		double cd_rate;

		bool select_flag;

		sin_wave<int> volume;
		sin_wave<int> jacket_move_sin;
		sin_wave<int> jacket_change;
		sin_wave<double> level_change;

		ef_load ef_level_voice[3];
		ef_load ef_jacket_move;
		ef_load ef_jacket_select;
		ef_load ef_enter;
		ef_load select_music;

		Cfont_handle sub_font;
		Cfont_handle main_font;
		Cfont_handle artist_font;
		Cbutton draw_buttons[4];
		img_load jacket_back;
	}select;

	struct{
		int pattern;
		Cfumen_analyzer *fumen;
	}fumen_play;


	int debug_time;
	void sort(SORTTYPE param);
	void jacket_draw();
	void jacket_move();
	void top_draw();
	void check_jaclet_select();
	void select_animation();
	void jacket_select(int new_index, int old_index);
	void init();
public:
	SCENE_STATE update(SCREEN_DATA *screen_data);
	scene_select_music();
	~scene_select_music();

};

