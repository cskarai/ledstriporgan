/*
 * BassPeakBulb.cpp
 */

#include "BassPeakBulb.h"

#define PEAK_DECREASE     3

#define ENERGY_LIMIT    400
#define AVG_ENERGY_LIMIT 18

void BassPeakBulb::processNewFrame(AudioAnalyzer & analyzer)
{
	if( peakMax < PEAK_DECREASE )
		peakMax = 0;
	else
		peakMax -= PEAK_DECREASE;

	peakEnergy = peakEnergy * 8 / 9;

	uint32_t energy = analyzer.getBassEnergy();
	if( (energy > ENERGY_LIMIT) && ( energy > peakMax ) ) {
		peakEnergy = 255;
		peakMax = energy;
		analyzer.setRefreshRequired();
	}

	color = scale( COLOR_BLUE, peakEnergy );
}

uint32_t BassPeakBulb::getColor(uint32_t variant)
{
	return color;
}
