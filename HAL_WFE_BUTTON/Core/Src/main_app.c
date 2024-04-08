#include <string.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "main_app.h"

void SystemClock_Config_HSE(uint8_t clock_freq);
void GPIO_Init(void);
void UART1_Init(void);
void GPIO_AnalogConfig(void);

void Error_handler(void);

UART_HandleTypeDef huart1;

extern char some_data[];

int main(void)
{
	char msg[50] = {0};

	HAL_Init();

//	SystemClock_Config_HSE(SYS_CLOCK_FREQ_50_MHZ);
	GPIO_Init();

	UART1_Init();

	GPIO_AnalogConfig();

//	HAL_PWR_EnableSleepOnExit();


	while (1) {

//		if ( HAL_UART_Transmit(&huart6, (uint8_t*)some_data, (uint16_t)strlen((char*)some_data), HAL_MAX_DELAY)!= HAL_OK ) {
//			Error_handler();
//		}

		memset(msg, 0, sizeof(msg));
		sprintf(msg, "Going to sleep !\r\n");

		if ( HAL_UART_Transmit(&huart1, (uint8_t*)msg, (uint16_t)strlen((char*)msg), HAL_MAX_DELAY) != HAL_OK ) {
			Error_handler();
		}

		HAL_SuspendTick();

		/* going to sleep here */
		__SEV(); // Send event -> just to make sure an event is always available even if the systick didn't cause any event
		__WFE(); // Clears the event and resumes back
		__WFE(); // Goes to sleep definitely

		HAL_ResumeTick();

		memset(msg, 0, sizeof(msg));
		sprintf(msg, "Woke up !\r\n");

		if ( HAL_UART_Transmit(&huart1, (uint8_t*)msg, (uint16_t)strlen((char*)msg), HAL_MAX_DELAY)!= HAL_OK ) {
			Error_handler();
		}
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

void GPIO_AnalogConfig(void)
{
	GPIO_InitTypeDef gpioa, gpioc = {0};

	uint32_t gpio_pins = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | \
						 GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | \
						 GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | \
						 GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | \
						 GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	gpioa.Pin = gpio_pins;
	gpioa.Mode = GPIO_MODE_ANALOG;
	HAL_GPIO_Init(GPIOA, &gpioa);

	gpio_pins = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | \
				GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | \
				GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | \
				GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | \
				GPIO_PIN_14 | GPIO_PIN_15;
	gpioc.Pin = gpio_pins;
	gpioc.Mode = GPIO_MODE_ANALOG;
	HAL_GPIO_Init(GPIOC, &gpioc);
}

void GPIO_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE(); //BTN
	__HAL_RCC_GPIOC_CLK_ENABLE(); //USART6
	__HAL_RCC_GPIOC_CLK_SLEEP_DISABLE();

	GPIO_InitTypeDef btngpio;

	btngpio.Pin = GPIO_PIN_0;
	btngpio.Mode = GPIO_MODE_EVT_RISING;
	btngpio.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &btngpio);

#if 0
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

#endif
}

void UART1_Init(void)
{
	huart1.Instance = USART6;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.Mode = UART_MODE_TX;

	if (HAL_UART_Init(&huart1) != HAL_OK) {
		//There is a problem
		Error_handler();
	}
}

#if 0
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if ( HAL_UART_Transmit(&huart1, (uint8_t*)some_data, (uint16_t)strlen((char*)some_data), HAL_MAX_DELAY)!= HAL_OK ) {
		Error_handler();
	}
}
#endif

void Error_handler(void)
{
	while(1);
}
