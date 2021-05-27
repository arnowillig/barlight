#ifndef BARLIGHT_H
#define BARLIGHT_H

#include <stdint.h>
#include <vector>

class LightStrip;

class LightStripSegment
{
public:
	LightStripSegment(int firstLED, int lastLED, int channel);
	int ledCount() const;
	int firstLed() const;
	int lastLED() const;
	int channel() const;

	void setStrip(LightStrip* strip);

	void clear();
	void setColor(int idx, uint8_t r, uint8_t g, uint8_t b);
	void setColor(uint8_t r, uint8_t g, uint8_t b);

private:
	int _firstLed;
	int _lastLED;
	int _channel;
	LightStrip* _strip;
};

struct ws2811_t;

class LightStrip
{
public:
	LightStrip();
	~LightStrip();

	void addSegment(LightStripSegment* segment);
	bool init(double gamma=0);

	int exec();
	void render();

	int ledCount() const;
	int segmentCount() const;

	void setGamma(double gamma);
	double gamma() const;

	LightStripSegment* segment(int idx) const;

	void clear();
	void setColor(int idx, uint8_t r, uint8_t g, uint8_t b);
	void setColor(uint8_t r, uint8_t g, uint8_t b);

	void setRainbowColor(int cnt);
	void calcRainbowColor(int pos, uint8_t &r, uint8_t &g, uint8_t &b);
private:
	int getMaxLedForChannel(int chan);

	ws2811_t* _ws2811;
	double _gamma;
	std::vector<LightStripSegment*> _segments;
};

#endif // BARLIGHT_H
