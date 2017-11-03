/*
 * BackgroundBulb.cpp
 *
 *  Created on: 2017. okt. 22.
 *      Author: ckarai
 */

#include "BackgroundBulb.h"
#include "BeatDetector.h"
#include "Bulbs.h"
#include <stdlib.h>

#define COLOR_BCKGND_1 0xFF0000 // green
#define COLOR_BCKGND_2 0x0000FF // blue
#define COLOR_BCKGND_3 0xFFFF00 // yellow
#define COLOR_BCKGND_4 0x00FFFF // purple
#define COLOR_BCKGND_5 0xFF00FF // cyan
#define COLOR_BCKGND_6 0x20FF50 // pink

uint32_t BackgroundBulb::color = COLOR_BCKGND_1;

void BackgroundBulb::processNewFrame(AudioAnalyzer & analyzer)
{
	if( analyzer.getFrameNumber() == 1 )
		newColor();

	if( beatDetector.hasBeat(analyzer) ) {
		newColor();
		analyzer.setRefreshRequired();

		beatCount++;
		if(( beatCount % 6 ) == 0 )
			bulbs.newBulb();
	}
}

void BackgroundBulb::newColor()
{
	uint32_t oldColor = color;

	while(oldColor == color)
	{
		switch( rand() % 6 )
		{
		case 0:
			color = COLOR_BCKGND_1;
			break;
		case 1:
			color = COLOR_BCKGND_2;
			break;
		case 2:
			color = COLOR_BCKGND_3;
			break;
		case 3:
			color = COLOR_BCKGND_4;
			break;
		case 4:
			color = COLOR_BCKGND_5;
			break;
		case 5:
			color = COLOR_BCKGND_6;
			break;
		}
	}
}
