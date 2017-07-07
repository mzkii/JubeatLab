#pragma once

#include <math.h>
#include "main_loop.h"



template<class TYPE> class sin_wave
{

private:
	int count;
	int frame;
	TYPE domain;
	TYPE ret;

public:
	void sin_set(int get_frame, TYPE get_domain);
	TYPE type1_sin_update_and_return();
	TYPE type2_sin_update_and_return();
	void sin_reset();
	TYPE sin_check();
	TYPE get_domain();
};

#include "sin_wave_.h"