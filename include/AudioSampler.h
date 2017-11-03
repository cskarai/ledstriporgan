/*
 * AudioSampler.h
 */

#ifndef AUDIOSAMPLER_H_
#define AUDIOSAMPLER_H_

#include <stm32f10x.h>

#define SAMPLE_COUNT       128
#define SAMPLE_BUFFER_SIZE (SAMPLE_COUNT*2)

extern "C"
{
void DMA1_Channel1_IRQHandler();
};

class AudioSampler
{
	friend void DMA1_Channel1_IRQHandler();
	uint32_t * volatile dataAvailable;

private:
	uint32_t      sampleBuffer[SAMPLE_BUFFER_SIZE];

	void          configurePins();
	void          configureADC();
	void          configureDMA();
	void          configureInterrupts();
	void          configureTimer();

	void          calibrateADC(ADC_TypeDef * ADC );
	void          startSampling();

	void          handleIrq(uint32_t state);
public:
	void          init();

	bool          hasData() { return dataAvailable != 0; }
	uint32_t *    getData() { return dataAvailable; }
	uint32_t      getDataCount() { return SAMPLE_COUNT; }
	void          releaseData() { dataAvailable = 0; }
};

extern AudioSampler audioSampler;

#endif /* AUDIOSAMPLER_H_ */
