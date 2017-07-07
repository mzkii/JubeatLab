#include "main_loop.h"
#include "system_tools.h"



class scene_demo
{
private:
	int state;
	img_load logo;
	img_load touch_icon;
	ef_load welcome;
	ef_load background;
	ef_load enter;
	change_animation fade;
	sin_wave<int> touch_fade;
	SCREEN_DATA *scr_data;
public:
	scene_demo();
	~scene_demo();
	SCENE_STATE update(SCREEN_DATA *screen_data);
	void bottom_draw(int x_size,int y_size);
	void top_draw(int x_size, int y_size);

};