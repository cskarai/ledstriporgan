/*
 * RGBBulb.h
 */

#ifndef BULB_RGB_H_
#define BULB_RGB_H_

#include "AbstractBulb.h"
#include "AudioAnalyzer.h"

class RGBBulb : public AbstractBulb
{
private:
	uint32_t         rgbColorV1 = 0;
	uint32_t         rgbColorV2 = 0;

	uint32_t         frequencies[MAX_BANDS];

public:
	RGBBulb();

	virtual void     processNewFrame(AudioAnalyzer & analyzer);
	virtual uint32_t getColor(uint32_t variant);
};



#endif /* BULB_RGB_H_ */
