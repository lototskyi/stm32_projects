#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "stm32f4xx_hal.h"
#include "main_app.h"

void SystemClock_Config_HSE(uint8_t clock_freq);
void GPIO_Init(void);
void UART1_Init(void);

void Error_handler(void);

UART_HandleTypeDef huart1;

void printmsg(char *format,...) {
	char str[80];

	/*Extract the argument list using VA apis*/
	va_list args;
	va_start(args, format);
	vsprintf(str, format, args);
	HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
	va_end(args);
}

int main(void)
{
	uint32_t * pBackupSRAMBase;

	char write_buf[] = "Hello";

	HAL_Init();

	GPIO_Init();

	SystemClock_Config_HSE(SYS_CLOCK_FREQ_50_MHZ);

	UART1_Init();

	//1. Turn on the clock in RCC register for backup SRAM
	__HAL_RCC_BKPSRAM_CLK_ENABLE();

	//2. Enable Write access to the backup domain
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWR_EnableBkUpAccess();

	pBackupSRAMBase = (uint32_t*)BKPSRAM_BASE;

	if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET) {

		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

		printmsg("Woke up from the standby mode\r\n");
		uint8_t data = (uint8_t)*pBackupSRAMBase;

		if (data != 'H') {
			printmsg("Backup SRAM data lost\r\n");
		} else {
			printmsg("Backup SRAM data is safe\r\n");
		}

	} else {
		for (uint32_t i = 0; i < strlen(write_buf) + 1; i++) {
			*(pBackupSRAMBase + i) = write_buf[i];
		}
	}

	printmsg("Press the user button to enter the Standby mode\r\n");

	while (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_4) != GPIO_PIN_RESET);

	//when user pushes the user button, it comes here
	printmsg("Going to Standby mode\r\n");

	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

	//Enable backup voltage regulator
	HAL_PWREx_EnableBkUpReg();

	HAL_PWR_EnterSTANDBYMode();


	while (1) {

	}

	return 1;
}

void SystemClock_Config_HSE(uint8_t clock_freq)
{
	RCC_OscInitTypeDef Osc_Init = {0};
	RCC_ClkInitTypeDef Clock_Init = {0};
    uint8_t flash_latency=0;

	Osc_Init.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_HSI ;
	Osc_Init.HSEState = RCC_HSE_ON;
	Osc_Init.LSEState = RCC_LSE_OFF;
	Osc_Init.HSIState = RCC_HSI_ON;
	Osc_Init.PLL.PLLState = RCC_PLL_ON;
	Osc_Init.PLL.PLLSource = RCC_PLLSOURCE_HSE;

	Clock_Init.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
						  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	Clock_Init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;

	switch(clock_freq) {
		case SYS_CLOCK_FREQ_50_MHZ:
			Osc_Init.PLL.PLLM = 4;
			Osc_Init.PLL.PLLN = 50;
			Osc_Init.PLL.PLLP = RCC_PLLP_DIV2;
			Osc_Init.PLL.PLLQ = 2;
			Clock_Init.AHBCLKDivider = RCC_SYSCLK_DIV1;
			Clock_Init.APB1CLKDivider = RCC_HCLK_DIV2;
			Clock_Init.APB2CLKDivider = RCC_HCLK_DIV1;
			flash_latency = 1;
			break;

		case SYS_CLOCK_FREQ_84_MHZ:
			Osc_Init.PLL.PLLM = 4;
			Osc_Init.PLL.PLLN = 84;
			Osc_Init.PLL.PLLP = RCC_PLLP_DIV2;
			Osc_Init.PLL.PLLQ = 2;
			Clock_Init.AHBCLKDivider = RCC_SYSCLK_DIV1;
			Clock_Init.APB1CLKDivider = RCC_HCLK_DIV2;
			Clock_Init.APB2CLKDivider = RCC_HCLK_DIV1;
			flash_latency = 2;
			break;

		case SYS_CLOCK_FREQ_120_MHZ:
			Osc_Init.PLL.PLLM = 4;
			Osc_Init.PLL.PLLN = 120;
			Osc_Init.PLL.PLLP = RCC_PLLP_DIV2;
			Osc_Init.PLL.PLLQ = 2;
			Clock_Init.AHBCLKDivider = RCC_SYSCLK_DIV1;
			Clock_Init.APB1CLKDivider = RCC_HCLK_DIV4;
			Clock_Init.APB2CLKDivider = RCC_HCLK_DIV2;
			flash_latency = 3;
			break;

		default:
			return ;
	}

	if (HAL_RCC_OscConfig(&Osc_Init) != HAL_OK)	{
		Error_handler();
	}

	if (HAL_RCC_ClockConfig(&Clock_Init, flash_latency) != HAL_OK) {
		Error_handler();
	}

	/*Configure the systick timer interrupt frequency (for every 1 ms) */
	uint32_t hclk_freq = HAL_RCC_GetHCLKFreq();
	HAL_SYSTICK_Config(hclk_freq/1000);

	/**Configure the Systick
	*/
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void GPIO_Init(void)
{
	__HAL_RCC_GPIOF_CLK_ENABLE();
	GPIO_InitTypeDef btngpio;
	btngpio.Pin = GPIO_PIN_4;
	btngpio.Mode = GPIO_MODE_INPUT;
	btngpio.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOF, &btngpio);
}

void UART1_Init(void)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;

	if (HAL_UART_Init(&huart1) != HAL_OK) {
		//There is a problem
		Error_handler();
	}
}

void Error_handler(void)
{
	while(1);
}
