/*
 * Bulbs.cpp
 */

#include "Bulbs.h"
#include "BackgroundBulb.h"
#include "AmplitudeBulb.h"
#include "AmplitudePeakBulb.h"
#include "RGBBulb.h"
#include "SpectralPeakBulb.h"
#include "BassPeakBulb.h"
#include <stdlib.h>

#define BULB_KIND_BACKGROUND        0
#define BULB_KIND_AMPLITUDE         1
#define BULB_KIND_AMPLITUDE_PEAK    2
#define BULB_KIND_RGB               3
#define BULB_KIND_SPECTRAL_PEAK     4
#define BULB_KIND_BASS_PEAK         5

Bulbs bulbs;

void Bulbs::init() {
	bulbHandler[BULB_KIND_BACKGROUND]      = new BackgroundBulb();
	bulbHandler[BULB_KIND_AMPLITUDE]       = new AmplitudeBulb();
	bulbHandler[BULB_KIND_AMPLITUDE_PEAK]  = new AmplitudePeakBulb();
	bulbHandler[BULB_KIND_RGB]             = new RGBBulb();
	bulbHandler[BULB_KIND_SPECTRAL_PEAK]   = new SpectralPeakBulb();
	bulbHandler[BULB_KIND_BASS_PEAK]       = new BassPeakBulb();

	bulbFirstPtr = lastBulbRemaining = 0;

	for(int i=0; i < NEOPIXEL_LEDS; i++)
		newBulb();
}

void Bulbs::newFrame(AudioAnalyzer &analyzer) {
	for(int i=0; i < BULB_HANDLERS; i++)
		bulbHandler[i]->processNewFrame(analyzer);

	if( analyzer.isRefreshRequired() && !neoPixel.isBusy() ) {
		for(int i=0; i < NEOPIXEL_LEDS; i++) {
			int ndx = bulbFirstPtr + i;
			if( ndx >= NEOPIXEL_LEDS )
				ndx -= NEOPIXEL_LEDS;

			BulbDesc bulb = bulbs[ndx];
			neoPixel.getPixelBuffer()[i] = bulbHandler[bulb.handler]->getColor(bulb.variant);
		}
		neoPixel.send();
	}
}

void Bulbs::newBulb() {
	if( lastBulbRemaining != 0 ) {
		if( lastBulbRemaining == 4 )
			bulbs[bulbFirstPtr] = {BULB_KIND_BACKGROUND, 0};
		else {
			bulbs[bulbFirstPtr] = lastBulb;
			lastBulb.variant++;
		}
		lastBulbRemaining--;
	} else {
		bulbs[bulbFirstPtr] = {BULB_KIND_BACKGROUND, 0};
		BulbDesc oldBulb = lastBulb;
		oldBulb.variant &= 0xF8; // remove location

		do
		{
			uint32_t rnd = rand() % 10;
			lastBulbRemaining = 3;

			switch( rnd )
			{
			case 0:
			case 8:
				lastBulb = {BULB_KIND_AMPLITUDE, 0};
				break;
			case 1:
				lastBulb = {BULB_KIND_AMPLITUDE, 16};
				break;
			case 2:
			case 9:
				lastBulb = {BULB_KIND_AMPLITUDE_PEAK, 0};
				break;
			case 3:
				lastBulb = {BULB_KIND_AMPLITUDE_PEAK, 16};
				break;
			case 4:
				lastBulb = {BULB_KIND_RGB, 0};
				break;
			case 5:
				lastBulb = {BULB_KIND_RGB, 16};
				break;
			case 6:
				lastBulb = {BULB_KIND_SPECTRAL_PEAK, 0};
				lastBulbRemaining = 7;
				break;
			case 7:
				lastBulb = {BULB_KIND_BASS_PEAK, 0};
				break;
			}

		}while( (oldBulb.handler == lastBulb.handler) && (oldBulb.variant == lastBulb.variant) );
	}

	if( ++bulbFirstPtr == NEOPIXEL_LEDS )
		bulbFirstPtr = 0;
}
