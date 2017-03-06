/**
 * @file USART.h
 * @brief USART.h
 * @author  bespoon.com
 * @date 30/1/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HAL_UART_H
#define __HAL_UART_H

extern unsigned char USE_SHELL_USART_NB;

void HAL_USART1_Init ();
void HAL_USART2_Init ();
/* return -1 if nothing happen, 0 if char on uart, with char val filled*/
int HAL_USART_GetChar(char* val);
void HAL_USART3_Init ();
void HAL_USART3_Sync_write_buffer(unsigned char *data_out, unsigned short size_out);
void HAL_USART3_Sync_read_buffer(unsigned char *data_out, unsigned short size_out, unsigned char *data_in, unsigned short size_in);
void HAL_USART4_Init ();

void HAL_USART_DeInit();
void HAL_USART_Init ();

#endif /* __HAL_UART_H */
