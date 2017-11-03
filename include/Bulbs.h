/*
 * Bulbs.h
 *
 *  Created on: 2017. okt. 22.
 *      Author: ckarai
 */

#ifndef BULBS_H_
#define BULBS_H_

#define BULB_HANDLERS        6

#include "AbstractBulb.h"

#include "NeoPixel.h"

struct BulbDesc
{
	uint8_t handler  : 3;
	uint8_t variant  : 5;
};


class Bulbs
{
private:
	AbstractBulb * bulbHandler[BULB_HANDLERS];

	BulbDesc       bulbs[NEOPIXEL_LEDS];
	uint32_t       bulbFirstPtr;

	BulbDesc       lastBulb;
	uint32_t       lastBulbRemaining;

public:
	void init();
	void newFrame(AudioAnalyzer &analyzer);

	void newBulb();
};

extern Bulbs bulbs;

#endif /* BULBS_H_ */
