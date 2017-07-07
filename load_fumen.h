#pragma once
#include "system_tools.h"
#include <vector>




typedef struct MARK_DATA{
	string mark;
	int number;
}_MARK_DATA;

class CLoad_fumen{

private:
	
	img_load background;
	int marker[25];
	int perfect[25];

	bool clap_flag;

	Cfont_handle myfont;

	enum hold_mode{

		HOLD_NONE,
		
		HOLD_L_START,
		HOLD_R_START,
		HOLD_T_START,
		HOLD_D_START,
	};

	struct {

		hold_mode h_mode;
		int start_shosetu_index;
		int start_note_index;

		int end_button_position;


	}check_hold_state[16];
	


	enum marker_mode{

		MARKER_NONE,
		MARKER_NORMAL,
		MARKER_HOLD
	};

	typedef struct NOTE{
		marker_mode m_mode[16];
		unsigned char num;
		double time;
		double hold_end_time;
		int bpm;
	};

	typedef struct SHOSETU{
		vector <NOTE> note_data;
	};

	vector <SHOSETU> shosetu_data;

	enum analyzer_mode{
		POSITION_LOAD,
		STR_LOAD,
		TIME_LOAD
	};

	music_info *myinfo;
	analyzer_mode mode;
	int offset;
	int now_bpm;
	int cnt_line;
	int button_pos;
	int index;
	double all_time;
	int cnt_file_line;
	void add_shosetu();
	void add_note();
	void add_note_data(int _note_num);
	void shosetu_sort();
	void shosetu_draw();
	double get_note_time(int cnt_haku);


	void load_line(string get_str);
	int SJISMultiCheck(unsigned char c);
	MARK_DATA check_mark(string str);


	ef_load hand_clap[16];
	int now_clap;
	int cnt_haku;
	int start_index;
	int play_pattern;
	bool finish_flag;
	bool log_flag;
	int start_time;
public:
	int load_fumen(music_info *param);
	bool play();
	CLoad_fumen();
	~CLoad_fumen();
};