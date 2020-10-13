/************************************************************************************
 * Created: 29/10/2015
 * Edited: Slim Limem
 *
 * If you have any question, send an email to slimlimem@gmail.com
 ************************************************************************************/

#include "gpio.h"

void init_GPIO(void) {
	GPIO_InitTypeDef GPIO_Config;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	// OUTPUT
	GPIO_Config.GPIO_Pin 	= GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12;
	GPIO_Config.GPIO_Mode 	= GPIO_Mode_OUT;
	GPIO_Config.GPIO_OType 	= GPIO_OType_PP;
	GPIO_Config.GPIO_Speed 	= GPIO_Speed_25MHz;
	GPIO_Config.GPIO_PuPd 	= GPIO_PuPd_UP;
		GPIO_Init(GPIOD, &GPIO_Config);
}


