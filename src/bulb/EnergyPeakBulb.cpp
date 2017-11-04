/*
 * EnergyPeakBulb.cpp
 */

#include <EnergyPeakBulb.h>
#include "BackgroundBulb.h"

#define PEAK_DECREASE    3

#define ENERGY_LIMIT     30
#define AVG_ENERGY_LIMIT 40

EnergyPeakBulb::EnergyPeakBulb()
{
	for(int i=0; i < ENERGY_HISTORY_WINDOW; i++)
		energyHistory[i] = 0xFFFFF;
}

void EnergyPeakBulb::addToHistory(uint32_t energy)
{
	energyHistory[energyHistoryPtr] = energy;
	if( ++energyHistoryPtr == ENERGY_HISTORY_WINDOW )
		energyHistoryPtr = 0;

	energyMin = 0xFFFFF;

	for(int i=2; i < ENERGY_HISTORY_WINDOW-2; i++)
	{
		uint32_t sum = 0;

		for(int8_t d=-2; d <= 2; d++ )
		{
			uint8_t ndx = energyHistoryPtr + i + d;
			ndx = ndx % ENERGY_HISTORY_WINDOW;

			sum += energyHistory[ndx];
		}

		sum /= 5;

		if( sum < energyMin )
			energyMin = sum;
	}

	if( energyMin == 0 )
		energyMin++;
}

void EnergyPeakBulb::processNewFrame(AudioAnalyzer & analyzer)
{
	peakMax = (peakMax * 127) / 128;

	peakEnergy = peakEnergy * 6 / 7;

	uint32_t energy = analyzer.getEnergy();
	addToHistory(energy);

	uint32_t magnitude = 256 * energy / energyMin;

	if( (energy > ENERGY_LIMIT) && ( magnitude > peakMax ) ) {
		peakEnergy = magnitude / 5;
		if( peakEnergy > 255 )
			peakEnergy = 255;
		peakMax = magnitude;
	}

	variantWhiteColor = scale( COLOR_WHITE, peakEnergy );
	variantBackgroundColor = scale( BackgroundBulb::getColor(), peakEnergy );
}

uint32_t EnergyPeakBulb::getColor(uint32_t variant)
{
	if( variant & 16 ) {
		return variantWhiteColor;
	} else {
		return variantBackgroundColor;
	}
}
