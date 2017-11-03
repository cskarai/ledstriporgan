/*
 * SpectralPeakBulb.cpp
 */

#include "SpectralPeakBulb.h"

#define ENERGY_LIMIT     2
#define RGB_WINDOW_SIZE 50
#define RGB_LIMIT       18

SpectralPeakBulb::SpectralPeakBulb() {
	for(int i=0; i < MAX_BANDS; i++)
		frequenciesMax[i] = frequenciesAvg[i] = rgbEnergy[i] = rgbColor[i] = 0;
}

void SpectralPeakBulb::processNewFrame(AudioAnalyzer & analyzer)
{
	analyzer.computeFFT();

    frequenciesDiv = frequenciesDiv  * (RGB_WINDOW_SIZE - 1) / RGB_WINDOW_SIZE + (uint32_t)RGB_WINDOW_SIZE;
	for(int i=0; i < MAX_BANDS; i++) {
		uint16_t fftBand = analyzer.getFFTBand(i);

	    rgbEnergy[i] = ((uint16_t)rgbEnergy[i]) * 8 / 9;

	    if( frequenciesMax[i] > 1 )
	    	frequenciesMax[i]-= 1;
	    else
	    	frequenciesMax[i] = 0;

	    frequenciesAvg[i] = frequenciesAvg[i]  * (RGB_WINDOW_SIZE - 1) / RGB_WINDOW_SIZE + (uint32_t)RGB_WINDOW_SIZE * fftBand;
	    uint16_t avg = frequenciesAvg[i] / frequenciesDiv;

	    if(( fftBand > frequenciesMax[i] ) && (fftBand >= ENERGY_LIMIT))
	    {
	      if( avg * RGB_LIMIT / 16 < fftBand )
	      {
	    	  frequenciesMax[i] = fftBand;
	    	  rgbEnergy[i] = 255;
	      }
	    }

        uint32_t energy = rgbEnergy[i];

        uint32_t common = (uint16_t)energy * 2 / 9;
        energy -= 3 * common;
        uint32_t v1 = energy / 2;
        uint32_t v2 = energy - v1;

        uint32_t rf=0, gf=0, bf = 0;

        switch(i)
        {
        case 0:
          rf = energy;
          break;
        case 1:
          gf = energy;
          break;
        case 2:
          bf = energy;
          break;
        case 3:
          rf = v1;
          gf = v2;
          break;
        case 4:
          rf = v1;
          bf = v2;
          break;
        case 5:
          gf = v1;
          bf = v2;
          break;
        }

        rgbColor[i] = toColor(rf + common, gf + common, bf + common);
	}
}

uint32_t SpectralPeakBulb::getColor(uint32_t variant)
{
	return rgbColor[variant & 7];
}

