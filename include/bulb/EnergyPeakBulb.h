/*
 * EnergyPeakBulb.h
 */

#ifndef BULB_ENERGYPEAKBULB_H_
#define BULB_ENERGYPEAKBULB_H_

#include "AbstractBulb.h"

#define  ENERGY_HISTORY_WINDOW  20

class EnergyPeakBulb : public AbstractBulb
{
private:
	uint32_t         energyHistory[ENERGY_HISTORY_WINDOW];
	uint32_t         energyHistoryPtr = 0;

	uint32_t         energyMin = 0;

	uint32_t         peakMax = 0;
	uint32_t         peakEnergy = 0;

	uint32_t         variantBackgroundColor = 0;
	uint32_t         variantWhiteColor = 0;

	void             addToHistory(uint32_t energy);


public:
	EnergyPeakBulb();

	virtual void     processNewFrame(AudioAnalyzer & analyzer);
	virtual uint32_t getColor(uint32_t variant);
};



#endif /* BULB_ENERGYPEAKBULB_H_ */
