/*
 * UartDebug.cpp
 */

#include "UartDebug.h"
#include "AudioAnalyzer.h"
#include "BeatDetector.h"
#include <stm32f10x.h>
#include <string.h>

UartDebug uartDebug;

#define UART_MAGIC      0xC50EDAB1
#define DEBUG_BAUD_RATE    2000000

void UartDebug::configurePin()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	// configure GPIO
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void UartDebug::configureUart()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	USART_InitTypeDef USART_InitStruct;

	USART_InitStruct.USART_BaudRate = DEBUG_BAUD_RATE;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx;
	USART_DeInit(USART3);
	USART_Init(USART3, &USART_InitStruct);

	USART_Cmd(USART3, ENABLE);

	firstTransfer = true;
}

void UartDebug::configureDMA()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_InitTypeDef  DMA_InitStructure;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;

	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					// automatically increase buffer index

	DMA_InitStructure.DMA_BufferSize = sizeof(UartPacket);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART3->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &uartPacket;		// this is the buffer memory

	DMA_Init(DMA1_Channel2, &DMA_InitStructure);
}

void UartDebug::init()
{
	configurePin();
	configureUart();
	configureDMA();
}

void UartDebug::sendDebugInfo()
{
	if( (! firstTransfer) && ((DMA_GetFlagStatus(DMA1_FLAG_TC2)) == RESET) )
		return; // DMA is too slow
	firstTransfer = false;

	DMA_ClearFlag(DMA1_FLAG_TC2);

	DMA_Cmd(DMA1_Channel2, DISABLE);

	memset(&uartPacket, 0, sizeof(UartPacket));

	uartPacket.magic = UART_MAGIC;
	uartPacket.frame = (uint8_t)audioAnalyzer.getFrameNumber();
	uartPacket.avg = (uint16_t)audioAnalyzer.getAvg();
	uartPacket.bassStrength = (uint16_t)audioAnalyzer.getBassStrength();
	uartPacket.flags = beatDetector.getBeatState() & FLAG_BEAT_STATE_MASK;
	uartPacket.bassCurve = (uint16_t)audioAnalyzer.getBassCurve();

	if( beatDetector.isBeat() )
		uartPacket.flags |= FLAG_BEAT;

	uint16_t * sampleBuf = audioAnalyzer.getSamples();
	uint16_t * writeBuf = uartPacket.samples;

	uint32_t avg = 0;
	int cnt = 0;
	for( int i=0; i < 128; i++ ) {
		avg += sampleBuf[i];
		if( ++cnt == 4 ) {
			*writeBuf++ = (uint16_t)avg;
			cnt = avg = 0;
		}
	}


	DMA1_Channel2->CNDTR = sizeof(UartPacket);
	DMA1_Channel2->CMAR = (uint32_t) &uartPacket;

	// enable DMA
	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA1_Channel2, ENABLE);
}
