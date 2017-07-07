#include "system_tools.h"
#define MARKER_FRAME 35


using namespace std;

class Cfumen_analyzer
{

private:
	enum ANALYZE_MODE
	{
		POS_LOAD,
		TIME_LOAD
	};

	enum MARKER_STATE
	{
		M_NONE,
		M_NORMAL,
		M_HOLD_START,
		M_HOLD_END
	};

	enum FUMEN_ANALYZE
	{
		PHASE_POSITION_LOAD,
		PHASE_CNT_HAKU,
		PHASE_TIME_LOAD
	};

	int check_mark(string param);
	int SJISMultiCheck(unsigned char c);
	double get_note_time(int cnt_haku, double bpm);
	void init_value();
	void add_note_data();
	void load_line_str(string param);
	void button_pos_update();

	typedef struct NOTE_DATA
	{
		MARKER_STATE m_state[16];
		int bpm;
		int note_num;
		double time;
	};

	struct
	{
		FUMEN_ANALYZE mode;
		int now_str_index;
		int now_bpm;
		double now_time;
		int button_pos;
		int last_shosetu_index;
		int cnt_haku;
		int cnt_haku_index;

	}f_analyze;

	struct
	{
		int start_time;
		int m_normal[25];
		int m_perfect[25];
	}play;

	music_info* myinfo;
	vector<NOTE_DATA> note_data;


	int cnt_fumen_index;
	bool log_flag;

	
public:
	Cfumen_analyzer();
	~Cfumen_analyzer();
	bool fumen_analyze(music_info *param);
	void draw_fumen();
};