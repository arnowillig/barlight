#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "barlight.h"
#include "barlight.h"
#include "rpi_ws281x/ws2811.h"

#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN1               18
#define GPIO_PIN2               13
#define WS2811_DMA              10

LightStrip::LightStrip() : _ws2811(nullptr), _gamma(0.0)
{
}

LightStrip::~LightStrip()
{
	clear();
#ifndef _GUI_
	ws2811_render(_ws2811);
	ws2811_wait(_ws2811);
#endif
}

void LightStrip::addSegment(LightStripSegment* segment)
{
	segment->setStrip(this);
	_segments.push_back(segment);
}

int LightStrip::getMaxLedForChannel(int chan)
{
	int lastChannelLed = 0;
	for (const LightStripSegment* segment : _segments) {
		if (segment->channel() == chan) {
			if (segment->firstLed() > lastChannelLed) {
				lastChannelLed = segment->firstLed();
			}
			if (segment->lastLed() > lastChannelLed) {
				lastChannelLed = segment->lastLed();
			}
		}
	}
	return lastChannelLed+1;
}

bool LightStrip::init(int bri, double gamma)
{
	_ws2811 = (ws2811_t *) malloc(sizeof(ws2811_t));
	memset(_ws2811, 0, sizeof(ws2811_t));

	_ws2811->freq			= WS2811_TARGET_FREQ;
	_ws2811->dmanum			= WS2811_DMA;

	_ws2811->channel[0].gpionum	= GPIO_PIN1;
	_ws2811->channel[0].count	= getMaxLedForChannel(0);
	_ws2811->channel[0].strip_type	= WS2812_STRIP;
	_ws2811->channel[0].brightness	= bri;

	_ws2811->channel[1].gpionum	= GPIO_PIN2;
	_ws2811->channel[1].count	= getMaxLedForChannel(1);
	_ws2811->channel[1].strip_type	= WS2812_STRIP;
	_ws2811->channel[1].brightness	= bri;

#ifdef _GUI_
	_ws2811->channel[0].leds = (ws2811_led_t*) malloc(sizeof(ws2811_led_t) * _ws2811->channel[0].count);
	_ws2811->channel[1].leds = (ws2811_led_t*) malloc(sizeof(ws2811_led_t) * _ws2811->channel[1].count);
	memset(_ws2811->channel[0].leds, 0, sizeof(ws2811_led_t) * _ws2811->channel[0].count);
	memset(_ws2811->channel[1].leds, 0, sizeof(ws2811_led_t) * _ws2811->channel[1].count);
	ws2811_return_t ret = WS2811_SUCCESS;
#else
	ws2811_return_t ret = ws2811_init(_ws2811);
#endif
	if (ret != WS2811_SUCCESS) {
		fprintf(stderr, "Error: %s\n", ws2811_get_return_t_str(ret));
		free(_ws2811);
		return false;
	}

	setGamma(gamma);
	return true;
}

void LightStrip::render()
{
#ifndef _GUI_
	ws2811_render(_ws2811);
#endif
}

int LightStrip::exec()
{
	return 0;
}

int LightStrip::ledCount() const
{
	// return LED_COUNT1 + LED_COUNT2 ;
	int ledcount = 0;
	for (const LightStripSegment* segment : _segments) {
		ledcount += segment->ledCount();
	}
	return ledcount;
}

int LightStrip::segmentCount() const
{
	return _segments.size();
}

void LightStrip::setGamma(double gamma)
{
	_gamma = gamma;
#ifndef _GUI_
	ws2811_set_custom_gamma_factor(_ws2811, gamma);
#endif
}

double LightStrip::gamma() const
{
	return _gamma;
}

LightStripSegment *LightStrip::segment(int idx) const
{
	assert(idx>=0 && idx < segmentCount());
	return _segments.at(idx);
}

void LightStrip::setColor(int idx, int chan, uint8_t r, uint8_t g, uint8_t  b)
{
	_ws2811->channel[chan].leds[idx] = ((r & 0x0ff) << 16) | ((g & 0x0ff) << 8) | (b & 0x0ff);
}

void LightStrip::setColor(int idx, uint8_t r, uint8_t g, uint8_t  b)
{
	for (LightStripSegment* segment : _segments) {
		if (idx < segment->ledCount()) {
			segment->setColor(idx, r, g, b);
			return;
		}
		idx -= segment->ledCount();
	}
}

void LightStrip::setColor(uint8_t r, uint8_t g, uint8_t  b)
{
	for (int i=0; i < ledCount(); i++) {
		setColor(i, r, g, b);
	}
}

void LightStrip::clear()
{
	// fprintf(stderr,"clear(%d,%d)\n", _ws2811->channel[0].count, _ws2811->channel[1].count);
	memset(_ws2811->channel[0].leds, 0, sizeof(ws2811_led_t) * _ws2811->channel[0].count);
	memset(_ws2811->channel[1].leds, 0, sizeof(ws2811_led_t) * _ws2811->channel[1].count);
}

void LightStrip::setRainbowColor(int cnt)
{
	uint8_t r, g, b;
	int c = ledCount();
	for (int i = 0; i < c; i++) {
		uint8_t pos = (255*i / (c-1) + cnt);
		calcRainbowColor(pos, r, g, b);
		setColor(i, r, g, b);
	}
}

void LightStrip::calcRainbowColor(int pos, uint8_t& r, uint8_t& g, uint8_t& b)
{
	pos = pos % 256;
	if (pos<85) {
		r = pos*3;
		g = 255-pos*3;
		b = 0;
	} else if (pos<170) {
		pos -= 85;
		r = 255-pos*3;
		g = 0;
		b = pos*3;
	} else {
		pos -= 170;
		r = 0;
		g = pos*3;
		b = 255-pos*3;
	}
}




LightStripSegment::LightStripSegment(int firstLed, int lastLed, int channel, int bri) : 
	_firstLed(firstLed), _lastLed(lastLed), _channel(channel), _bri(bri),_strip(nullptr)
{
}

int LightStripSegment::ledCount() const
{
	return 1+abs(_lastLed - _firstLed);
}

int LightStripSegment::firstLed() const
{
	return _firstLed;
}

int LightStripSegment::lastLed() const
{
	return _lastLed;
}

int LightStripSegment::channel() const
{
	return _channel;
}

void LightStripSegment::setStrip(LightStrip* strip)
{
	_strip = strip;
}

void LightStripSegment::setColor(int idx, uint8_t r, uint8_t g, uint8_t b)
{
	// fprintf(stderr, "seg::setCol(%d, '%02x%02x%02x')\n", idx, r,g,b);
	assert(idx >=0 && idx < ledCount());

	int i = _firstLed < _lastLed ? _firstLed + idx : _firstLed - idx;
	_strip->setColor(i, _channel, _bri*r / 255, _bri*g / 255, _bri*b / 255);
//	_strip->setColor(i, _channel, r,g,b);
}

void LightStripSegment::setColor(uint8_t r, uint8_t g, uint8_t b)
{
	// fprintf(stderr, "seg::setCol('%02x%02x%02x')\n", r,g,b);
	for (int i=0; i < ledCount(); i++) {
		setColor(i, r, g, b);
	}
}

void LightStripSegment::clear()
{
	// fprintf(stderr, "seg::clear()\n");
	for (int i=0; i < ledCount(); i++) {
		setColor(i, 0, 0, 0);
	}
}

void LightStripSegment::setRainbowColor(int cnt)
{
	uint8_t r, g, b;
	int c = ledCount();
	for (int i = 0; i < c; i++) {
		uint8_t pos = (255*i / (c-1) + cnt);
		LightStrip::calcRainbowColor(pos, r, g, b);
		setColor(i, r, g, b);
	}
}
