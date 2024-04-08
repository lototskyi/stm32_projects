#include "main_app.h"
#include "it.h"

extern TIM_HandleTypeDef htimer6;
extern UART_HandleTypeDef huart1;

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void TIM6_DAC_IRQHandler(void)
{
//	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_TIM_IRQHandler(&htimer6);
//	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_RESET);
}

void USART2_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart1);
}
