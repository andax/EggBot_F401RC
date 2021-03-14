/*
 * parser.c
 *
 *  Created on: Mar 13, 2021
 *      Author: Andreas
 *
 *      Based on: https://evil-mad.github.io/EggBot/ebb.html
 *
 */
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "cmsis_os.h"
#include "parser.h"
#include "stepper.h"
#include "servo.h"
#include "usbd_cdc_if.h"
#include "segger_rtt.h"

#define RX_CMD_BUFFER_LEN 100

uint8_t  rx_cmd_buffer[RX_CMD_BUFFER_LEN];
uint16_t rx_cmd_buffer_cnt = 0;
uint8_t  tx_buffer[100];

extern osMessageQId cmdQueueHandle;
extern osPoolId  commandPool_Id;

void Parse_Command()
{
	uint16_t cmd_token;
	int32_t value1 = 0;
	int32_t value2 = 0;
	int32_t value3 = 0;

	SEGGER_RTT_WriteString(0, (const char*)rx_cmd_buffer);
	SEGGER_RTT_WriteString(0, "\r\n");


	cmd_token = (rx_cmd_buffer[0] << 8) + rx_cmd_buffer[1];
	if (rx_cmd_buffer[1] == ',' || rx_cmd_buffer[1] == 0) // Check if we have received a one letter command
	{
		cmd_token = rx_cmd_buffer[0];
	}

	command_t* pCommand;
	pCommand = osPoolAlloc(commandPool_Id); // Allocate memory for the message
	memset(pCommand, 0, sizeof(command_t));

	switch (cmd_token)
	{
	case ('E'<<8) + 'M':
		sscanf((const char*) rx_cmd_buffer, "EM,%ld,%ld", &value1, &value2);
		pCommand->cmd = CMD_EM;
		pCommand->value1 = value1;
		pCommand->value2 = value2;
		break;

	case ('H'<<8) + 'M':
		sscanf((const char*) rx_cmd_buffer, "SM,%ld,%ld,%ld", &value1, &value2, &value3);
		pCommand->cmd = CMD_HM;
		pCommand->value1 = value1;
		pCommand->value2 = value2;
		pCommand->value3 = value3;
		break;

	case ('Q'<<8) + 'B':
		pCommand->cmd = CMD_QB;
		break;

	case ('Q'<<8) + 'P':
		pCommand->cmd = CMD_QP;
		break;

	case ('Q'<<8) + 'S':
		pCommand->cmd = CMD_QS;
		break;

	case 'R':
		pCommand->cmd = CMD_R;
		break;

	case ('S'<<8) + 'C':
		sscanf((const char*) rx_cmd_buffer, "SC,%ld,%ld", &value1, &value2);
		pCommand->cmd = CMD_SC;
		pCommand->value1 = value1;
		pCommand->value2 = value2;
		break;

	case ('S'<<8) + 'E':
		pCommand->cmd = CMD_SE;
		break;

	case ('S'<<8) + 'M':
		sscanf((const char*) rx_cmd_buffer, "SM,%ld,%ld,%ld", &value1, &value2, &value3);
		pCommand->cmd = CMD_SM;
		pCommand->value1 = value1;
		pCommand->value2 = value2;
		pCommand->value3 = value3;
		break;

	case ('S'<<8) + 'P':
		sscanf((const char*) rx_cmd_buffer, "SP,%ld,%ld", &value1, &value2);
		pCommand->cmd = CMD_SP;
		pCommand->value1 = value1;
		pCommand->value2 = value2;
		break;

	case ('T'<<8) + 'P':
		sscanf((const char*) rx_cmd_buffer, "TP,%ld", &value1);
		pCommand->cmd = CMD_TP;
		pCommand->value1 = value1;
		break;

	case 'V':
		pCommand->cmd = CMD_V;
		break;

	default:
		pCommand->cmd = CMD_ERROR;
		break;
	}

	osStatus status = osMessagePut(cmdQueueHandle, (uint32_t)pCommand, 0);  // Send Message
	if (status != osOK)
	{
		TransmitMsg("ERROR\r\n");
	}
}

void Command_OnReceive(uint8_t* Buf, uint16_t Len)
{
	for (uint16_t cnt = 0; cnt < Len; cnt++)
	{
		uint8_t d = Buf[cnt];
		if (d == '\n') // Discard newline
		{
			continue;
		}
		else if (d == '\t') // Discard tabs
		{
			continue;
		}
		else if (d >= 0x20 && d <= 0x7f)
		{
			if (rx_cmd_buffer_cnt < RX_CMD_BUFFER_LEN-2)
			{
				rx_cmd_buffer[rx_cmd_buffer_cnt++] = toupper(d);
			}
		}
		else if (d == '\r') // Carriage return received. Execute command
		{
			rx_cmd_buffer[rx_cmd_buffer_cnt++] = '\0';	// terminate string

			Parse_Command() ;	// Dispatch the current command line for parsing

			rx_cmd_buffer_cnt = 0;			// Reset cmd buffer for next command
		}
	}
}

void TransmitMsg(char* msg)
{
	CDC_Transmit_FS((uint8_t*)msg, strlen(msg));
	SEGGER_RTT_WriteString(0, msg);
}

