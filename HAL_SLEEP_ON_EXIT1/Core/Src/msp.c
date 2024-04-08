#include "main_app.h"

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

	//1. enable the clock for the UART2 peripheral

	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_USART2_CLK_SLEEP_DISABLE();

	__HAL_RCC_GPIOA_CLK_ENABLE();

	//2. do the pin muxing configurations
	gpio_uart.Pin = GPIO_PIN_2; // USART2_TX
	gpio_uart.Mode = GPIO_MODE_AF_PP;
	gpio_uart.Pull = GPIO_PULLUP;
	gpio_uart.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	gpio_uart.Alternate = GPIO_AF7_USART2;

	HAL_GPIO_Init(GPIOA, &gpio_uart);

	gpio_uart.Pin = GPIO_PIN_3; // USART2_RX
	HAL_GPIO_Init(GPIOA, &gpio_uart);

	//3. enable the IRQ and set up the priority
	HAL_NVIC_EnableIRQ(USART2_IRQn);
	HAL_NVIC_SetPriority(USART2_IRQn, 15, 0);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	//enable the clock for TIM6 peripheral
	__HAL_RCC_TIM6_CLK_ENABLE();

	//enable the IRQ of TIM6
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

	//setup the priority
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0, 0);
}
