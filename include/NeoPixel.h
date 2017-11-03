/*
 * NeoPixel.h
 *
 *  Created on: 2017. okt. 21.
 *      Author: ckarai
 */

#ifndef NEOPIXEL_H_
#define NEOPIXEL_H_

#include <inttypes.h>

#define NEOPIXEL_LEDS  30

#define PIXEL_BITS   24
#define DMA_BUFFER_SIZE (2*PIXEL_BITS)

extern "C"
{
void DMA1_Channel7_IRQHandler();
}

class NeoPixel
{
	friend void DMA1_Channel7_IRQHandler();

private:
	uint32_t   pixelBuffer[NEOPIXEL_LEDS];
	uint8_t    dmaBuffer[DMA_BUFFER_SIZE];

	int32_t    sendPtr = NEOPIXEL_LEDS - 1;
	bool       sending = false;

	void       configurePin();
	void       configureTimer();
	void       configureDMA();
	void       configureInterrupts();

	void       fillDMABuffer(bool isHalf);
	void       stopDMA();

public:
	void       init();

	uint32_t * getPixelBuffer() { return pixelBuffer; }

	bool       send();

	bool       isBusy() { return sending; }
};

extern NeoPixel neoPixel;

#endif /* NEOPIXEL_H_ */
