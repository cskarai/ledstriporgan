/*
 * AudioSampler.cpp
 */

#include "AudioSampler.h"

#define AUDIO_SAMPLING_RATE 44100

AudioSampler audioSampler;

extern "C"
{

void DMA1_Channel1_IRQHandler()
{
	uint32_t isr = DMA1->ISR;
	if( isr & (DMA1_IT_TC1 | DMA1_IT_HT1) )
	{
		DMA_ClearITPendingBit(DMA1_IT_GL1 | DMA1_IT_TC1 | DMA1_IT_HT1);

		audioSampler.handleIrq(isr);
	}
}

}


void AudioSampler::init()
{
	configurePins();
	configureTimer();
	configureADC();
	configureDMA();
	configureInterrupts();

	startSampling();
}

void AudioSampler::configurePins() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	// configure channel 0
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	// configure channel 1
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void AudioSampler::configureADC() {
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);

	// Configure ADC1
	ADC_DeInit(ADC1);
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_ExternalTrigConvCmd(ADC1, ENABLE);

	// ADC1 regular channels configuration
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);

	// Enable ADC1 DMA
	ADC_DMACmd(ADC1, ENABLE);

	// Configure ADC2
	ADC_DeInit(ADC2);
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_Init(ADC2, &ADC_InitStructure);

	ADC_ExternalTrigConvCmd(ADC2, ENABLE);

	// ADC2 regular channels configuration
	ADC_RegularChannelConfig(ADC2, ADC_Channel_1, 1, ADC_SampleTime_1Cycles5);

	ADC_TempSensorVrefintCmd(ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	calibrateADC(ADC1);

	ADC_Cmd(ADC2, ENABLE);
	calibrateADC(ADC2);

	ADC_Cmd(ADC1, DISABLE);
	ADC_Cmd(ADC2, DISABLE);
}

void AudioSampler::calibrateADC(ADC_TypeDef * ADC ) {
	//Enable ADC reset calibration register
	ADC_ResetCalibration(ADC);
	//Check the end of ADC reset calibration register
	while(ADC_GetResetCalibrationStatus(ADC));
	//Start ADC calibration
	ADC_StartCalibration(ADC);
	//Check the end of ADC calibration
	while(ADC_GetCalibrationStatus(ADC));
}

void AudioSampler::configureTimer() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);

	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq (&RCC_Clocks);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	/* Time Base configuration */
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = RCC_Clocks.PCLK2_Frequency / AUDIO_SAMPLING_RATE;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM1, ENABLE);

	/* TIM1 counter disable */
	TIM_Cmd(TIM1, DISABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

void AudioSampler::configureInterrupts() {
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//Preemption Priority
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //Sub Priority
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_Init(&NVIC_InitStructure);
}

void AudioSampler::configureDMA() {
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_InitTypeDef     DMA_InitStructure;

	DMA_DeInit(DMA1_Channel1);

	// Configure DMA for ADC1
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)sampleBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_BufferSize = SAMPLE_BUFFER_SIZE;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	/* Enable DMA Channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);

	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_HT, ENABLE);
}

void AudioSampler::startSampling() {
	ADC_Cmd(ADC1, ENABLE);
	ADC_Cmd(ADC2, ENABLE);

	TIM1->CNT = 0;
	TIM_Cmd(TIM1, ENABLE);
}

void AudioSampler::handleIrq(uint32_t state) {
#if defined(DEBUG)
	if( hasData() ) {
		//asm volatile ("bkpt 0");
	}
#endif

	if( state & DMA1_IT_TC1 )
		dataAvailable = sampleBuffer + SAMPLE_COUNT;
	else
		dataAvailable = sampleBuffer;
}
