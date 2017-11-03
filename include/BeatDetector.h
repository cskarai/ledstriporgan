/*
 * BeatDetector.h
 *
 *  Created on: 2017. okt. 23.
 *      Author: ckarai
 */

#ifndef BEATDETECTOR_H_
#define BEATDETECTOR_H_

#include <inttypes.h>
#include "AudioAnalyzer.h"

#define HISTORY_WINDOW_SIZE 128

typedef enum  {
  NO_BEAT=0,
  IN_BEAT=1,
  WAIT_FALL=2,
  BEAT_DISABLE=3,
} BeatState;

class BeatDetector
{
  private:
    int32_t   magnitude = 0;
    int32_t   lastMagnitude = 0;
    int32_t   beatStartMagnitude = 0;
    int32_t   beatRiseMagnitude = 0;
    uint32_t  beatDisableStart = 0;
    uint32_t  beatLength = 0;
    BeatState beatState = NO_BEAT;
    uint32_t  historyStrength[HISTORY_WINDOW_SIZE];
    uint32_t  historyPtr = 0;
    uint32_t  historyAvg = 0;
    uint32_t  historyMin = 0;
    uint32_t  waitCnt = 0;

    bool      beat = false;

    void      addToHistory(uint32_t strength);

  public:
    BeatDetector();

    bool      hasBeat(AudioAnalyzer & analyzer);
    uint32_t  getBeatState() { return beatState; }
    bool      isBeat() { return beat; }
};

extern BeatDetector beatDetector;

#endif /* BEATDETECTOR_H_ */
