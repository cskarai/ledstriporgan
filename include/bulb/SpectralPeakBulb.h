/*
 * SpectralPeakBulb.h
 */

#ifndef BULB_SPECTRALPEAK_H_
#define BULB_SPECTRALPEAK_H_

#include "AbstractBulb.h"
#include "AudioAnalyzer.h"

class SpectralPeakBulb : public AbstractBulb
{
private:
	uint32_t         frequenciesMax[MAX_BANDS];
	uint32_t         frequenciesAvg[MAX_BANDS];
	uint32_t         rgbEnergy[MAX_BANDS];
	uint32_t         frequenciesDiv = 0;
	uint32_t         rgbColor[MAX_BANDS];

public:
	SpectralPeakBulb();

	virtual void     processNewFrame(AudioAnalyzer & analyzer);
	virtual uint32_t getColor(uint32_t variant);
};



#endif /* BULB_SPECTRALPEAK_H_ */
