/*
 * AbstractBulb.h
 *
 *  Created on: 2017. okt. 22.
 *      Author: ckarai
 */

#ifndef BULB_ABSTRACTBULB_H_
#define BULB_ABSTRACTBULB_H_

#include "AudioAnalyzer.h"

#define COLOR_WHITE  0xFFFFFF
#define COLOR_BLUE   0x0000FF

class AbstractBulb
{
public:
	virtual void     processNewFrame(AudioAnalyzer & analyzer) = 0;
	virtual uint32_t getColor(uint32_t variant) = 0;

	virtual ~AbstractBulb() {};

	uint32_t toColor(uint32_t r, uint32_t g, uint32_t b);
	uint32_t lighten(uint32_t color, uint32_t intensity);
	uint32_t scale(uint32_t color, uint32_t intensity);
	uint32_t multiplyDiv16(uint32_t color, uint32_t mul);
};


#endif /* BULB_ABSTRACTBULB_H_ */
