/*
 * parser.h
 *
 *  Created on: Mar 13, 2021
 *      Author: Andreas
 */

#ifndef INC_PARSER_H_
#define INC_PARSER_H_

typedef enum {
	CMD_NOT_DEFINED = 0,
	CMD_EM,
	CMD_HM,
	CMD_QB,
	CMD_QP,
	CMD_QS,
	CMD_R,
	CMD_SC,
	CMD_SE,
	CMD_SM,
	CMD_SP,
	CMD_TP,
	CMD_V,
	CMD_ERROR
} cmd_enum_t;

typedef struct {
	cmd_enum_t cmd;
	int32_t value1;
	int32_t value2;
	int32_t value3;
} command_t;

void Command_OnReceive(uint8_t* Buf, uint16_t Len);
void TransmitMsg(char* msg);

#endif /* INC_PARSER_H_ */
