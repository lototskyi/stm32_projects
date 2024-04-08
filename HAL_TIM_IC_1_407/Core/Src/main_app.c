#include <string.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "main_app.h"

void SystemClockConfig(uint8_t clock_freq);
void SystemClock_Config_HSE(uint8_t clock_freq);
void GPIO_Init(void);
void Timer2_Init(void);
void LSE_Configuration(void);
void UART1_Init(void);

void SystemClock_Config(void);

void Error_handler(void);

TIM_HandleTypeDef htimer2;
UART_HandleTypeDef huart1;

uint32_t input_captures[2] = {0};
uint8_t count = 1;
uint8_t is_capture_done = FALSE;

int main(void)
{
	uint32_t capture_difference = 0;
	double timer2_cnt_freq = 0;
	double timer2_cnt_res = 0;
	double user_signal_time_period = 0;
	double user_signal_freq = 0;
	char usr_msg[100];
	char *start_msg = "Application running\r\n";

	HAL_Init();
	LSE_Configuration();
	//SystemClockConfig(SYS_CLOCK_FREQ_50_MHZ);
	SystemClock_Config_HSE(SYS_CLOCK_FREQ_50_MHZ);
	GPIO_Init();

	Timer2_Init();
	UART1_Init();



	HAL_TIM_IC_Start_IT(&htimer2, TIM_CHANNEL_1);

	HAL_UART_Transmit(&huart1, (uint8_t*)start_msg, strlen(start_msg), HAL_MAX_DELAY);

	while(1) {
		if (is_capture_done) {
			if (input_captures[1] > input_captures[0]) {
				capture_difference = input_captures[1] - input_captures[0];
			} else {
				capture_difference = (0xFFFFFFFF - input_captures[0]) + input_captures[1];
			}

			timer2_cnt_freq = (HAL_RCC_GetPCLK1Freq() * 2) / (htimer2.Init.Prescaler + 1);
			timer2_cnt_res = 1 / timer2_cnt_freq;
			user_signal_time_period = capture_difference * timer2_cnt_res;
			user_signal_freq = 1 / user_signal_time_period;

			sprintf(usr_msg, "Frequency of the signal applied = %f\r\n", user_signal_freq);
			HAL_UART_Transmit(&huart1, (uint8_t*)usr_msg, strlen(usr_msg), HAL_MAX_DELAY);

			is_capture_done = FALSE;
		}
	}

	return 1;
}

void SystemClockConfig(uint8_t clock_freq)
{
	RCC_OscInitTypeDef Osc_Init = {0};
	RCC_ClkInitTypeDef Clock_Init = {0};

	Osc_Init.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSE;
	Osc_Init.HSIState = RCC_HSI_ON;
	Osc_Init.LSEState = RCC_LSE_ON;
	Osc_Init.HSICalibrationValue = 16;
	Osc_Init.PLL.PLLState = RCC_PLL_ON;
	Osc_Init.PLL.PLLSource = RCC_PLLSOURCE_HSI;

	Clock_Init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	Clock_Init.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
						  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;

	switch(clock_freq) {
		case SYS_CLOCK_FREQ_50_MHZ:
			Osc_Init.PLL.PLLM = 8;
			Osc_Init.PLL.PLLN = 50;
			Osc_Init.PLL.PLLP = RCC_PLLP_DIV2;
			Osc_Init.PLL.PLLQ = 2;
			Clock_Init.AHBCLKDivider = RCC_SYSCLK_DIV1;
			Clock_Init.APB1CLKDivider = RCC_HCLK_DIV2;
			Clock_Init.APB2CLKDivider = RCC_HCLK_DIV1;
			break;

		case SYS_CLOCK_FREQ_84_MHZ:
			Osc_Init.PLL.PLLM = 8;
			Osc_Init.PLL.PLLN = 84;
			Osc_Init.PLL.PLLP = RCC_PLLP_DIV2;
			Osc_Init.PLL.PLLQ = 2;
			Clock_Init.AHBCLKDivider = RCC_SYSCLK_DIV1;
			Clock_Init.APB1CLKDivider = RCC_HCLK_DIV2;
			Clock_Init.APB2CLKDivider = RCC_HCLK_DIV1;
			break;

		case SYS_CLOCK_FREQ_120_MHZ:
			Osc_Init.PLL.PLLM = 8;
			Osc_Init.PLL.PLLN = 120;
			Osc_Init.PLL.PLLP = RCC_PLLP_DIV2;
			Osc_Init.PLL.PLLQ = 2;
			Clock_Init.AHBCLKDivider = RCC_SYSCLK_DIV1;
			Clock_Init.APB1CLKDivider = RCC_HCLK_DIV4;
			Clock_Init.APB2CLKDivider = RCC_HCLK_DIV2;
			break;

		default:
			return ;
		}

	if (HAL_RCC_OscConfig(&Osc_Init) != HAL_OK)	{
		Error_handler();
	}



	if (HAL_RCC_ClockConfig(&Clock_Init, FLASH_LATENCY_2) != HAL_OK) {
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

void SystemClock_Config_HSE(uint8_t clock_freq)
{
	RCC_OscInitTypeDef Osc_Init = {0};
	RCC_ClkInitTypeDef Clock_Init = {0};
    uint8_t flash_latency=0;

	Osc_Init.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_HSI ;
	Osc_Init.HSEState = RCC_HSE_ON;
	Osc_Init.LSEState = RCC_LSE_ON;
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

void Timer2_Init(void)
{
	TIM_IC_InitTypeDef timer2Ic_Config;

	htimer2.Instance = TIM2;
	htimer2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htimer2.Init.Period = 0xFFFFFFFF;
	htimer2.Init.Prescaler = 1;
	if (HAL_TIM_IC_Init(&htimer2) != HAL_OK) {
		Error_handler();
	}

	timer2Ic_Config.ICFilter = 0;
	timer2Ic_Config.ICPolarity = TIM_ICPOLARITY_RISING;
	timer2Ic_Config.ICPrescaler = TIM_ICPSC_DIV1;
	timer2Ic_Config.ICSelection = TIM_ICSELECTION_DIRECTTI;

	if (HAL_TIM_IC_ConfigChannel(&htimer2, &timer2Ic_Config, TIM_CHANNEL_1) != HAL_OK) {
		Error_handler();
	}
}

void GPIO_Init(void)
{
	__HAL_RCC_GPIOG_CLK_ENABLE();
	GPIO_InitTypeDef ledgpio;
	ledgpio.Pin = GPIO_PIN_13;
	ledgpio.Mode = GPIO_MODE_OUTPUT_PP;
	ledgpio.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOG, &ledgpio);
}

void LSE_Configuration(void)
{
	HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_LSE, RCC_MCODIV_1);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (! is_capture_done ) {
		if (count == 1) {
			input_captures[0] = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_1);
			count++;
		} else if (count == 2) {
			input_captures[1] = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_1);
			count = 1;
			is_capture_done = TRUE;
		}
	}
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