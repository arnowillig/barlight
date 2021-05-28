#ifndef BARLIGHT_H
#define BARLIGHT_H

#include <stdint.h>
#include <vector>
#include <string>

class LightStrip;

class LightStripSegment
{
public:
	LightStripSegment(int firstLed, int lastLed, int channel, int bri);
	int ledCount() const;
	int firstLed() const;
	int lastLed() const;
	int channel() const;

	void setStrip(LightStrip* strip);

	void clear();
	void setColor(int idx, uint8_t r, uint8_t g, uint8_t b);
	void setColor(uint8_t r, uint8_t g, uint8_t b);
	void setRainbowColor(int cnt);

	int brightness() const;
	void setBrightness(int bri);

private:
	int _firstLed;
	int _lastLed;
	int _channel;
	int _bri;
	LightStrip* _strip;
};

struct ws2811_t;

class LightStrip
{
public:
	LightStrip();
	~LightStrip();

	void addSegment(LightStripSegment* segment);
	bool init(int bri=128, double gamma=0);

	int exec();
	void render();

	int ledCount() const;
	int segmentCount() const;

	void setGamma(double gamma);
	double gamma() const;

	LightStripSegment* segment(int idx) const;

	void clear();
	void setColor(int idx, int chan, uint8_t r, uint8_t g, uint8_t b);
	void setColor(int idx, uint8_t r, uint8_t g, uint8_t b);
	void setColor(uint8_t r, uint8_t g, uint8_t b);
	void setLastColor();

	void setRainbowColor(int cnt);
	static	void calcRainbowColor(int pos, uint8_t &r, uint8_t &g, uint8_t &b);

	// API
	uint8_t brightness() const;
	void setBrightness(uint8_t bri);
	std::string mode() const;
	void setMode(const std::string &mode);

private:
	int getMaxLedForChannel(int chan);

	ws2811_t* _ws2811;
	double _gamma;
	std::vector<LightStripSegment*> _segments;
	uint32_t _color;
	std::string _mode;
};

#endif // BARLIGHT_H
