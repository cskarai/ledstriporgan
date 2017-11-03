/*
 * AbstractBulb.cpp
 */

#include "AbstractBulb.h"

uint32_t AbstractBulb::toColor(uint32_t r, uint32_t g, uint32_t b)
{
	if( r > 255 )
		r = 255;

	if( g > 255 )
		g = 255;

	if( b > 255 )
		b = 255;

	return ((uint32_t)r << 8 ) | ((uint16_t)g << 16 ) | b;
}

uint32_t AbstractBulb::lighten(uint32_t color, uint32_t intensity)
{
	if( intensity > 255 )
		intensity = 255;

	uint32_t g = 255 - ((color >> 16) & 255);
	uint32_t r = 255 - ((color >> 8) & 255);
	uint32_t b = 255 - (color & 255);

	r = r * intensity / 255;
	g = g * intensity / 255;
	b = b * intensity / 255;

	return toColor(255-r,255-g,255-b);
}

uint32_t AbstractBulb::scale(uint32_t color, uint32_t intensity)
{
	if( intensity > 255 )
		intensity = 255;

	uint32_t g = (color >> 16) & 255;
	uint32_t r = (color >> 8) & 255;
	uint32_t b = color & 255;

	r = r * intensity / 255;
	g = g * intensity / 255;
	b = b * intensity / 255;

	return toColor(r,g,b);
}

uint32_t AbstractBulb::multiplyDiv16(uint32_t color, uint32_t mul)
{
	uint32_t g = (color >> 16) & 255;
	uint32_t r = (color >> 8) & 255;
	uint32_t b = color & 255;

	uint16_t r2 = (uint16_t)r * mul / 16;
	uint16_t g2 = (uint16_t)g * mul / 16;
	uint16_t b2 = (uint16_t)b * mul / 16;

	if( r2 > 255 )
		r2 = 255;
	if( g2 > 255 )
		g2 = 255;
	if( b2 > 255 )
		b2 = 255;

	return toColor((uint8_t)r2,(uint8_t)g2,(uint8_t)b2);
}

