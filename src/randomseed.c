#include "stm32f10x.h"

uint32_t create_random_seed(void) {
	//enable ADC1 clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	// Initialize ADC 14MHz RC
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	ADC_DeInit(ADC1);
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	//enable internal channel
	ADC_TempSensorVrefintCmd(ENABLE);

	// Enable ADCperipheral
	ADC_Cmd(ADC1, ENABLE);

	//Convert the ADC1 temperature sensor, user shortest sample time to generate most noise
	ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 1, ADC_SampleTime_1Cycles5);

	// Enable CRC clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);

	CRC_ResetDR();

	uint8_t i;
	for (i = 0; i < 8; i++) {
		//Start ADC1 Software Conversion
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		//wait for conversion complete
		while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)) {
		}

		CRC_CalcCRC(ADC_GetConversionValue(ADC1));
		//clear EOC flag
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	}

	//disable ADC1 to save power
	ADC_Cmd(ADC1, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);

	return CRC_CalcCRC(0xBADABADA);
}
