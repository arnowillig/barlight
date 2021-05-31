#ifndef _HSV_H_
#define _HSV_H_

#include <stdint.h>

typedef struct RgbColor
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
} RgbColor;

typedef struct HsvColor
{
	unsigned char h;
	unsigned char s;
	unsigned char v;
} HsvColor;

RgbColor hsvToRgb(unsigned char h, unsigned char s, unsigned char v);
HsvColor rgbToHsv(RgbColor rgb);

class Color
{
public:
	Color();
	Color(uint8_t r, uint8_t g, uint8_t b);
	Color(const Color& other);
static Color fromHSV(uint8_t h, uint8_t s, uint8_t v);
	uint32_t toInt() const;
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

#endif
