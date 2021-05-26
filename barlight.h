#ifndef BARLIGHT_H
#define BARLIGHT_H

#include <stdint.h>

struct ws2811_t;

class BarLightStrip
{
public:
	BarLightStrip();
	~BarLightStrip();
	bool init(double gamma=0);

	int exec();
	void render();

	int ledCount() const;

	void setGamma(double gamma);
	double gamma() const;

	void clear();
	void setColor(int idx, uint8_t r, uint8_t g, uint8_t b);
	void setColor(uint8_t r, uint8_t g, uint8_t b);

	void setRainbowColor(int cnt);
	void calcRainbowColor(int pos, uint8_t &r, uint8_t &g, uint8_t &b);
private:
	void mapIndex(int idx, int &ledIdx, int &channel);

	ws2811_t* _ws2811;
	double _gamma;
};

#endif // BARLIGHT_H
