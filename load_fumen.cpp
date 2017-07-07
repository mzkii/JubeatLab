#include "load_fumen.h"

using namespace std;

#define STR_2BYTE_SIZE 55

const string str_data_2byte[STR_2BYTE_SIZE] = {

	"①", "②", "③", "④", "⑤", "⑥", "⑦", "⑧", //7
	"⑨", "⑩", "⑪", "⑫", "⑬", "⑭", "⑮", "⑯", //15
	"⑰", "⑱", "⑲", "⑳", "Ａ", "Ｂ", "Ｃ", "Ｄ", //23
	"Ｅ", "Ｆ", "Ｇ", "Ｈ", "Ｉ", "Ｊ", "Ｋ", "Ｌ", //31
	"Ｍ", "Ｎ", "Ｏ", "Ｐ", "Ｑ", "Ｒ", "Ｓ", "Ｔ", //39
	"Ｕ", "Ｖ", "Ｗ", "Ｘ", "Ｙ", "Ｚ", "口", "□", //47
	"＜", "＞", "∨", "∧", "｜", "|" , "―", "－"  //55
};

int CLoad_fumen::SJISMultiCheck(unsigned char c)
{
	if (((c >= 0x81) && (c <= 0x9f)) || ((c >= 0xe0) && (c <= 0xfc)))return 1;
	else return 0;
}

_MARK_DATA CLoad_fumen::check_mark(string str)
{
	string tmp = "";
	_MARK_DATA ret;

	ret.mark = "";
	ret.number = -1;

	//2バイト文字
	if (SJISMultiCheck(str[index]))
	{
		tmp += str[index];
		tmp += str[index + 1];

		for (int i = 0; i < STR_2BYTE_SIZE; i++)
		{
			if (!strcmp(tmp.c_str(), str_data_2byte[i].c_str()))
			{
				ret.mark += str_data_2byte[i];
				ret.number = i;
				break;
			}
		}
		index++;
		index++;
	}
	else
	{
		tmp += str[index];

		for (int i = 0; i < STR_2BYTE_SIZE; i++)
		{
			if (!strcmp(tmp.c_str(),str_data_2byte[i].c_str()))
			{
				ret.mark += str_data_2byte[i];
				ret.number = i;
				break;
			}
		}
		index++;
	}
	return ret;
}

void CLoad_fumen::load_line(string get_str)
{
	if (get_str.find("t=", 0) != string::npos)
	{
		sscanf_s(get_str.c_str(), "t=%d", &now_bpm, (unsigned)sizeof(now_bpm));
		MyOutputDebugString("tコマンドを読み込みました。bpm : %d\n", now_bpm);
		return;
	}
	if (get_str.find("o=", 0) != string::npos)
	{
		sscanf_s(get_str.c_str(), "o=%d", &offset, (unsigned)sizeof(offset));
		MyOutputDebugString("oコマンドを読み込みました。offset : %d\n", offset);
		return;
	}
	if (get_str.find("#log", 0) != string::npos)
	{
		log_flag = true;
		MyOutputDebugString("logコマンドを読み込みました。\n");
		return;
	}
	
	mode = POSITION_LOAD;
	index = 0;

	while (get_str[index] != '\0')
	{
		_MARK_DATA ret = check_mark(get_str);

		switch (mode)
		{
		case POSITION_LOAD:

			if (ret.number == 52 || ret.number == 53)
			{
				mode = STR_LOAD;
				start_index = index;
				break;
			}

			if (ret.number >= 0 && ret.number <= 45)
			{
				int shosetu_size = shosetu_data.size();
				int note_size = shosetu_data[shosetu_size - 1].note_data.size();
				bool new_flag = true;

				for (int i = 0; i < note_size; i++)
				{
					if (shosetu_data[shosetu_size-1].note_data[i].num == ret.number)
					{
						shosetu_data[shosetu_size - 1].note_data[i].m_mode[button_pos] = MARKER_NORMAL;
						new_flag = false;
					}
				}

				if (new_flag == true)
				{
					add_note_data(ret.number);
					note_size = shosetu_data[shosetu_size - 1].note_data.size();
					shosetu_data[shosetu_size - 1].note_data[note_size - 1].m_mode[button_pos] = MARKER_NORMAL;
				}
			}

			if (ret.number >= 0 && ret.number <= 47)
			{
				if (button_pos < 15)button_pos++;
				else button_pos = 0;
			}

			else if (ret.number >= 54 && ret.number <= 55)
			{
				if (button_pos < 15)button_pos++;
				else button_pos = 0;
			}

			else if (ret.number >= 48 && ret.number <= 51)
			{
				//	"＜", "＞", "∨", "∧", "｜", "|" , "―", "－"  //55

				if (ret.number == 48)check_hold_state[button_pos].h_mode = HOLD_L_START;
				if (ret.number == 49)check_hold_state[button_pos].h_mode = HOLD_R_START;
				if (ret.number == 50)check_hold_state[button_pos].h_mode = HOLD_D_START;
				if (ret.number == 51)check_hold_state[button_pos].h_mode = HOLD_T_START;

				check_hold_state[button_pos].start_shosetu_index = -1;
				check_hold_state[button_pos].start_note_index = -1;
				check_hold_state[button_pos].end_button_position = -1;

				if (button_pos < 15)button_pos++;
				else button_pos = 0;
			}
			break;


		case STR_LOAD:

			//拍数先読み
			cnt_haku = 0;

			for (; get_str[index] != '\0'; ret = check_mark(get_str))
			{
				if (ret.number >= 0 && ret.number <= 45)
				{
					cnt_haku++;
				}
				if (ret.number == 52 || ret.number == 53)
				{
					break;
				}
				if (ret.number == 54 || ret.number == 55)
				{
					cnt_haku++;
				}
			}

			index = start_index;
			mode = TIME_LOAD;
			break;



		case TIME_LOAD:

			if (ret.number == 52 || ret.number == 53)
			{
				cnt_line++;
				break;
			}
			if (ret.number >= 0 && ret.number <= 45)
			{
				int shosetu_size = shosetu_data.size();
				int note_size = shosetu_data[shosetu_size - 1].note_data.size();
				bool new_flag = true;

				for (int i = 0; i < note_size; i++)
				{
					if (shosetu_data[shosetu_size - 1].note_data[i].num == ret.number)
					{
						shosetu_data[shosetu_size - 1].note_data[i].time = all_time;
						all_time += get_note_time(cnt_haku);
						new_flag = false;
					}
				}

				if (new_flag == true)
				{
					add_note_data(ret.number);
					note_size = shosetu_data[shosetu_size - 1].note_data.size();

					shosetu_data[shosetu_size - 1].note_data[note_size - 1].time = all_time;
					all_time += get_note_time(cnt_haku);
				}
			}

			if (ret.number == 55 || ret.number == 54)
			{
				all_time += get_note_time(cnt_haku);
			}
			break;
		}
	}
	if (cnt_line >= 4){
		cnt_line = 0;
		shosetu_sort();
		if (log_flag==true)shosetu_draw();
		add_shosetu();
		if (log_flag==true)OutputDebugString("小節を追加しました。\n");
	}
}
double CLoad_fumen::get_note_time(int cnt_haku)
{
	double tmp = 60 * 1000 / (double)now_bpm;

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

int CLoad_fumen::load_fumen(music_info *param)
{
	myinfo = param;

	log_flag = false;

	string fumen_path = myinfo->get_fumen_path();

	MyOutputDebugString("-------------------------------------------------------------------------------------------\n");
	MyOutputDebugString("%s の読み込みを開始します。\n", fumen_path.c_str());

	ifstream ifs(fumen_path.c_str());

	std::string load_line_str;

	if (ifs.fail())
	{
		MyOutputDebugString("[CLoad_fumen]  %s の読み込みに失敗しました。\n", fumen_path.c_str());
		MyOutputDebugString("-------------------------------------------------------------------------------------------\n");
		return -1;
	}

	for (int i = 0; i < 16; i++)
	{
		hand_clap[i].load("rc/ef/ef_hcp.wav");
	}

	for (int i = 0; i < 16; i++)
	{
		check_hold_state[i].h_mode = HOLD_NONE;
		check_hold_state[i].start_shosetu_index = -1;
		check_hold_state[i].start_note_index = -1;
		check_hold_state[i].end_button_position = -1;
	}

	myfont.create_font_handle("meiryo ui", 150, 6, DX_FONTTYPE_ANTIALIASING_8X8);
	now_clap = 0;
	offset = 0;
	cnt_line = 0;
	all_time = 0;
	button_pos = 0;
	now_bpm = 100;
	cnt_file_line = 0;
	add_shosetu();

	while (getline(ifs, load_line_str))
	{
		load_line(load_line_str);
		cnt_file_line++;
	}

	play_pattern = 0;
	finish_flag = false;

	MyOutputDebugString("------------------ロード完了----------------------------------------------------------------\n");
	return 0;
}

void CLoad_fumen::add_shosetu()
{
	shosetu_data.emplace_back();
}

//現在の小節内にノーツを追加する
void CLoad_fumen::add_note_data(int _note_num)
{
	shosetu_data[shosetu_data.size() - 1].note_data.emplace_back();

	int shosetu_index = shosetu_data.size() - 1;
	int note_index = shosetu_data[shosetu_index].note_data.size() - 1;

	for (int i = 0; i < 16; i++)
	{
		shosetu_data[shosetu_index].note_data[note_index].m_mode[i] = MARKER_NONE;
	}
	shosetu_data[shosetu_index].note_data[note_index].hold_end_time = -1;
	shosetu_data[shosetu_index].note_data[note_index].time = -1;
	shosetu_data[shosetu_index].note_data[note_index].num = _note_num;
	shosetu_data[shosetu_index].note_data[note_index].bpm = now_bpm;
}

void CLoad_fumen::shosetu_sort()
{
	int shosetu_index = shosetu_data.size() -1;
	int note_index = shosetu_data[shosetu_index].note_data.size();

	if (note_index < 2)return;


	/* ノーツデータを降順にソート */
	for (int i = 0; i < note_index; ++i) {

		for (int j = i + 1; j < note_index; ++j) {
			if (shosetu_data[shosetu_index].note_data[i].num > shosetu_data[shosetu_index].note_data[j].num) {

				NOTE tmp = shosetu_data[shosetu_index].note_data[i];
				shosetu_data[shosetu_index].note_data[i] = shosetu_data[shosetu_index].note_data[j];
				shosetu_data[shosetu_index].note_data[j] = tmp;
			}
		}
	}

	for (int i = 0; i < 16; i++)
	{
		//	"＜", "＞", "∨", "∧", "｜", "|" , "―", "－"  //55

		switch (check_hold_state[i].h_mode)
		{

		case HOLD_L_START:

			for (int j = 0; j < note_index; j++)
			{
				int  last_pos = 0;

				for (int k = 0; k < i; k++)
				{
					if (shosetu_data[shosetu_index].note_data[j].m_mode[k] == MARKER_NORMAL)
					{
						last_pos = k;
					}
				}
				check_hold_state[i].end_button_position = last_pos;
				check_hold_state[i].start_note_index = j;
				check_hold_state[i].start_shosetu_index = shosetu_index;

				shosetu_data[shosetu_index].note_data[j].m_mode[last_pos] = MARKER_HOLD;
			}
			break;

		case HOLD_R_START:

			for (int j = 0; j < note_index; j++)
			{
				int  last_pos = 15;

				for (int k = 15; k > i; k--)
				{
					if (shosetu_data[shosetu_index].note_data[j].m_mode[k] == MARKER_NORMAL)
					{
						last_pos = k;
					}
				}
				check_hold_state[i].end_button_position = last_pos;
				check_hold_state[i].start_note_index = j;
				check_hold_state[i].start_shosetu_index = shosetu_index;

				shosetu_data[shosetu_index].note_data[j].m_mode[last_pos] = MARKER_HOLD;
			}
			break;

		case HOLD_D_START:

			for (int j = 0; j < note_index; j++)
			{
				int  last_pos = 12 + i % 4;

				for (int k = 12 + i % 4; k > i; k -= 4)
				{
					if (shosetu_data[shosetu_index].note_data[j].m_mode[k] == MARKER_NORMAL)
					{
						last_pos = k;
					}
				}
				check_hold_state[i].end_button_position = last_pos;
				check_hold_state[i].start_note_index = j;
				check_hold_state[i].start_shosetu_index = shosetu_index;

				shosetu_data[shosetu_index].note_data[j].m_mode[last_pos] = MARKER_HOLD;
			}
			break;

		case HOLD_T_START:

			for (int j = 0; j < note_index; j++)
			{
				int  last_pos = i % 4;

				for (int k = i % 4; k < i; k += 4)
				{
					if (shosetu_data[shosetu_index].note_data[j].m_mode[k] == MARKER_NORMAL)
					{
						last_pos = k;
					}
				}
				check_hold_state[i].end_button_position = last_pos;
				check_hold_state[i].start_note_index = j;
				check_hold_state[i].start_shosetu_index = shosetu_index;

				shosetu_data[shosetu_index].note_data[j].m_mode[last_pos] = MARKER_HOLD;
			}
			break;

		default:
			continue;

		}
	}
}

void CLoad_fumen::shosetu_draw()
{
	int shosetu_index = shosetu_data.size();
	int note_index = shosetu_data[shosetu_index-1].note_data.size();

	MyOutputDebugString("-------------------- %d 小節目 --------------------\n", shosetu_index);

	for (int i = 0; i < note_index; i++)
	{
		int tmp = shosetu_data[shosetu_index - 1].note_data[i].num;
		
		MyOutputDebugString(" [ note_num : %s ] ", str_data_2byte[tmp].c_str());
		MyOutputDebugString(" [ time : %10.3f ] ", shosetu_data[shosetu_index - 1].note_data[i].time);
		MyOutputDebugString(" [ bpm  : %3d ] ", shosetu_data[shosetu_index - 1].note_data[i].bpm);

		MyOutputDebugString(" [ ");
		
		for (int j = 0; j < 16; j++)
		{
			if (shosetu_data[shosetu_index - 1].note_data[i].m_mode[j] == MARKER_NORMAL)
			{
				MyOutputDebugString("%d ", j + 1);
			}
		}

		MyOutputDebugString("]\n");
	}
	MyOutputDebugString("---------------------------------------------------\n");
}

bool CLoad_fumen::play()
{

	//background

	switch (play_pattern)
	{
	case 0:
		SetUseASyncLoadFlag(TRUE);
		myinfo->sound_load();
		SetUseASyncLoadFlag(FALSE);
		play_pattern++;
		break;
		
	case 1:
		switch (CheckHandleASyncLoad(myinfo->get_sound_handle()))
		{
		case TRUE:
			break;

		case FALSE:
			play_pattern++;
			start_time = GetNowCount();
			break;

		case -1:
			return true;
			break;
		}
		break;

	case 2:
		if (keyBuff[KEY_INPUT_D] == 1)
		{
			return true;
		}
		if (keyBuff[KEY_INPUT_SPACE] != 0)
		{

		}
		if (keyBuff[KEY_INPUT_LEFT] != 0)
		{
			start_time += 40;
			SetSoundCurrentTime(GetNowCount() - start_time - offset, myinfo->get_sound_handle());
		}
		else if (keyBuff[KEY_INPUT_RIGHT] != 0)
		{
			start_time -= 10;
			SetSoundCurrentTime(GetNowCount() - start_time - offset, myinfo->get_sound_handle());
		}


		if (!CheckSoundMem(myinfo->get_sound_handle()))
		{
			if (GetNowCount() - start_time > offset)
			{
				PlaySoundMem(myinfo->get_sound_handle(), DX_PLAYTYPE_BACK);
			}
		}

		clap_flag = false;
		for (int i = 0; i < shosetu_data.size(); i++)
		{
			for (int j = 0; j < shosetu_data[i].note_data.size(); j++)
			{
				int frame = (int)((GetNowCount() - start_time) - shosetu_data[i].note_data[j].time) / MARKER_FRAME;

				for (int pos = 0; pos < 16; pos++)
				{
					if (shosetu_data[i].note_data[j].m_mode[pos] == MARKER_NORMAL)
					{
						int x, y,width;
						GetButtonPos(pos + 1, &x, &y);

						if (frame >= 0 && frame < 15)
						{
							DrawExtendGraph(x, y, x + BUTTON_SIZE, y + BUTTON_SIZE, marker[frame], true);

							width = GetDrawFormatStringWidthToHandle(
								myfont.get_font_handle(),
								"%d",
								shosetu_data[i].note_data[j].num + 1);

							DrawFormatStringToHandle(
								x + BUTTON_SIZE / 2 - width / 2,
								y + 20,
								GetColor(150, 150, 150),
								myfont.get_font_handle(),
								"%d",
								shosetu_data[i].note_data[j].num + 1);

						}
						if (frame >= 15 && frame  < 40)
						{
							DrawExtendGraph(x, y, x + BUTTON_SIZE, y + BUTTON_SIZE, perfect[frame - 15], true);
							if (frame == 14)clap_flag = true;
						}
					}
				}
			}
		}
		break;
	}

	return false;
}

CLoad_fumen::CLoad_fumen()
{
	
	LoadDivGraph("rc/img/shutter.png", 25, 5, 5, 160, 160, marker);
	LoadDivGraph("rc/img/shutter_.png", 25, 5, 5, 160, 160, perfect);
}

CLoad_fumen::~CLoad_fumen()
{
	DeleteGraph(marker[0]);
	DeleteGraph(perfect[0]);
}
