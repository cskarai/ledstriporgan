/*
 * AudioAnalyzer.cpp
 */

#include "AudioAnalyzer.h"
#include "isqrt.h"
#include "fix_fft.h"
#include <string.h>

#if BASS_FRAME_SIZE < 128 || BASS_FRAME_SIZE > 255
#error BASS_FRAME_SIZE must be between 128 and 255
#endif

#define BASS_REMNANT_SIZE (256 - BASS_FRAME_SIZE)

AudioAnalyzer audioAnalyzer;


#define FFT_N               7
#define FFT_SIZE ( 1 << FFT_N)

#define MAX_FREQUENCIES 64

uint16_t bandLimits[] = { 20, 15, 15, 15, 5, 5 };
uint8_t  bandScale[] = { 10, 10, 21, 25, 40, 40 };

AudioAnalyzer::AudioAnalyzer() {
	memset(fftBandMax, 0, MAX_BANDS * sizeof(uint16_t));

	for(int i=0; i < BASS_WINDOW_SIZE; i++)
		bassWindow[i] = 0x800;
}

void AudioAnalyzer::processData(uint32_t * data, uint32_t count)
{
	leftAvg = rightAvg = 0;

	bassCurve = bassRemnant;
	bassRemnant = 0;

	for(uint32_t i=0; i < count; i++) {

		uint32_t left = data[i] >> 16;
		uint32_t right = data[i] & 0xFFFF;

		leftAvg += left;
		rightAvg += right;

		uint32_t mid = (left + right)>>1;
		samples[i] = mid;

		bassCurve += mid;
		if( i >= BASS_REMNANT_SIZE )
			bassRemnant += mid;
	}

	leftAvg /= count;
	rightAvg /= count;
	bassCurve /= BASS_FRAME_SIZE;

	leftEnergy = rightEnergy = 0;

	for(uint32_t i=0; i < count; i++) {
		int32_t left = data[i] >> 16;
		int32_t right = data[i] & 0xFFFF;
		left -= leftAvg;
		right -= rightAvg;

		leftEnergy += left * left;
		rightEnergy += right * right;
	}
	leftEnergy /= count;
	rightEnergy /= count;

	leftEnergy = isqrt(leftEnergy);
	rightEnergy = isqrt(rightEnergy);
	energy = (leftEnergy + rightEnergy) / 2;

	frameNumber++;

	avg = (leftAvg + rightAvg) / 2;
	bassWindow[bassWindowPtr++] = bassCurve;
	if( bassWindowPtr == BASS_WINDOW_SIZE)
		bassWindowPtr = 0;

	refreshRequired = (frameNumber & 7) == 0;

	avgEnergy = avgEnergy  * (WINDOW_SIZE - 1) / WINDOW_SIZE + (uint32_t)WINDOW_SIZE * energy;
	div = div * (WINDOW_SIZE - 1) / WINDOW_SIZE + WINDOW_SIZE;

	uint32_t bassAvg = 0;
	for(int i=0; i < BASS_WINDOW_SIZE; i++ ) {
		bassAvg += bassWindow[i];
	}
	bassAvg /= BASS_WINDOW_SIZE;

	bassEnergy = 0;
	for(int i=0; i < BASS_WINDOW_SIZE; i++) {
		int diff = bassWindow[i] - bassAvg;
		bassEnergy += diff * diff;
	}
	bassEnergy = isqrt(bassEnergy);
}

void AudioAnalyzer::computeFFT()
{
	if( fftFrame == frameNumber )
		return;

	if(( frameNumber & 7) != 6 )
		return;

	fftFrame = frameNumber;

	fixed fft_real[FFT_SIZE];
	fixed fft_imag[FFT_SIZE];

	for(int i=0; i < SAMPLE_COUNT; i++)
		fft_real[i] = samples[i] - avg;
	memset(fft_imag, 0, sizeof(fixed) * FFT_SIZE);

	fix_fft(fft_real, fft_imag, FFT_N, 0);

	for(int i=0; i < FFT_SIZE/2; i++) {
		fft_real[i] = isqrt(fft_real[i]*fft_real[i] + fft_imag[i]*fft_imag[i]);
	}

	int mask = 1;
	int p = 1;

	for(uint8_t i=0; i < MAX_BANDS; i++, mask <<= 1)
	{
		uint16_t max = 0;
		for(uint8_t j=0; j < mask; j++)
		{
			if( fft_real[p] > max )
				max = fft_real[p];
			p++;
			if( p >= MAX_FREQUENCIES - 1 )
				break;
	    }
		fftBandMax[i] = max;

		if( max < bandLimits[i] )
			max = 0;
		else
			max -= bandLimits[i];

		max = (uint16_t)((uint32_t)max * bandScale[i] / 16);
		fftBand[i] = max;
	}
}

