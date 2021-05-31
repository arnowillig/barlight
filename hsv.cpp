#include "hsv.h"

RgbColor hsvToRgb(unsigned char h, unsigned char s, unsigned char v)
{
	RgbColor rgb;
	unsigned char region, remainder, p, q, t;

	if (s == 0) {
		rgb.r = v;
		rgb.g = v;
		rgb.b = v;
		return rgb;
	}

	region = h / 43;
	remainder = (h - (region * 43)) * 6;

	p = (v * (255 - s)) >> 8;
	q = (v * (255 - ((s * remainder) >> 8))) >> 8;
	t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

	switch (region)
	{
	case 0:
		rgb.r = v; rgb.g = t; rgb.b = p;
		break;
	case 1:
		rgb.r = q; rgb.g = v; rgb.b = p;
		break;
	case 2:
		rgb.r = p; rgb.g = v; rgb.b = t;
		break;
	case 3:
		rgb.r = p; rgb.g = q; rgb.b = v;
		break;
	case 4:
		rgb.r = t; rgb.g = p; rgb.b = v;
		break;
	default:
		rgb.r = v; rgb.g = p; rgb.b = q;
		break;
	}

	return rgb;
}

HsvColor rgbToHsv(RgbColor rgb)
{
	HsvColor hsv;
	unsigned char rgbMin, rgbMax;

	rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
	rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);

	hsv.v = rgbMax;
	if (hsv.v == 0)
	{
		hsv.h = 0;
		hsv.s = 0;
		return hsv;
	}

	hsv.s = 255 * long(rgbMax - rgbMin) / hsv.v;
	if (hsv.s == 0)
	{
		hsv.h = 0;
		return hsv;
	}

	if (rgbMax == rgb.r)
		hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
	else if (rgbMax == rgb.g)
		hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
	else
		hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);

	return hsv;
}

Color::Color() : r(0), g(0), b(0)
{
}

Color::Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b)
{
}

Color::Color(const Color &other) : r(other.r), g(other.g), b(other.b)
{
}

Color Color::fromHSV(uint8_t h, uint8_t s, uint8_t v)
{
	if (s == 0) {
		return Color(v,v,v);
	}

	uint8_t region = h / 43;
	uint8_t remainder = (h - (region * 43)) * 6;

	uint8_t p = (v * (255 - s)) >> 8;
	uint8_t q = (v * (255 - ((s * remainder) >> 8))) >> 8;
	uint8_t t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

	switch (region) {
	case 0:
		return Color(v, t, p);
	case 1:
		return Color(q, v, p);
	case 2:
		return Color(p, v, t);
	case 3:
		return Color(p, q, v);
	case 4:
		return Color(t, p, v);
	default:
		return Color(v, p, q);
	}
}
