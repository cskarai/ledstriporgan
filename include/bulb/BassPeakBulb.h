/*
 * BassPeakBulb.h
 */

#ifndef BULB_BASSPEAKBULB_H_
#define BULB_BASSPEAKBULB_H_

#include "AbstractBulb.h"

class BassPeakBulb : public AbstractBulb
{
private:
	uint32_t         peakMax = 0;
	uint32_t         peakEnergy = 0;

	uint32_t         color = 0;

public:
	virtual void     processNewFrame(AudioAnalyzer & analyzer);
	virtual uint32_t getColor(uint32_t variant);
};



#endif /* BULB_BASSPEAKBULB_H_ */
