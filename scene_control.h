#include "back_animation.h"
#include "scene_demo.h"
#include "scene_select_music.h"
#include "system_tools.h"

class scene_control
{
private:
	back_animation wave_animation;

	Cfont_handle credit;


	enum scene_pattern{
		DEMO,
		SELECT_MUSIC
	};

	scene_pattern pattern;

	scene_demo scene_demo_;
	scene_select_music scene_select_music_;
public:
	scene_control();
	void scene_allocation(SCREEN_DATA *screen_data);

};
