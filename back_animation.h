class back_animation
{
private:
	struct{
		int wave_y;
		double phase;
		double speed;
		int amp;
		int thickness;
		double circle;
	}wave_data[4];

	int before_pos[4];

public:
	back_animation();
	void draw(int y, int x_size, int y_size);
};
