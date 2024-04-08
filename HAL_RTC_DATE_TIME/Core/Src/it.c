#include "main_app.h"
#include "it.h"

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void EXTI4_IRQHandler()
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}


