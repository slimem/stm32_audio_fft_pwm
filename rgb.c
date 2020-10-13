/************************************************************************************
 * Created: 29/10/2015
 * Edited: Slim Limem
 *
 * This file contains all the timer and GPIO declaration needed to use PWM properly.
 * We need to light up 3 RGB leds so we used 3 channels from TIM3.
 *
 * TIM3 runs initially at 84Mhz and every PWM channel runs at 10Khz. To edit the
 * output signal (the light intensity) we just have to edit the pulse value from 1 to
 * 100. Why? Because it's easier that way.
 *
 * If you have any question, send an email to slimlimem@gmail.com
 ************************************************************************************/

#include "rgb.h"


TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
TIM_OCInitTypeDef 			TIM_OCInitStructure;
GPIO_InitTypeDef 			GPIO_InitStructure;

uint16_t color[3];
float32_t fac; //magnitude factor

void RGB_GPIO_config(void) {                             // à revoir
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// TIM3 clock enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	// TIM4 clock enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	// GPIOC and GPIOB clock enable
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	// GPIOB Configuration: TIM3 et TIM4 CH1
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_0 | GPIO_Pin_1
									| GPIO_Pin_4 | GPIO_Pin_5
									| GPIO_Pin_6 | GPIO_Pin_7
									| GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//GPIOC Configuration: TIM3 CH1
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//Alternate functions
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_TIM3);	// CH1 R1
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);	// CH3 G1
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM3);	// CH4 B1

	// Alternate functions
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_TIM3);	// CH2 R2
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);	// CH1 G2
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_TIM4);	// CH2 B2

	// Alternate functions
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_TIM4);	// CH3 R3
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_TIM4);	// CH4 G3
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM3);	// CH3 B3
}

void RGB_Enable(void) {
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	// Color will be set by editing the pulse width value between 1 and 100
	
	// Time base configuration 10khz
	TIM_TimeBaseStructure.TIM_Period 		= 101-1; // <-- HERE :)
	TIM_TimeBaseStructure.TIM_Prescaler 	= 85-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode 	= TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode 			= TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState 	= TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity 		= TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse 			= 100; // Full color intensitiy initially

	/**
	LED STRIP 1
	*/
	// R1 PWM Config 							
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

	// G1 PWM Config
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

	// B1 PWM Config
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	/**
	LED STRIP 2
	*/
	// R2 PWM Config
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

	// G2 PWM Config
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

	// B2 PWM Config
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

	/**
	LED STRIP 3
	*/

	// R3 PWM Config
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

	// G3 PWM Config
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

	// B3 PWM Config
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

	// Enable ARRPreload register
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	
	//TIM3 and TIM4 enable counter
	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
}

//TODO: Merge these 3 funtions into one

/**
Set R, G and B values individually for LED STRIP 1
*/
void setRGB1(uint8_t R, uint8_t G, uint8_t B) {   // les couleurs sont selectionnés séparément

	// We saturate out of bound values
	if (R >  RGB_MAX) R = RGB_MAX;
	if (G >  RGB_MAX) G = RGB_MAX;
	if (B >  RGB_MAX) B = RGB_MAX;
	if (R <= RGB_MIN) R = RGB_MIN;
	if (G <= RGB_MIN) G = RGB_MIN;
	if (B <= RGB_MIN) B = RGB_MIN;

	TIM_OCInitStructure.TIM_Pulse = R; // I dont know what im doing here
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_Pulse = G;
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_Pulse = B;
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
}

/**
Set R, G and B values individually for LED STRIP 2
*/
void setRGB2(uint8_t R,uint8_t G,uint8_t B) {

	// We saturate out of bound values
	if (R >  RGB_MAX) R = RGB_MAX;
	if (G >  RGB_MAX) G = RGB_MAX;
	if (B >  RGB_MAX) B = RGB_MAX;
	if (R <= RGB_MIN) R = RGB_MIN;
	if (G <= RGB_MIN) G = RGB_MIN;
	if (B <= RGB_MIN) B = RGB_MIN;

	TIM_OCInitStructure.TIM_Pulse = R;
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_Pulse = G;
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_Pulse = B;
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
}

/**
Set R, G and B values individually for LED STRIP 3
*/
void setRGB3(uint8_t R,uint8_t G,uint8_t B){

	// We saturate out of bound values
	if (R >  RGB_MAX) R = RGB_MAX;
	if (G >  RGB_MAX) G = RGB_MAX;
	if (B >  RGB_MAX) B = RGB_MAX;
	if (R <= RGB_MIN) R = RGB_MIN;
	if (G <= RGB_MIN) G = RGB_MIN;
	if (B <= RGB_MIN) B = RGB_MIN;

	TIM_OCInitStructure.TIM_Pulse = R;
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_Pulse = G;
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_Pulse = B;
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
}

/**
Set RGB color fpr  individually for LED STRIP 1
*/
void setRGBx(uint32_t col){

	TIM_OCInitStructure.TIM_Pulse =(uint16_t) (((col & 0xFF0000)>>16)/0xFF)*100; // TODO: Use a define
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_Pulse =(uint16_t) (((col & 0xFF00)>>8)/0xFF)*100;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_Pulse =(uint16_t) ((col & 0xFF)/0xFF)*100;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
}

void setHardnessx(uint16_t mod, uint16_t max, uint32_t ini){
	
	//Initial color
	color[0] = (((ini & 0xFF0000)>>16)/0xFF) * 100; 	//Initial Red
	color[1] = (((ini & 0xFF00)>>8)/0xFF) * 100; 		//Initial Green
    color[2] = ((ini & 0xFF)/0xFF) * 100;				//Initial Blue

    fac = mod / max; 									// calculate magnitude factor
    
    color[0] = (uint16_t)(100-color[0])*fac+color[0]; 	// calculate new color R
    color[1] = (uint16_t)(100-color[1])*fac+color[1]; 	// calculate new color G
    color[2] = (uint16_t)(100-color[2])*fac+color[2]; 	// calculate new color B

    if (color[0] > 100) color[0] = 100; 				// recalculate new color R
    if (color[1] > 100) color[1] = 100; 				// recalculate new color G
    if (color[2] > 100) color[2] = 100; 				// recalculate new color B
    setRGB(color[0], color[1], color[3]); 				// Update colors
}