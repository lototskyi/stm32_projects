#include <string.h>
#include "stm32f4xx_hal.h"
#include "main_app.h"

#define TRUE				1
#define FALSE				0

void SystemClockConfig(void);
void UART1_Init(void);
uint8_t convert_to_capital(uint8_t data);
void Error_handler(void);

UART_HandleTypeDef huart1;

const char *user_data = "The application is running\r\n";
uint8_t data_buffer[100];
uint8_t recvd_data;
uint32_t count = 0;
uint8_t reception_complete = FALSE;

int main(void)
{
	HAL_Init();
	SystemClockConfig();

	UART1_Init();

	uint16_t len_of_data = strlen(user_data);

	if (HAL_UART_Transmit(&huart1, (const uint8_t*)user_data, len_of_data, HAL_MAX_DELAY) != HAL_OK) {
		Error_handler();
	}


	while (reception_complete != TRUE) {
		HAL_UART_Receive_IT(&huart1, &recvd_data, 1);
	}


	while(1);

	return 1;
}

void SystemClockConfig(void)
{

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

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (recvd_data == '\r') {
		reception_complete = TRUE;
		data_buffer[count++] = '\r';
		HAL_UART_Transmit(huart, data_buffer, count, HAL_MAX_DELAY);
	} else {
		data_buffer[count++] = recvd_data;
	}
}

uint8_t convert_to_capital(uint8_t data)
{
	if (data >= 'a' && data <= 'z') {
		data = data - ('a' - 'A');
	}

	return data;
}

void Error_handler(void)
{
	while(1);
}
