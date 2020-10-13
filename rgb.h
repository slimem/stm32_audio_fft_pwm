/************************************************************************************
 * Created: 29/10/2015
 * Edited: Slim Limem
 *
 * This file contains the RGB colors defines and funtion prototypes for
 * setting individual colors  
 *
 * If you have any question, send an email to slimlimem@gmail.com
 ************************************************************************************/

#include "main.h"

//USUAL COLORS
#define RED 0xFF0000
#define YELLOW 0xFFFF00
#define GREEN 0xFF00
#define CYAN 0xFFFF
#define BLUE 0xFF
#define MAGENTA 0xFF00FF
#define WHITE 0xFFFFFF
#define BLACK 0x00
#define MIDGRAY 0x7F7F7F

void RGB_GPIO_config(void);
void RGB_Enable(void);
void setRGB1(uint8_t R,uint8_t G,uint8_t B);
void setRGB2(uint8_t R,uint8_t G,uint8_t B);
void setRGB3(uint8_t R,uint8_t G,uint8_t B);
void setRGBx(uint32_t col);
void setHardness(uint16_t mod,uint32_t ini,uint32_t fin,uint8_t fac);