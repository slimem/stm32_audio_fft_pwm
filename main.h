/************************************************************************************
 * Created: 29/10/2015
 * Edited: Slim Limem
 *
 * If you have any question, send an email to slimlimem@gmail.com
 ************************************************************************************/

#define HSE_VALUE ((uint32_t)8000000) /* STM32 discovery uses a 8Mhz external crystal */
// FFT defines
#define MAX_FFT_HEIGHT 128
#define FFT_SIZE 1024
#define ARM_MATH_CM4

#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include "arm_math.h"
#include "time.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"
#include "usb_dcd_int.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_adc.h"
#include "misc.h"

#define ARM_MATH_CM4
#define RGB_MIN 0
#define RGB_MAX 100
#define RGB_ERROR 1


void ColorfulRingOfDeath(void);
void SysTick_Handler(void);
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void OTG_FS_IRQHandler(void);
void OTG_FS_WKUP_IRQHandler(void);

void interrupt_init(void);
//void TIM2_IRQHandler(void);
void TIM4_IRQHandler(void);