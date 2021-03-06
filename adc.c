/************************************************************************************
 * Created: 29/10/2015
 * Edited: Slim Limem
 *
 * If you have any question, send an email to slimlimem@gmail.com
 ************************************************************************************/

#include "adc.h"

int ConvertedValue = 0;

void adc_configure() {
	ADC_InitTypeDef ADC_init_structure;
	GPIO_InitTypeDef GPIO_initStructre;
	//Clock configuration
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE); 	// The ADC1 is connected the APB2 peripheral bus thus we will use its clock source
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOCEN,ENABLE); 	// Clock for the ADC port!! Do not forget about this one ;)

	//Analog pin configuration
	GPIO_initStructre.GPIO_Pin = GPIO_Pin_0; 				//The channel 10 is connected to GPIO PC0
	GPIO_initStructre.GPIO_Mode = GPIO_Mode_AN; 			//The PC0 pin is configured in analog mode
	GPIO_initStructre.GPIO_PuPd = GPIO_PuPd_NOPULL; 		//We don't need any pull up or pull down
	GPIO_Init(GPIOC,&GPIO_initStructre); 					//Affecting the port with the initialization structure configuration
	
	//ADC structure configuration
	ADC_DeInit();
	ADC_init_structure.ADC_DataAlign = ADC_DataAlign_Right;							// Data converted will be shifted to right
	ADC_init_structure.ADC_Resolution = ADC_Resolution_12b;							// Input voltage is converted into a 12bit number giving a maximum value of 4096
	ADC_init_structure.ADC_ContinuousConvMode = ENABLE; 							// The conversion is continuous, the input data is converted more than once
	ADC_init_structure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;			// Conversion is synchronous with TIM1 and CC1 (actually I'm not sure about this one :/)
	ADC_init_structure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;	// No trigger for conversion
	ADC_init_structure.ADC_NbrOfConversion = 1;										// I think this one is clear :p
	ADC_init_structure.ADC_ScanConvMode = DISABLE;									// The scan is configured in one channel
	ADC_Init(ADC1,&ADC_init_structure);												//Initialize ADC with the previous configuration
	
	// Enable ADC conversion
	ADC_Cmd(ADC1,ENABLE);
	
	// Select the channel to be read from
	ADC_RegularChannelConfig(ADC1,ADC_Channel_10,1,ADC_SampleTime_144Cycles);
	ADC_SoftwareStartConv(ADC1);
}

int adc_convert() {
	ADC_SoftwareStartConv(ADC1);					// Start the conversion
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));	// Processing the conversion
	return ADC_GetConversionValue(ADC1); 			// Return the converted data
}

