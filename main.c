/************************************************************************************
 * Created: 29/10/2015
 * Edited: Slim Limem
 *
 * If you have any question, send an email to slimlimem@gmail.com
 ************************************************************************************/

#include "main.h"
#define Dcy GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_10)
#define FDC GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11)
#define RAZ GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_12)

volatile uint32_t ticker, downTicker;

/*
 * FFT RELATED VARIABLES DECLARATIONS
 */
static float32_t fftOutput[FFT_SIZE];
uint32_t 	maxfft, indexfft; // Full spectrum
uint32_t 	indexfftLow = 0; // Low frequencies spectrum
uint16_t 	lowMag;
float32_t 	maxfftlow;
uint16_t 	indexfftlow;
uint32_t 	maxfftMid, indexfftMid; // MID frequencies spectrum
uint32_t 	maxfftHigh, indexfftHigh; // HIGH frequencies spectrum

arm_cfft_radix4_instance_f32 instP; //instance pointer
float32_t 	maxValue; //Maximum value (test)
float32_t 	fftout[FFT_SIZE/2];
char 		fftout_str[129]; 	//String to be sent via USB
fftout_str[128] ='\0';
float32_t 	sample[FFT_SIZE*2]; //Sample ready for fft (real and im values)
uint16_t 	n = 0;//Sample offset
uint8_t 	j; //fft string conversion counter

float32_t 	moy1 = 0; // Mean bass value
float32_t 	moy2 = 0; // Mean mid value
float32_t 	moy3 = 0; // Mean high value

/*
 * ADC RELATED VARIABLES DECLARATIONS
 */
float32_t audio[4];

/*
 * USB RELATED VARIABLES DECLARATIONS
 */
uint16_t sendC=0; // Sending counter
char conv[100] = {65}; // String to be sent
conv[25]='\0';
uint8_t getCh; //getchar

/*
 * COLOR COMPUTATION VARIABLES
 */
float32_t co1 = 0;
float32_t co2 = 0;
float32_t co3 = 0;
uint16_t t1 = 0; //time offset
uint16_t t2 = 0;
uint16_t t3 = 0;
float32_t amp1 = 0;
float32_t amp2 = 0;
float32_t amp3 = 0;
uint16_t x = 0, y = 0, z = 0; // RGB values counter

/*
 * The USB data must be 4 byte aligned if DMA is enabled. This macro handles
 * the alignment, if necessary (it's actually magic, but don't tell anyone).
 */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;

void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}

void init();
void ColorfulRingOfDeath(void);


/*
 * Define prototypes for interrupt handlers here. The conditional "extern"
 * ensures the weak declarations from startup_stm32f4xx.c are overridden.
 */
#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

extern uint8_t APP_Rx_Buffer[];

int main(void)
{	
	// Set up the system clocks
	SystemInit();

	// Initialize USB, IO, SysTick, and all those other things you do in the morning
	init();
	init_GPIO();
	RGB_GPIO_config();
	RGB_Enable();
	adc_configure();

	interrupt_init();
	TIM2_IRQHandler();

	// set default colors
	setRGB1(50,20,80);
	setRGB2(20,50,30);
	setRGB3(80,20,50);

	while (1) {}
	
	return 0;
}

void init() {
	// Setup SysTick or ColorfulRingOfDeath
	if (SysTick_Config(SystemCoreClock / 1000)) {
		ColorfulRingOfDeath();
	}

	// Setup USB
	USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_CDC_cb, &USR_cb);
}

/*
 * Call this to indicate a failure.  Blinks the STM32F4 discovery LEDs
 * in sequence.  At 168Mhz, the blinking will be very fast - about 5 Hz.
 * Keep that in mind when debugging, knowing the clock speed might help
 * with debugging.
 */
void ColorfulRingOfDeath(void)
{
	uint16_t ring = 1;
	while (1) {
		uint32_t count = 0;
		while (count++ < 500000);

		GPIOD->BSRRH = (ring << 12);
		ring = ring << 1;
		if (ring >= 1<<4) {
			ring = 1;
		}
		GPIOD->BSRRL = (ring << 12);
	}
}

/*
 * Interrupt Handlers
 */

void SysTick_Handler(void) {
	ticker++;
	if (downTicker > 0) {
		downTicker--;
	}
}

void NMI_Handler(void)       {}
void HardFault_Handler(void) { ColorfulRingOfDeath(); }
void MemManage_Handler(void) { ColorfulRingOfDeath(); }
void BusFault_Handler(void)  { ColorfulRingOfDeath(); }
void UsageFault_Handler(void){ ColorfulRingOfDeath(); }
void SVC_Handler(void)       {}
void DebugMon_Handler(void)  {}
void PendSV_Handler(void)    {}

void OTG_FS_IRQHandler(void) {
  USBD_OTG_ISR_Handler (&USB_OTG_dev);
}

void OTG_FS_WKUP_IRQHandler(void) {
	if(USB_OTG_dev.cfg.low_power) {
	  *(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ;
	  SystemInit();
	  USB_OTG_UngateClock(&USB_OTG_dev);
	}

  	EXTI_ClearITPendingBit(EXTI_Line18);
}

/*
 * This interruption is called for each adc conversion
 * (44100 times per second)
 * Change this frequency according to the audio sampling
 * rate, to get accurate fft frequencies
 */

void TIM12_IRQHandler(void) {
	// Toggle led
	GPIO_ToggleBits(GPIOD,GPIO_Pin_14);
	
	audio[0] = ADC_GetConversionValue(ADC1);

	// Remember: FFT expects a data buffer in this format:
	// real[0],im[1],real[2],im[3]... and so on and so forth

	// normalize audio level
	sample[n] 	= (float32_t)((float32_t)audio[0] - (float32_t)2048.0) / (float32_t)2048.0; // real val
	sample[n+1] = 0.f; // Remember, im val always zero

	// we fill the audio buffer with two values at a time
	n += 2;
	
	// if we fill our buffer by FFT_SIZE*2 values 
	if (n >= FFT_SIZE*2) {
		arm_cfft_radix4_init_f32(&instP, FFT_SIZE, 0, 1);
		arm_cfft_radix4_f32(&instP, sample);
		arm_cmplx_mag_f32(sample, fftOutput, FFT_SIZE);
		arm_max_f32(fftOutput, FFT_SIZE, &maxValue, &testIndex);
		n = 0; // reset audio buffer counter
		// toggle LED for each successful FFT operation
		GPIO_ToggleBits(GPIOD,GPIO_Pin_15);
		// These are our interesting frequency values. These values were selected after many tests.
		moy1 =((fftOutput[1] + fftOutput[2] + fftOutput[3]) / 3); // Low freq
		moy2 =((fftOutput[7] + fftOutput[10] + fftOutput[14] + fftOutput[17] + fftOutput[19] + fftOutput[21] + fftOutput[23]) / 7); // Mid Freq
		moy3 =((fftOutput[46] + fftOutput[69] + fftOutput[100] + fftOutput[116] + fftOutput[162] + fftOutput[167]) / 6); // High Freq
	}

	// Sending data through USB is only useful while debugging. If you dont use it, remove the USB_OTG once and for all
	
	// Send string via usb
	sendC++;
	if (sendC >= 441) { //Almost 100hz 441
		sendC=0;
		// Convert FFT values to a logarithmic scale
		sprintf(conv, "%05u%05u%05u%05u", (uint16_t)audio[0], (uint16_t) moy1, (uint16_t)(co1*10), (uint16_t)((-log(co1/100))*100));

		// String is only sent when we read a character. To continuously send data, remove this section
		if (VCP_get_char(&getCh)) {
			VCP_send_str(conv);
			GPIO_ToggleBits(GPIOD,GPIO_Pin_13);
		}
	}

	TIM_ClearITPendingBit(TIM12,TIM_IT_Update);
}

/*
 * This interruption is called for every time color needs to be updated.
 * Here RGB colors are computed for every led strip according to the FFT
 * mean value (low, mid and high). Fast color transitions are eased out
 * with an exponent 100*exp(-t/100)
 */
void TIM2_IRQHandler(void) {
	// TODO: MERGE EVERYTHING INTO ONE FUNCTION

	// FFT computed value for Low freq
	amp1 =  moy1;                         // LED STRIP 1
	GPIO_ToggleBits(GPIOD,GPIO_Pin_12);
	co1 = 100*exp((float32_t)-t1/100); 	// ease the color change with exponential curve
	t1 += 4;

	// saturate value
	if(t1 >= 500) t1 = 500;
	if(amp1 >= 100) t1 = 0;
	else if(amp1 > co1) t1 = (uint16_t)(-log(amp1/100)*100);
	
	// change LED strip 1 colors using sin
	setRGB1(1+(uint16_t)(co1*((arm_sin_f32(x1*2*M_PI/250)+1)/2)),1+(uint16_t)(co1*((arm_sin_f32(y1*2*M_PI/400)+1)/2)),1+(uint16_t)(co1*((arm_sin_f32(z1*2*M_PI/150)+1)/2)));

	// FFT computed value for mid freq
	amp2 =  moy2;                         // LED STRIP 2
	//GPIO_ToggleBits(GPIOD,GPIO_Pin_12);
	co2 = 100*exp((float32_t)-t2/100); // ease the color change with exponential curve
	t2+=4;

	// saturate value
	if(t2 >= 500) t2 = 500;

	if(amp2 >= 100) t2 = 0;
	else if(amp2 > co2) t2 = (uint16_t)(-log(amp2/100)*100);
	
	// change LED strip 2 colors using sin
	setRGB2(1+(uint16_t)(co2*((arm_sin_f32(x*2*M_PI/250)+1)/2)),1+(uint16_t)(co2*((arm_sin_f32(y*2*M_PI/400)+1)/2)),1+(uint16_t)(co2*((arm_sin_f32(z*2*M_PI/150)+1)/2)));


	// FFT computed value for high freq
	amp3 =  moy3;                         // LED STRIP 3
	//GPIO_ToggleBits(GPIOD,GPIO_Pin_12);
	co3 = 100*exp((float32_t)-t3/100); // ease the color change with exponential curve
	t3 += 4;

	// saturate value
	if(t3>=500)t3=500;
	if(amp3>= 100) t3=0;
	else if(amp3>co3) t3 = (uint16_t)(-log(amp3/100)*100);
	
	// change LED strip 3 colors using sin
	setRGB3(1+(uint16_t)(co3*((arm_sin_f32(x*2*M_PI/250)+1)/2)),1+(uint16_t)(co3*((arm_sin_f32(y*2*M_PI/400)+1)/2)),1+(uint16_t)(co3*((arm_sin_f32(z*2*M_PI/150)+1)/2)));

	// update RGB color counters
	x++;y++;z++;
	if(x == 250) x = 0;
	if(y == 400) y = 0;
	if(z == 150) z = 0;

	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
}


void interrupt_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12,ENABLE); //Audio, must be at 44.1Khz

    TIM_InitStructure.TIM_Prescaler = 1680 - 1;//42000
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_InitStructure.TIM_Period = 1000 - 1;
    TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2,&TIM_InitStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2,ENABLE);

    TIM_InitStructure.TIM_Prescaler = 635 - 1;//42000
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_InitStructure.TIM_Period = 3 - 1;
    TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM12,&TIM_InitStructure);
    TIM_ITConfig(TIM12, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM12,ENABLE);


    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = TIM12_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


