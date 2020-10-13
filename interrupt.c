/************************************************************************************
 * Created: 29/10/2015
 * Edited: Slim Limem
 *
 * If you have any question, send an email to slimlimem@gmail.com
 ************************************************************************************/

#include "main.h"

GPIO_InitTypeDef GPIO_InitStructure;
TIM_TimeBaseInitTypeDef TIM_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

int x = 1;

void TIM2_IRQHandler(void) {
    if (x) {
        pos(20);
        x=0;
    } else {
        pos(50);
        x=1;
    }
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
}

void interrupt_init(void) {

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

    TIM_InitStructure.TIM_Prescaler         = 42000 - 1;
    TIM_InitStructure.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_InitStructure.TIM_Period            = 2000 - 1;
    TIM_InitStructure.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_InitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2,&TIM_InitStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2,ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel      = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd   = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
