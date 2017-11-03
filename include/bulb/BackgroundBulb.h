/*
 * BackgroundBulb.h
 */

#ifndef BULB_BACKGROUNDBULB_H_
#define BULB_BACKGROUNDBULB_H_

#include "AbstractBulb.h"

class BackgroundBulb : public AbstractBulb {
private:
	static uint32_t  color;

	uint32_t         beatCount = 0;

public:
	virtual void     processNewFrame(AudioAnalyzer & analyzer);
	virtual uint32_t getColor(uint32_t /* variant */) { return color; }

	static uint32_t  getColor() { return color; }

	static void      newColor();
};

#endif /* BULB_BACKGROUNDBULB_H_ */
