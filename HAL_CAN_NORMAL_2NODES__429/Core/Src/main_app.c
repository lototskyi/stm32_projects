#include <string.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "main_app.h"

void SystemClock_Config_HSE(uint8_t clock_freq);
void GPIO_Init(void);
void UART1_Init(void);
void TIMER6_Init(void);
void CAN1_Init(void);
void CAN_Filter_Config(void);
void CAN1_Tx(void);
void CAN1_Rx(void);
void LED_Manage_Output(uint8_t led_no);
void Send_response(uint32_t StdId);

void Error_handler(void);

UART_HandleTypeDef huart1;
CAN_HandleTypeDef hcan1;
TIM_HandleTypeDef htimer6;

CAN_RxHeaderTypeDef RxHeader;

uint8_t req_counter = 0;

int main(void)
{
	HAL_Init();

	SystemClock_Config_HSE(SYS_CLOCK_FREQ_84_MHZ);
	GPIO_Init();
	UART1_Init();

	TIMER6_Init();

	CAN1_Init();

	CAN_Filter_Config();

	if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_BUSOFF) != HAL_OK) {
		Error_handler();
	}

	if (HAL_CAN_Start(&hcan1) != HAL_OK) {
		Error_handler();
	}



	while (1) {

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
	Osc_Init.LSEState = RCC_LSE_OFF;
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

void GPIO_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE(); //BTN
	__HAL_RCC_GPIOG_CLK_ENABLE(); //LED
	//__HAL_RCC_GPIOF_CLK_ENABLE(); //LED

	GPIO_InitTypeDef btngpio = {0};
	GPIO_InitTypeDef ledgpio = {0};

	btngpio.Pin = GPIO_PIN_0;
	btngpio.Mode = GPIO_MODE_IT_RISING;
	btngpio.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &btngpio);

	ledgpio.Pin = GPIO_PIN_13 | GPIO_PIN_14;
	ledgpio.Mode = GPIO_MODE_OUTPUT_PP;
	ledgpio.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOG, &ledgpio);

//	ledgpio.Pin = GPIO_PIN_10;
//	ledgpio.Mode = GPIO_MODE_OUTPUT_PP;
//	ledgpio.Pull = GPIO_NOPULL;
//	HAL_GPIO_Init(GPIOF, &ledgpio);

	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void TIMER6_Init(void)
{
	htimer6.Instance = TIM6;
	htimer6.Init.Prescaler = 1343;
	htimer6.Init.Period = 62499;
	if (HAL_TIM_Base_Init(&htimer6) != HAL_OK) {
		Error_handler();
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

void CAN1_Init(void)
{
	hcan1.Instance = CAN1;
	hcan1.Init.Mode = CAN_MODE_NORMAL;
	hcan1.Init.AutoBusOff = DISABLE;
	hcan1.Init.AutoRetransmission = ENABLE;
	hcan1.Init.AutoWakeUp = DISABLE;
	hcan1.Init.ReceiveFifoLocked = DISABLE;
	hcan1.Init.TimeTriggeredMode = DISABLE;
	hcan1.Init.TransmitFifoPriority = DISABLE;

	hcan1.Init.Prescaler = 3;
	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan1.Init.TimeSeg1 = CAN_BS1_11TQ;
	hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;

	if (HAL_CAN_Init(&hcan1) != HAL_OK) {
		Error_handler();
	}
}

void CAN_Filter_Config(void)
{
	CAN_FilterTypeDef can1_filter_init;

	can1_filter_init.FilterActivation = ENABLE;
	can1_filter_init.FilterBank = 0;
	can1_filter_init.FilterFIFOAssignment = CAN_RX_FIFO0;
	can1_filter_init.FilterIdHigh = 0x0000;
	can1_filter_init.FilterIdLow = 0x0000;
	can1_filter_init.FilterMaskIdHigh = 0x01C0;
	can1_filter_init.FilterMaskIdLow = 0x0000;
	can1_filter_init.FilterMode = CAN_FILTERMODE_IDMASK;
	can1_filter_init.FilterScale = CAN_FILTERSCALE_32BIT;

	if (HAL_CAN_ConfigFilter(&hcan1, &can1_filter_init) != HAL_OK) {
		Error_handler();
	}

}

uint8_t led_no = 0;

void CAN1_Tx(void)
{
	CAN_TxHeaderTypeDef TxHeader = {0};

	uint32_t TxMailbox;

	uint8_t message;

	TxHeader.DLC = 1;
	TxHeader.StdId = 0x65D;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;

	message = ++led_no;

	if (led_no == 2) {
		led_no = 0;
	}

	HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);

	if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, &message, &TxMailbox) != HAL_OK) {
		Error_handler();
	}
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
	char msg[50] = {0};
	sprintf(msg, "Message Transmitted:M0\r\n");
	HAL_UART_Transmit(&huart1, (uint8_t*)msg, sizeof(msg), HAL_MAX_DELAY);
}
//
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{
	char msg[50] = {0};
	sprintf(msg, "Message Transmitted:M1\r\n");
	HAL_UART_Transmit(&huart1, (uint8_t*)msg, sizeof(msg), HAL_MAX_DELAY);
}
//
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{
	char msg[50] = {0};
	sprintf(msg, "Message Transmitted:M2\r\n");
	HAL_UART_Transmit(&huart1, (uint8_t*)msg, sizeof(msg), HAL_MAX_DELAY);
}
//
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	char msg[50];

	uint8_t rcvd_msg[8];

	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, rcvd_msg) != HAL_OK) {
		Error_handler();
	}

	if (RxHeader.StdId == 0x65D && RxHeader.RTR == CAN_RTR_DATA) {
		//This is data frame sent by n1 to n2
		LED_Manage_Output(rcvd_msg[0]);
		sprintf(msg, "Message Received : %#X\r\n", rcvd_msg[0]);
	} else if (RxHeader.StdId == 0x651 && RxHeader.RTR == CAN_RTR_REMOTE) {
		//This is a remote frame sent by n1 to n2
		Send_response(RxHeader.StdId);
		return;
	} else if (RxHeader.StdId == 0x651 && RxHeader.RTR == 0) {
		//it's a reply (data frame) by n2 to n1
		sprintf(msg, "Reply Received : %#X\r\n", rcvd_msg[0] << 8 | rcvd_msg[1]);
	}

//	sprintf(msg, "Message Received : %s\r\n", rcvd_msg);
	HAL_UART_Transmit(&huart1, (uint8_t*)msg, sizeof(msg), HAL_MAX_DELAY);
}
//
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	char msg[50] = {0};
	sprintf(msg, "CAN Error Detected\r\n");
	HAL_UART_Transmit(&huart1, (uint8_t*)msg, sizeof(msg), HAL_MAX_DELAY);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	CAN_TxHeaderTypeDef TxHeader = {0};

	uint32_t TxMailbox;

	char * message = "";

	if (req_counter == 2) {
		TxHeader.DLC = 2;
		TxHeader.StdId = 0x651;
		TxHeader.IDE = CAN_ID_STD;
		TxHeader.RTR = CAN_RTR_REMOTE;

		if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, (uint8_t*)&message, &TxMailbox) != HAL_OK) {
			Error_handler();
		}

		req_counter = 0;
	} else {
		CAN1_Tx();
		req_counter++;
	}
//	CAN1_Tx();
}


void LED_Manage_Output(uint8_t led_no)
{
	switch(led_no) {
	case 1:
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
		break;
	}
}

void Send_response(uint32_t StdId)
{
	CAN_TxHeaderTypeDef TxHeader = {0};

	uint32_t TxMailbox;

	uint8_t response[2] = {0xAB, 0xCD};

	TxHeader.DLC = 2;
	TxHeader.StdId = StdId;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;

	if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, response, &TxMailbox) != HAL_OK) {
		Error_handler();
	}
}

void Error_handler(void)
{
	while(1);
}
