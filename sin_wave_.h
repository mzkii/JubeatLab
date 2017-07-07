


template <class TYPE>
void sin_wave<TYPE>::sin_set(int get_frame, TYPE get_domain){
	count = 0;
	ret = 0;
	frame = get_frame;
	domain = get_domain;

}


template <class TYPE>
TYPE sin_wave<TYPE>::type1_sin_update_and_return(){
	ret = (TYPE)(sin(PI / 2 / frame * count) * domain);
	count++;
	return ret;

}


template <class TYPE>
TYPE sin_wave<TYPE>::type2_sin_update_and_return(){
	ret = (TYPE)((sin(-PI / 2 + PI / frame * count) + 1) / 2 * domain);
	count++;
	return ret;
}


template <class TYPE>
void sin_wave<TYPE>::sin_reset(){
	count = 0;
	ret = 0;
}


template <class TYPE>
TYPE sin_wave<TYPE>::sin_check(){
	return ret;
}


template <class TYPE>
TYPE sin_wave<TYPE>::get_domain(){
	return domain;
}

