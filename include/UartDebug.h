/*
 * UartDebug.h
 */

#ifndef UARTDEBUG_H_
#define UARTDEBUG_H_

#include <inttypes.h>

#define FLAG_BEAT_STATE_MASK 0x07
#define FLAG_BEAT            0x80

typedef struct {
	uint32_t magic;
	uint16_t avg;
	uint8_t  frame;
	uint8_t  flags;
	uint16_t bassStrength;
	uint16_t samples[32];
} UartPacket;

class UartDebug {
private:
	UartPacket uartPacket;
	bool firstTransfer = true;

	void configurePin();
	void configureUart();
	void configureDMA();

public:
	void init();
	void sendDebugInfo();
};

extern UartDebug uartDebug;

#endif /* UARTDEBUG_H_ */
