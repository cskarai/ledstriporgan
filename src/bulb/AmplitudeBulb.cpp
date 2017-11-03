/*
 * AmplitudeBulb.cpp
 */

#include "AmplitudeBulb.h"
#include "BackgroundBulb.h"

#define ENERGY_LIMIT     12
#define ENERGY_SCALE     12

void AmplitudeBulb::processNewFrame(AudioAnalyzer & analyzer)
{
	uint32_t energy = analyzer.getEnergy() * 2 / ENERGY_SCALE;
	if( energy < ENERGY_LIMIT )
		energy = 0;
	else
		energy -= ENERGY_LIMIT;

	variantWhiteColor = scale( COLOR_WHITE, energy );
	variantBackgroundColor = scale( BackgroundBulb::getColor(), energy );
}

uint32_t AmplitudeBulb::getColor(uint32_t variant)
{
	if( variant & 16 ) {
		return variantWhiteColor;
	} else {
		return variantBackgroundColor;
	}
}
