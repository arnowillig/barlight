#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "barlight.h"
#include "barlight.h"
#include "rpi_ws281x/ws2811.h"

#define LED_COUNT1		30
#define LED_COUNT2		30

#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN1               18
#define GPIO_PIN2               13
#define WS2811_DMA              10
#define MAX_BRIGHTNESS		255



BarLightStrip::BarLightStrip() : _ws2811(nullptr), _gamma(0.0)
{
}

BarLightStrip::~BarLightStrip()
{
	clear();
#ifndef _GUI_
	ws2811_render(_ws2811);
	ws2811_wait(_ws2811);
#endif
}

bool BarLightStrip::init(double gamma)
{
	_ws2811 = (ws2811_t *) malloc(sizeof(ws2811_t));
	memset(_ws2811, 0, sizeof(ws2811_t));

	_ws2811->freq			= WS2811_TARGET_FREQ;
	_ws2811->dmanum			= WS2811_DMA;

	_ws2811->channel[0].gpionum	= GPIO_PIN1;
	_ws2811->channel[0].count	= LED_COUNT1;
	_ws2811->channel[0].strip_type	= WS2812_STRIP;
	_ws2811->channel[0].brightness	= MAX_BRIGHTNESS;

	_ws2811->channel[1].gpionum	= GPIO_PIN2;
	_ws2811->channel[1].count	= LED_COUNT2;
	_ws2811->channel[1].strip_type	= WS2812_STRIP;
	_ws2811->channel[1].brightness	= MAX_BRIGHTNESS;

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

void BarLightStrip::render()
{
#ifndef _GUI_
	ws2811_render(_ws2811);
#endif
}


int BarLightStrip::exec()
{
	return 0;
}

int BarLightStrip::ledCount() const
{
	return LED_COUNT1 + LED_COUNT2 ;
}


void BarLightStrip::setGamma(double gamma)
{
	_gamma = gamma;
#ifndef _GUI_
	ws2811_set_custom_gamma_factor(_ws2811, gamma);
#endif
}

double BarLightStrip::gamma() const
{
	return _gamma;
}

void BarLightStrip::setColor(int idx, uint8_t r, uint8_t g, uint8_t  b)
{
	int chanIdx, ledIdx;
	mapIndex(idx, chanIdx, ledIdx);

	_ws2811->channel[chanIdx].leds[ledIdx] = ((r & 0x0ff) << 16) | ((g & 0x0ff) << 8) | (b & 0x0ff);
}

void BarLightStrip::setColor(uint8_t r, uint8_t g, uint8_t  b)
{
	for (int i=0; i < ledCount(); i++) {
		setColor(i, r, g, b);
	}
}

void BarLightStrip::clear()
{
	setColor(0, 0, 0);
}

void BarLightStrip::setRainbowColor(int cnt)
{
	uint8_t r, g, b;
	int c = ledCount();
	for (int i = 0; i < c; i++) {
		uint8_t pos = (255*i / (c-1) + cnt);
		calcRainbowColor(pos, r, g, b);
		setColor(i, r, g, b);
	}
}

void BarLightStrip::mapIndex(int idx, int& ledIdx, int& chanIdx)
{
	if (idx < LED_COUNT1) {
		chanIdx = 0;
		ledIdx  = LED_COUNT1 - idx - 1;
	} else {
		chanIdx = 0;
		ledIdx  = idx - LED_COUNT1;
	}
}

void BarLightStrip::calcRainbowColor(int pos, uint8_t& r, uint8_t& g, uint8_t& b)
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
