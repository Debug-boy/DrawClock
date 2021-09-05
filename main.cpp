#include <easyx.h>
#include <math.h>
#include <iostream>
#include <string>
#include <time.h>

#define FRAMER_WIDTH	800
#define FRAMER_HEIGHT	800

#define FRAMER_CENTER_X	(FRAMER_WIDTH / 2)
#define FRAMER_CENTER_Y	(FRAMER_HEIGHT / 2)

#define CALCUATE_ANG(rad)	(rad * 180.0f / 3.14.0f)
#define CALCUATE_RAD(ang)	(ang * 3.14 / 180.0f)


class Mclock {
	
private:
	int center_x, center_y, radius;
	IMAGE img_blackGround{};

public:
	Mclock(int x, int y, int radius) {
		this->center_x = x;
		this->center_y = y;
		this->radius = radius;
	}

	virtual ~Mclock() = default;


private:

	void draw_framer(COLORREF color) {
		setlinecolor(color);
		setlinestyle(PS_SOLID, 1);
		circle(
			this->center_x,
			this->center_y,
			this->radius
		);

		fillcircle(
			this->center_x,
			this->center_y,
			this->radius / 40
		);

	}

	void draw_time(COLORREF color_hour = RED, COLORREF color_minute = WHITE) {

		const float rotation_scale = 360 / 60;
		const unsigned int rotation_hour_scale = 360 / 12;
		const unsigned int rotation_minute_scale = 360 / 60;
		
		const float raduis2 = this->radius * 0.84;

		const float raduis3 = this->radius * 0.96;
		const float raduis4 = this->radius * 0.94;

		LOGFONT mFont_hour,mFont_minute;
		gettextstyle(&mFont_hour);
		gettextstyle(&mFont_minute);

		mFont_hour.lfHeight = 40;
		_tcscpy(mFont_hour.lfFaceName, _T("黑体"));
		mFont_hour.lfQuality = ANTIALIASED_QUALITY;


		float rotation_angle = 90;
		const float rotation_t = rotation_angle + 360;//旋转一个周期后的角度

		while (rotation_angle < rotation_t) {

			float rotate_x = cosf(CALCUATE_RAD(rotation_angle));
			float rotate_y = sinf(CALCUATE_RAD(rotation_angle));

			POINT p_start{};
			POINT p_end{};

			//判断此旋转角度是否为特殊角度点
			unsigned int rotate_select = !((unsigned int)rotation_angle % rotation_hour_scale);

			p_start = {
			center_x - (int)(rotate_x * this->radius),
			center_y - (int)(rotate_y * this->radius) };//圆上刻度的点

			p_end = {
			center_x - (int)(rotate_x * raduis3) ,
			center_y - (int)(rotate_y * raduis3) };//圆内刻度的点

			if (rotate_select) {

				//连接两点,绘制 小时 的刻度
				setlinecolor(RED);
				setlinestyle(PS_SOLID, 6);
				line(p_start.x, p_start.y, p_end.x, p_end.y);

				//绘制 小时 文字显示
				POINT hour_outxy{ 
					center_x + rotate_x * raduis2 - 9,
					center_y - rotate_y * raduis2 - 15 };

				unsigned int time_hour = 12 - (rotation_angle - 90) / rotation_hour_scale;
				std::string hour_str(std::to_string(time_hour));
				settextstyle(&mFont_hour);
				settextcolor(color_hour);
				outtextxy(hour_outxy.x, hour_outxy.y, hour_str.c_str());
			}
			else {

				//绘制 分钟 的描点
				setlinecolor(WHITE);
				fillcircle(p_start.x, p_start.y, 0.5);

				//绘制 分钟 文字显示
				POINT minute_outxy{
					center_x + rotate_x * raduis4 - 6,
					center_y - rotate_y * raduis4 - 10 };

				unsigned int time_minute = 60 - (rotation_angle - 90) / rotation_scale;
				std::string minute_str(std::to_string(time_minute));
				settextstyle(&mFont_minute);
				settextcolor(color_minute);
				outtextxy(minute_outxy.x, minute_outxy.y, minute_str.c_str());

			}


			rotation_angle += rotation_scale;
		}

	}

	void draw_vector() {

		time_t localTime = time(nullptr);
		tm* localTimeInfo = nullptr;
		localTimeInfo = localtime(&localTime);

		if (localTimeInfo) {

			float rotation_hour = CALCUATE_RAD((((double)localTimeInfo->tm_hour + 3) * (360 / 12)));
			float rotation_minute = CALCUATE_RAD((((double)localTimeInfo->tm_min + 15) * (360 / 60)));
			float rotation_sec = CALCUATE_RAD((((double)localTimeInfo->tm_sec + 15) * (360 / 60)));

			std::string sec_str;
			sec_str += std::to_string(localTimeInfo->tm_hour);
			sec_str += ":";
			sec_str += std::to_string(localTimeInfo->tm_min);
			sec_str += ":";
			sec_str += std::to_string(localTimeInfo->tm_sec);
			outtextxy(50, 50, sec_str.c_str());

			const float vec_hour_size = this->radius * 0.4;
			const float vec_minute_size = this->radius * 0.75;
			const float vec_sec_size = this->radius * 0.9;

			POINT vector_hour{};
			POINT vector_minute{};
			POINT vector_sec{};

			vector_hour.x = this->center_x - cosf(rotation_hour) * vec_hour_size;
			vector_hour.y = this->center_y - sinf(rotation_hour) * vec_hour_size;

			vector_minute.x = this->center_x - cosf(rotation_minute) * vec_minute_size;
			vector_minute.y = this->center_y - sinf(rotation_minute) * vec_minute_size;

			vector_sec.x = this->center_x - cosf(rotation_sec) * vec_sec_size;
			vector_sec.y = this->center_y - sinf(rotation_sec) * vec_sec_size;

			setlinestyle(PS_SOLID, 12);
			setlinecolor(CYAN);
			line(
				this->center_x, this->center_y,
				vector_hour.x, vector_hour.y);

			setlinestyle(PS_SOLID, 4);
			setlinecolor(YELLOW);
			line(
				this->center_x, this->center_y,
				vector_minute.x, vector_minute.y);

			setlinestyle(PS_SOLID, 2);
			setlinecolor(WHITE);
			line(
				this->center_x, this->center_y,
				vector_sec.x, vector_sec.y);

		}

	}

	void draw_blackGround() {
		putimage(0, 0, &this->img_blackGround);
	}

	void eraseBack() {
		cleardevice();
	}


public:

	void setBackGroundImg(LPCSTR img_dir) {
		loadimage(&this->img_blackGround, img_dir);
	}

	void render() {
		while (true) {
			this->draw_blackGround();
			this->draw_framer(WHITE);
			this->draw_time(WHITE, GREEN);
			this->draw_vector();
			Sleep(1000);
			this->eraseBack();
		}

	}

};


int main(int argc,char *argv[]) {

	Mclock mClock(FRAMER_CENTER_X, FRAMER_CENTER_Y, 300);
	initgraph(FRAMER_WIDTH, FRAMER_HEIGHT);

	mClock.setBackGroundImg("black_sky.jpg");
	mClock.render();


	getchar();
	closegraph();

	return 0;
}