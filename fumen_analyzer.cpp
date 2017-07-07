#include "fumen_analyzer.h"
#include "str_type.h"




//文字列paramが譜面解析に使用する文字列かどうか判定
//使用しない文字列の時は-1、存在する場合はmark_dataのインデックスを返す
int Cfumen_analyzer::check_mark(string param)
{
	string tmp = "";

	if (SJISMultiCheck(param[f_analyze.now_str_index]))
	{
		tmp += param[f_analyze.now_str_index];
		tmp += param[f_analyze.now_str_index + 1];

		f_analyze.now_str_index++;
		f_analyze.now_str_index++;

		for (int i = 0; i < STR_2BYTE_SIZE; i++)
		{
			if (!strcmp(tmp.c_str(), mark_data[i].c_str()))
			{
				return i;
			}
		}
	}

	else{
		tmp += param[f_analyze.now_str_index];

		f_analyze.now_str_index++;

		for (int i = 0; i < STR_2BYTE_SIZE; i++)
		{
			if (!strcmp(tmp.c_str(), mark_data[i].c_str()))
			{
				return i;
			}
		}
	}

	return -1;
}


//文字データcが、2バイト文字の1バイト目か、1バイト文字か判定
//1で2バイト文字の1バイト目、0で1バイト文字
int Cfumen_analyzer::SJISMultiCheck(unsigned char c)
{
	if (((c >= 0x81) && (c <= 0x9f)) || ((c >= 0xe0) && (c <= 0xfc)))return 1;
	else return 0;
}


//拍数に応じた一拍当たりの経過時間を取得
double Cfumen_analyzer::get_note_time(int cnt_haku,double bpm)
{
	double tmp = 60 * 1000 / bpm;

	if (cnt_haku >= 4)
	{
		double ret = tmp / (double)cnt_haku;
		return ret;
	}
	else
	{
		double ret = tmp / (double)4 * cnt_haku / 4;
		return ret;
	}
}


//譜面データを解析
void Cfumen_analyzer::load_line_str(string param)
{
	f_analyze.mode = PHASE_POSITION_LOAD;
	f_analyze.now_str_index = 0;

	while (param[f_analyze.now_str_index] != '\0')
	{
		int ret = check_mark(param);

		if (ret == -1)
		{
			continue;
		}

		switch (f_analyze.mode)
		{

		case PHASE_POSITION_LOAD:

			if (ret >= STR_01 && ret <= STR_45)
			{
				bool add_flag = true;

				for (int i = f_analyze.last_shosetu_index + 1; i < note_data.size(); i++){

					if (note_data[i].note_num == ret)
					{
						add_flag = false;
						note_data[i].m_state[f_analyze.button_pos] = M_NORMAL;
						break;
					}
				}

				if (add_flag == true)
				{
					add_note_data();
					note_data[note_data.size() - 1].m_state[f_analyze.button_pos] = M_NORMAL;
					note_data[note_data.size() - 1].note_num = ret;
				}
			}

			if (ret >= STR_01 && ret <= STR_POS_SPACE2)
			{
				button_pos_update();
			}

			if (ret == STR_TIME_SPACE1 || ret == STR_TIME_SPACE2)
			{
				f_analyze.mode = PHASE_CNT_HAKU;
				f_analyze.cnt_haku = 0;
				f_analyze.cnt_haku_index = f_analyze.now_str_index;
			}
			break;


		case PHASE_CNT_HAKU:
			if (ret == STR_TIME_SPACE1 || ret == STR_TIME_SPACE2)
			{
				f_analyze.mode = PHASE_TIME_LOAD; 
				f_analyze.now_str_index = f_analyze.cnt_haku_index;

			}
			break;


		case PHASE_TIME_LOAD:
			if (ret >= STR_01 && ret <= STR_45)
			{
				bool add_flag = true;

				for (int i = f_analyze.last_shosetu_index + 1; i < note_data.size(); i++){

					if (note_data[i].note_num == ret)
					{
						add_flag = false;
						note_data[i].m_state[f_analyze.button_pos] = M_NORMAL;
						break;
					}
				}

				if (add_flag == true)
				{
					add_note_data();
					f_analyze.now_time += get_note_time(f_analyze.cnt_haku, f_analyze.now_bpm);
					note_data[note_data.size() - 1].note_num = ret;
					note_data[note_data.size() - 1].time = f_analyze.now_time;
				}
			}

			if (ret >= STR_01 && ret <= STR_POS_SPACE2)
			{
				button_pos_update();
			}

			break;
		}
	}
}


//譜面解析に使用する変数の初期化処理
void Cfumen_analyzer::init_value()
{
	MyOutputDebugString("execute init_value.\n");
	f_analyze.mode = PHASE_POSITION_LOAD;
	f_analyze.now_str_index = 0;
	f_analyze.now_bpm = 0;
	f_analyze.now_time = 0.0;
	f_analyze.button_pos = 0;
	f_analyze.last_shosetu_index = 0;
}


//現在の小節内にノーツデータを追加する
//ノーツデータ内部の値は初期化される
void Cfumen_analyzer::add_note_data()
{
	note_data.emplace_back();

	note_data[note_data.size() - 1].bpm = 0;
	note_data[note_data.size() - 1].time = 0;

	for (int i = 0; i < 16; i++)
	{
		note_data[note_data.size() - 1].m_state[i] = M_NONE;
	}
}


//譜面データを解析する
//paramの譜面データを読み込む
bool Cfumen_analyzer::fumen_analyze(music_info *param)
{
	log_flag = false;
	int cnt_loading_time = GetNowCount();

	myinfo = param;
	string fumen_path = myinfo->get_fumen_path();

	MyOutputDebugString("--------------------------------------------------\n");
	MyOutputDebugString("fumen path %s\n", fumen_path.c_str());

	ifstream ifs(fumen_path.c_str());
	if (ifs.fail())
	{
		MyOutputDebugString("load failed.\n");
		MyOutputDebugString("--------------------------------------------------\n");
		return false;
	}
	MyOutputDebugString("--------------------------------------------------\n");

	init_value();

	string get_line_str;

	MyOutputDebugString("fumen data analyzeing...\n");
	while (getline(ifs, get_line_str))
	{
		load_line_str(get_line_str);
	}
	MyOutputDebugString("completed.\n");
	MyOutputDebugString("------------------------------\n");




	play.start_time = GetNowCount();
	return true;
}


void Cfumen_analyzer::button_pos_update()
{
	if (f_analyze.button_pos < 15)f_analyze.button_pos++;
	else f_analyze.button_pos = 0;
}


void Cfumen_analyzer::draw_fumen()
{
	for (int fumen_index = 0; fumen_index < fumen_data.size(); fumen_index++)
	{
		for (int note_index = 0; note_index < fumen_data[fumen_index].note.size(); note_index++)
		{
			int frame = (int)((GetNowCount() - play.start_time - fumen_data[fumen_index].note[note_index].time) / 35);

			if (frame >= 0 && frame < 25)
			{
				for (int pos = 0; pos < 16; pos++)
				{
					int x, y;
					GetButtonPos(pos + 1, &x, &y);
					switch (fumen_data[fumen_index].note[note_index].m_state[pos])
					{
					case M_NORMAL:
						DrawExtendGraph(
							x,
							y,
							x + BUTTON_SIZE,
							y + BUTTON_SIZE,
							play.m_normal[frame],
							true);
						break;
					}
				}
			}
		}
	}
}

Cfumen_analyzer::Cfumen_analyzer()
{
	LoadDivGraph("rc/img/shutter.png", 25, 5, 5, 160, 160, play.m_normal);
	LoadDivGraph("rc/img/_shutter.png", 25, 5, 5, 160, 160, play.m_perfect);
}

Cfumen_analyzer::~Cfumen_analyzer()
{
	DeleteGraph(play.m_normal[0]);
	DeleteGraph(play.m_perfect[0]);
}
