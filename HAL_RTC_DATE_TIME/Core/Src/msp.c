#include "main_app.h"

extern void Error_handler(void);

void HAL_MspInit(void)
{
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	SCB->SHCSR |= 0x7 << 16; //usg fault, memory fault and bus fault system exception

	//configure priority for system exception
	HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef gpio_uart = {0};

	//1. enable the clock for the UART1 peripheral
	__HAL_RCC_USART1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	//2. do the pin muxing configurations
	gpio_uart.Pin = GPIO_PIN_9; // USART1_TX
	gpio_uart.Mode = GPIO_MODE_AF_PP;
	gpio_uart.Pull = GPIO_PULLUP;
	gpio_uart.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	gpio_uart.Alternate = GPIO_AF7_USART1;

	HAL_GPIO_Init(GPIOA, &gpio_uart);

	gpio_uart.Pin = GPIO_PIN_10; // USART1_RX
	HAL_GPIO_Init(GPIOA, &gpio_uart);

	//3. enable the IRQ and set up the priority
	HAL_NVIC_EnableIRQ(USART1_IRQn);
	HAL_NVIC_SetPriority(USART1_IRQn, 15, 0);
}

void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
	RCC_OscInitTypeDef Osc_Init = {0};
	RCC_PeriphCLKInitTypeDef RCC_RTCPeriClkInit = {0};

	//1. Turn on the LSE
	Osc_Init.OscillatorType = RCC_OSCILLATORTYPE_LSE;
	Osc_Init.LSEState = RCC_LSE_ON;
	Osc_Init.PLL.PLLState = RCC_PLL_NONE;

	if (HAL_RCC_OscConfig(&Osc_Init) != HAL_OK)	{
		Error_handler();
	}

	//2. select LSE as RTCCLK
	RCC_RTCPeriClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	RCC_RTCPeriClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	if (HAL_RCCEx_PeriphCLKConfig(&RCC_RTCPeriClkInit) != HAL_OK) {
		Error_handler();
	}

	//3. Enable the RTC clock
	__HAL_RCC_RTC_ENABLE();
}
