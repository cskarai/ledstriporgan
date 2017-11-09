/*
 * BeatDetector.cpp
 */

#include "BeatDetector.h"

BeatDetector beatDetector;

#define BEAT_FALL_INCREASE      5
#define BEAT_MIN_DISTANCE      30
#define BEAT_MIN_STRENGTH     300
#define BEAT_MIN_OVERHEAD    0x1C
#define BEAT_HYSTERESIS       100
#define BEAT_MAX_RISE_TIME      8

BeatDetector::BeatDetector()
{
	for(int i=0; i < HISTORY_WINDOW_SIZE; i++)
		historyStrength[i] = 0xFFFFF;
}

void BeatDetector::addToHistory(uint32_t strength)
{
	historyStrength[historyPtr] = strength;
	if( ++historyPtr == HISTORY_WINDOW_SIZE )
		historyPtr = 0;

	historyMin = 0xFFFFF;

	for(int i=2; i < HISTORY_WINDOW_SIZE-2; i++)
	{
		uint32_t sum = 0;

		for(int8_t d=-2; d <= 2; d++ )
		{
			uint8_t ndx = historyPtr + i + d;
			ndx = ndx % HISTORY_WINDOW_SIZE;

			sum += historyStrength[ndx];
		}

		sum /= 5;

		if( sum < historyMin )
			historyMin = sum;
	}

	historyAvg = 0;
	for(int i=0; i < HISTORY_WINDOW_SIZE; i++)
		historyAvg += historyStrength[i];

	historyAvg /= HISTORY_WINDOW_SIZE;
}

bool BeatDetector::hasBeat(AudioAnalyzer & analyzer)
{
	addToHistory(analyzer.getBassStrength());

	lastMagnitude = magnitude;
	magnitude = analyzer.getBassStrength() - historyAvg;

	switch( beatState )
	{
	case NO_BEAT:
		if( (magnitude > 0) && ( magnitude > lastMagnitude ) && ( analyzer.getBassStrength() >=  BEAT_MIN_OVERHEAD * (uint32_t)historyMin / 16 ) )
		{
			if( magnitude < BEAT_MIN_STRENGTH )
				break;

			waitCnt = 0;
			beatState = IN_BEAT;
			beatStartMagnitude = beatRiseMagnitude = magnitude;
			break;
		}
		break;
	case IN_BEAT:
		if( magnitude < beatStartMagnitude - BEAT_HYSTERESIS )
		{
			beatState = NO_BEAT;
			break;
		}
		if( magnitude > beatRiseMagnitude )
			beatRiseMagnitude = magnitude;

		if( magnitude < beatRiseMagnitude - BEAT_HYSTERESIS)
		{
			beatState = WAIT_FALL;
			return beat = true;
		}
		if( ++waitCnt == BEAT_MAX_RISE_TIME )
		{
			beatState = WAIT_FALL;
			return beat = true;
		}
		break;
	case WAIT_FALL:
		if( magnitude < beatStartMagnitude )
		{
			beatDisableStart = analyzer.getFrameNumber();
			beatState = BEAT_DISABLE;
		}
		break;
    case BEAT_DISABLE:
    	if( ( analyzer.getFrameNumber() - beatDisableStart ) >= BEAT_MIN_DISTANCE )
    		beatState = NO_BEAT;
    	break;
	}

	return beat = false;
}

