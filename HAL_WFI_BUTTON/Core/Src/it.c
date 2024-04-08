#include "main_app.h"
#include "it.h"

extern UART_HandleTypeDef huart1;

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void USART6_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart1);
}

void EXTI0_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}
