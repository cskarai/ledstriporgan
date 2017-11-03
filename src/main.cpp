#include <stdio.h>
#include <stdlib.h>
#include <stm32f10x.h>

#include "NeoPixel.h"
#include "AudioSampler.h"
#include "AudioAnalyzer.h"
#include "Bulbs.h"
#include "randomseed.h"
#include "UartDebug.h"

int main(int, char**)
{
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_2 );

	srand(create_random_seed());

#if defined(DEBUG)
	uartDebug.init();
#endif
	neoPixel.init();
	bulbs.init();
	audioSampler.init();

	while(true)
	{
		if( audioSampler.hasData() ) {
			audioAnalyzer.processData(audioSampler.getData(), audioSampler.getDataCount());
			audioSampler.releaseData();
			bulbs.newFrame(audioAnalyzer);
#if defined(DEBUG)
			uartDebug.sendDebugInfo();
#endif
		}
	}

	return 0;
}

