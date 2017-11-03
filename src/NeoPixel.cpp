/*
 * NeoPixel.cpp
 */

#include "NeoPixel.h"

#include <malloc.h>
#include <string.h>
#include <stm32f10x.h>

NeoPixel neoPixel;

#define BIT_0            8
#define BIT_1            21
#define TOTAL_WIDTH      (BIT_0 + BIT_1)

void NeoPixel::init() {
	configurePin();
	configureTimer();
	configureDMA();
	configureInterrupts();
}

void NeoPixel::configurePin() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	// configure GPIO
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void NeoPixel::configureTimer() {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	// prescaler to 24 MHz
	TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) (SystemCoreClock / 24000000) - 1;
	TIM_TimeBaseStructure.TIM_Period = TOTAL_WIDTH; // 800kHz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);

	// configure PWM
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);

	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

	TIM_SetCompare1(TIM4, 0); // don't send anything at beginning

	TIM_DMACmd(TIM4, TIM_DMA_Update, ENABLE);
	TIM_DMAConfig(TIM4, TIM_DMABase_CCR1, TIM_DMABurstLength_1Byte );
}

void NeoPixel::configureDMA() {
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_InitTypeDef  DMA_InitStructure;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;			// data shifted from memory to peripheral
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;				// circular mode
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;

	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					// automatically increase buffer index

	DMA_InitStructure.DMA_BufferSize = DMA_BUFFER_SIZE;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(TIM4->DMAR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) dmaBuffer;		// this is the buffer memory

	DMA_Init(DMA1_Channel7, &DMA_InitStructure);

	DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
	DMA_ITConfig(DMA1_Channel7, DMA_IT_HT, ENABLE);
	DMA_ITConfig(DMA1_Channel7, DMA_IT_TE, ENABLE);
}

void NeoPixel::configureInterrupts() {
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // priority
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // subpriority
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
	NVIC_Init(&NVIC_InitStructure);
}

extern "C"
{
void DMA1_Channel7_IRQHandler()
{
	// half buffer?
	if (DMA_GetITStatus(DMA1_IT_HT7)) {
		neoPixel.fillDMABuffer(true);
	}
	// transfer complete
	if (DMA_GetITStatus(DMA1_IT_TC7)) {
		neoPixel.fillDMABuffer(false);
	}
	// transfer error
	if (DMA_GetITStatus(DMA1_IT_TE7)) {
		neoPixel.stopDMA();
	}
	DMA_ClearITPendingBit(DMA1_IT_GL7 | DMA1_IT_TE7 | DMA1_IT_TC7 | DMA1_IT_HT7);

}
}

void NeoPixel::fillDMABuffer(bool isHalf) {
	int buf = isHalf ? 0 : PIXEL_BITS;

	if( sendPtr < 0 ) {
		memset(dmaBuffer + buf, 0, PIXEL_BITS);

		if( sendPtr < -3 )
			stopDMA();
	} else {
		uint32_t byte = pixelBuffer[sendPtr];

		uint32_t mask = 1 << (PIXEL_BITS - 1);
		for(int i=0; i < PIXEL_BITS; i++) {
			dmaBuffer[buf++] = (mask & byte) ? BIT_1 : BIT_0;
			mask >>= 1;
		}
	}
	sendPtr--;
}

void NeoPixel::stopDMA() {
	DMA_Cmd(DMA1_Channel7, DISABLE);
	TIM_Cmd(TIM4, DISABLE);

	sending = false;
}

bool NeoPixel::send() {
	if( sending )
		return false;

	sending = true;
	sendPtr = NEOPIXEL_LEDS - 1;

	fillDMABuffer(true);
	fillDMABuffer(false);

	// reset buffers
	DMA1_Channel7->CNDTR = DMA_BUFFER_SIZE;
	DMA1_Channel7->CMAR = (u32)dmaBuffer;

	// enable DMA
	DMA_Cmd(DMA1_Channel7, ENABLE);
	// start Timer
	TIM4->CNT = 0;
	TIM_Cmd(TIM4, ENABLE);
	return true;
}

