#include <string.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "main_app.h"

void SystemClock_Config_HSE(uint8_t clock_freq);
void GPIO_Init(void);
void Timer2_Init(void);
void UART1_Init(void);

void Error_handler(void);

TIM_HandleTypeDef htimer2;
UART_HandleTypeDef huart1;

uint32_t input_captures[2] = {0};
uint8_t count = 1;
uint8_t is_capture_done = FALSE;

uint32_t ccr_content;

uint32_t pulse_1hz = 12500000;

uint32_t pulse1_value = 25000; //to produce 500Hz
uint32_t pulse2_value = 12500; //to produce 1000Hz
uint32_t pulse3_value = 6250;  //to produce 2000Hz
uint32_t pulse4_value = 3125;  //to produce 4000Hz

int main(void)
{
	HAL_Init();

	SystemClock_Config_HSE(SYS_CLOCK_FREQ_50_MHZ);
	GPIO_Init();
	Timer2_Init();
//	UART1_Init();

	if (HAL_TIM_OC_Start_IT(&htimer2, TIM_CHANNEL_1) != HAL_OK) {
		Error_handler();
	}

	if (HAL_TIM_OC_Start_IT(&htimer2, TIM_CHANNEL_2) != HAL_OK) {
		Error_handler();
	}

	if (HAL_TIM_OC_Start_IT(&htimer2, TIM_CHANNEL_3) != HAL_OK) {
		Error_handler();
	}

	if (HAL_TIM_OC_Start_IT(&htimer2, TIM_CHANNEL_4) != HAL_OK) {
		Error_handler();
	}

	while(1) {

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
	TIM_OC_InitTypeDef timer2OC_Config = {0};

	htimer2.Instance = TIM2;
	htimer2.Init.Period = 0xFFFFFFFF;
	htimer2.Init.Prescaler = 1;

	if (HAL_TIM_OC_Init(&htimer2) != HAL_OK) {
		Error_handler();
	}

	timer2OC_Config.OCMode = TIM_OCMODE_TOGGLE;
	timer2OC_Config.OCPolarity = TIM_OCPOLARITY_HIGH;

	timer2OC_Config.Pulse = pulse_1hz;
	if (HAL_TIM_OC_ConfigChannel(&htimer2, &timer2OC_Config, TIM_CHANNEL_1) != HAL_OK) {
		Error_handler();
	}

	timer2OC_Config.Pulse = pulse2_value;
	if (HAL_TIM_OC_ConfigChannel(&htimer2, &timer2OC_Config, TIM_CHANNEL_2) != HAL_OK) {
		Error_handler();
	}

	timer2OC_Config.Pulse = pulse3_value;
	if (HAL_TIM_OC_ConfigChannel(&htimer2, &timer2OC_Config, TIM_CHANNEL_3) != HAL_OK) {
		Error_handler();
	}

	timer2OC_Config.Pulse = pulse4_value;
	if (HAL_TIM_OC_ConfigChannel(&htimer2, &timer2OC_Config, TIM_CHANNEL_4) != HAL_OK) {
		Error_handler();
	}

}

void GPIO_Init(void)
{
	__HAL_RCC_GPIOF_CLK_ENABLE();
	GPIO_InitTypeDef ledgpio;
	ledgpio.Pin = GPIO_PIN_9;
	ledgpio.Mode = GPIO_MODE_OUTPUT_PP;
	ledgpio.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOG, &ledgpio);
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

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{

	//500Hz
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
		ccr_content = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, ccr_content + pulse_1hz);
	}

	//1000Hz
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
		ccr_content = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, ccr_content + pulse2_value);
	}

	//2000Hz
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
		ccr_content = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, ccr_content + pulse3_value);
	}

	//4000Hz
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
		ccr_content = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, ccr_content + pulse4_value);
	}
}

void Error_handler(void)
{
	while(1);
}
