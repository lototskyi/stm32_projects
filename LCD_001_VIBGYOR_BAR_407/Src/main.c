/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#include "bsp_lcd.h"


#define RGB888(r,g,b)  (((r) << 16) | ((g) << 8) | (b))

#define VIOLET   	RGB888(148,0,211)
#define INDIGO   	RGB888(75,0,130)
#define BLUE   		RGB888(0,0,255)
#define GREEN   	RGB888(0,255,0)
#define YELLOW   	RGB888(255,255,0)
#define ORANGE   	RGB888(255,127,0)
#define RED   		RGB888(255,0,0)
#define WHITE   	RGB888(255,255,255)
#define BLACK		RGB888(0,0,0)

const uint32_t SystemCoreClock = 168000000UL;


void SystemClock_Setup(void);


int main(void)
{

	SystemClock_Setup();
	bsp_lcd_init();
#if 1
	bsp_lcd_set_background_color(YELLOW);
	bsp_lcd_set_display_area(60, 259, 100, 139);
	bsp_lcd_send_cmd_mem_write();
	uint16_t data[200UL * 40UL];
	for(uint32_t i = 0 ; i < (200UL * 40UL) ; i++){
		data[i] = bsp_lcd_convert_rgb888_to_rgb565(RED);
	}
#if USE_DMA == 0
	bsp_lcd_write((uint8_t*)data, (200UL * 40UL * 2UL));
#else
	bsp_lcd_write_dma((uint32_t) data, (200UL * 40UL * 2UL));
#endif
#endif

#if 0
	uint32_t x_start,x_width,y_start,y_height;
	bsp_lcd_set_background_color(BLACK);
#if(BSP_LCD_ORIENTATION == LANDSCAPE)
	x_start = 0;
	y_start = 0;
	x_width = 320;
	y_height = 34;
#elif(BSP_LCD_ORIENTATION == PORTRAIT)
	x_start = 0;
	y_start = 0;
	x_width = 240;
	y_height = 45;
#endif
	bsp_lcd_fill_rect(VIOLET, x_start, x_width, y_height*0, y_height);
	bsp_lcd_fill_rect(INDIGO, x_start, x_width, y_height*1, y_height);
	bsp_lcd_fill_rect(BLUE,   x_start, x_width, y_height*2, y_height);
	bsp_lcd_fill_rect(GREEN,  x_start, x_width, y_height*3, y_height);
	bsp_lcd_fill_rect(YELLOW, x_start, x_width, y_height*4, y_height);
	bsp_lcd_fill_rect(ORANGE, x_start, x_width, y_height*5, y_height);
	bsp_lcd_fill_rect(RED, 	  x_start, x_width, y_height*6, y_height);
	bsp_lcd_fill_rect(YELLOW, x_start, x_width, y_height*4, y_height);
#endif
    /* Loop forever */
	for(;;);
}


void SystemClock_Setup(void)
{
	RCC_TypeDef *pRCC = RCC;
	FLASH_TypeDef *pFlash = FLASH;
	PWR_TypeDef *pPWR = PWR;

	//1. Program flash wait states
	REG_SET_VAL(pFlash->ACR,0x5U,0xFU,FLASH_ACR_LATENCY_Pos);

	//2. Over drive settings
	REG_SET_BIT(pRCC->APB1ENR,RCC_APB1ENR_PWREN_Pos); 	/*Enable clock for PWR register access*/
	REG_SET_BIT(pPWR->CR,PWR_CR_VOS_Pos); 				/*VOS = 1*/

	//3. Setting up main PLL
	REG_SET_VAL(pRCC->PLLCFGR,0x8U,0x3FU,RCC_PLLCFGR_PLLM_Pos); /*PLL_M*/
	REG_SET_VAL(pRCC->PLLCFGR,168U,0x1FFU,RCC_PLLCFGR_PLLN_Pos); /*PLL_N*/
	REG_SET_VAL(pRCC->PLLCFGR,0x00U,0x3U,RCC_PLLCFGR_PLLP_Pos); /*PLL_P*/

	//5. Setting up AHB and APBx clocks
	REG_SET_VAL(pRCC->CFGR,0U,0xFU,RCC_CFGR_HPRE_Pos); /*AHB prescaler*/
	REG_SET_VAL(pRCC->CFGR,0x5U,0x7U,RCC_CFGR_PPRE1_Pos); /*APB1 prescaler*/
	REG_SET_VAL(pRCC->CFGR,0x4U,0x7U,RCC_CFGR_PPRE2_Pos); /*APB2 prescaler*/

	//6. Turn on PLL and wait for PLLCLK ready
	REG_SET_BIT(pRCC->CR,RCC_CR_PLLON_Pos);
	while(!REG_READ_BIT(pRCC->CR,RCC_CR_PLLRDY_Pos));

	//7. Switch PLLCLK as SYSCLK
	REG_SET_VAL(pRCC->CFGR,0x2U,0x3U,RCC_CFGR_SW_Pos);
	while(!(REG_READ_VAL(pRCC->CFGR,0x3U,RCC_CFGR_SWS_Pos) == 0x2U));

}




