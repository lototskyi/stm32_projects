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
	GPIO_InitTypeDef gpio_uart;

	//1. enable the clock for the UART2 peripheral
	__HAL_RCC_USART1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	//2. do the pin muxing configurations
	gpio_uart.Pin = GPIO_PIN_9; // USART2_TX
	gpio_uart.Mode = GPIO_MODE_AF_PP;
	gpio_uart.Pull = GPIO_PULLUP;
	gpio_uart.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	gpio_uart.Alternate = GPIO_AF7_USART1;

	HAL_GPIO_Init(GPIOA, &gpio_uart);

	gpio_uart.Pin = GPIO_PIN_10; // USART2_RX
	HAL_GPIO_Init(GPIOA, &gpio_uart);

	//3. enable the IRQ and set up the priority
	HAL_NVIC_EnableIRQ(USART1_IRQn);
	HAL_NVIC_SetPriority(USART1_IRQn, 15, 0);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef tim2OC_gpios;

	//enable the clock for timer2 and GPIOA
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	//AF configuration: a gpio to behave as timer 2 channel 1, 2, 3 and 4
	//PA0 --> TIM2_CH1

	tim2OC_gpios.Pin = GPIO_PIN_0;
	tim2OC_gpios.Mode = GPIO_MODE_AF_PP;
	tim2OC_gpios.Alternate = GPIO_AF1_TIM2;
	tim2OC_gpios.Pull = GPIO_NOPULL;
	tim2OC_gpios.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &tim2OC_gpios);

	HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
}
