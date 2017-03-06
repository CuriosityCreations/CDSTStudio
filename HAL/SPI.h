/**
 * @file SPI.h
 * @brief SPI.h
 * @author  bespoon.com
 * @date 30/1/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HAL_SPI_H
#define __HAL_SPI_H

#include "stm32f0xx_spi.h"
#include "stm32f0xx_dma.h"

void HAL_SPI_DeInit (void);
void HAL_SPI_Init (void);
void HAL_SPI1_Init (void);
void HAL_SPI2_Init (void);
void HAL_SPI1_Init_Slave (void);
void HAL_SPI2_Init_Slave (void);
void HAL_SPI1_write_buffer(unsigned char *data_reg, unsigned short size_reg);
void HAL_SPI1_read_buffer(unsigned char *data_reg, unsigned short size_reg, unsigned char *data_in, unsigned short size_in);
void HAL_SPI1_read_burst(unsigned char *data_reg, unsigned short size_reg, unsigned char *data_out, unsigned short size_out);
void HAL_SPI1_write_burst(unsigned char *data_reg, unsigned short size_reg, unsigned char *data_in, unsigned short size_in);
void HAL_SPI2_write_buffer(unsigned char *data_reg, unsigned short size_reg);
void HAL_SPI2_read_buffer(unsigned char *data_reg, unsigned short size_reg, unsigned char *data_in, unsigned short size_in);
void HAL_SPI2_read_burst(unsigned char *data_reg, unsigned short size_reg, unsigned char *data_out, unsigned short size_out);
void HAL_SPI2_write_burst(unsigned char *data_reg, unsigned short size_reg, unsigned char *data_in, unsigned short size_in);
void HAL_SPI2_bridge(unsigned char *data_in, unsigned short size_in, unsigned char *data_out, unsigned short size_out);



#endif /* __HAL_SPI_H */
