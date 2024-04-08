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

	//1. enable the clock for the UART6 peripheral

	__HAL_RCC_USART6_CLK_ENABLE();
	__HAL_RCC_USART6_CLK_SLEEP_DISABLE();

	//2. do the pin muxing configurations
	gpio_uart.Pin = GPIO_PIN_6; // USART6_TX
	gpio_uart.Mode = GPIO_MODE_AF_PP;
	gpio_uart.Pull = GPIO_NOPULL;
	gpio_uart.Speed = GPIO_SPEED_FREQ_LOW;
	gpio_uart.Alternate = GPIO_AF8_USART6;

	HAL_GPIO_Init(GPIOC, &gpio_uart);

	gpio_uart.Pin = GPIO_PIN_7; // USART6_RX
	HAL_GPIO_Init(GPIOC, &gpio_uart);

	//3. enable the IRQ and set up the priority
//	HAL_NVIC_EnableIRQ(USART6_IRQn);
//	HAL_NVIC_SetPriority(USART6_IRQn, 15, 0);
}
