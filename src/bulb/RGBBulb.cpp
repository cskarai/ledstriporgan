/*
 * RGBBulb.cpp
 */

#include "RGBBulb.h"

#define RGB_MULTIPLIER_1              0x05
#define RGB_MULTIPLIER_2              0x07

RGBBulb::RGBBulb() {
	for(int i=0; i < MAX_BANDS; i++)
		frequencies[i] = 0;
}

void RGBBulb::processNewFrame(AudioAnalyzer & analyzer)
{
	analyzer.computeFFT();

	for(int i=0; i < MAX_BANDS; i++) {
		uint16_t fftBand = analyzer.getFFTBand(i);

	    frequencies[i] = ((uint16_t)frequencies[i]) * 9 / 10;

	    if( fftBand > frequencies[i] )
	      frequencies[i] = fftBand;
	}

	rgbColorV1 = multiplyDiv16(toColor(frequencies[4], frequencies[2], frequencies[1]), RGB_MULTIPLIER_1);
	rgbColorV2 = multiplyDiv16(toColor(frequencies[5], frequencies[3], frequencies[0]), RGB_MULTIPLIER_2);
}

uint32_t RGBBulb::getColor(uint32_t variant)
{
	if( variant & 16 ) {
		return rgbColorV1;
	} else {
		return rgbColorV2;
	}
}
