/*
 * AudioAnalyzer.h
 */

#ifndef AUDIOANALYZER_H_
#define AUDIOANALYZER_H_

#include <inttypes.h>
#include "AudioSampler.h"

#define WINDOW_SIZE           50
#define BASS_WINDOW_SIZE      15
#define MAX_BANDS              6

class AudioAnalyzer
{
private:
	uint16_t      samples[SAMPLE_COUNT];
	uint32_t      bassWindow[BASS_WINDOW_SIZE];
	uint32_t      bassWindowPtr = 0;

	uint32_t      leftEnergy = 0;
	uint32_t      rightEnergy = 0;

	uint32_t      leftAvg = 0;
	uint32_t      rightAvg = 0;
	uint32_t      avg = 0;

	uint32_t      bassStrength = 0;

	uint32_t      energy = 0;

	uint32_t      avgEnergy = 0;
	uint32_t      div = 0;

	uint32_t      frameNumber = 0;
	bool          refreshRequired = false;

	uint32_t      fftFrame = 0;

	uint16_t      fftBandMax[MAX_BANDS];
	uint16_t      fftBand[MAX_BANDS];

public:
	AudioAnalyzer();

	void     processData(uint32_t * data, uint32_t count);

	uint32_t getLeftEnergy()  { return leftEnergy; }
	uint32_t getRightEnergy() { return rightEnergy; }
	uint32_t getEnergy()      { return energy; }
	uint32_t getAvgEnergy()   { return avgEnergy / div; }
	uint32_t getAvg()         { return avg; }

	uint32_t getFrameNumber() { return frameNumber; }

	bool     isRefreshRequired() { return refreshRequired; }
	void     setRefreshRequired() { refreshRequired = true; }

	void     computeFFT();

	uint16_t getFFTBandMax(int band) { return fftBandMax[band]; }
	uint16_t getFFTBand(int band) { return fftBand[band]; }

	uint32_t getBassStrength() { return bassStrength; }

	uint16_t * getSamples() { return samples; }
};

extern AudioAnalyzer audioAnalyzer;

#endif /* AUDIOANALYZER_H_ */
