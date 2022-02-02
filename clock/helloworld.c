/******************************************************************************
 *
 * Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications:
 * (a) running on a Xilinx device, or
 * (b) that interact with a Xilinx device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 ******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xspips.h"	 /* SPI device driver */
#include "xgpiops.h" //°üº¬PS GPIOµÄº¯Êý
#include "sleep.h"
#define GPIO_DEVICE_ID XPAR_XGPIOPS_0_DEVICE_ID
#define SPI_DEVICE_ID XPAR_XSPIPS_0_DEVICE_ID
static XSpiPs SpiInstancePtr;
u32 SPI_Run(XSpiPs *SpiInstancePtr, u32 *data)
{
	int Status;
	u32 rx_data = 0;
	u8 tx_data[4];
	tx_data[3]=*((u8 *)data+0);
	tx_data[2]=*((u8 *)data+1);
	tx_data[1]=*((u8 *)data+2);
	tx_data[0]=*((u8 *)data+3);
	/* send wite enable */
	Status = XSpiPs_PolledTransfer(SpiInstancePtr, tx_data, &rx_data, 4);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	return rx_data;
}
int main()
{
	uint32_t lmk_reg[28] = {
			0x80160140 ,
			0x800000C0 ,
			0x00000301 ,
			0x00000962 ,
			0x00000303 ,
			0x00000964 ,
			0x00002585 ,
			0x04040006 ,
			0x44CC0007 ,
			0xC0C00008 ,
			0x55555549 ,
			0x9000440A ,
			0x03F1300B ,
			0x1B0C016C ,
			0x2363102D ,
			0x1300000E ,
			0x8000800F ,
			0x01550410 ,
			0x000000D8 ,
			0x80010019 ,
			0x0FA8001A ,
			0x1800005B ,
			0x0010065C ,
			0x0100019D ,
			0x0100019E ,
			0x0029001F
	};
	int i = 0;
	int Status;
	u32 rx_data;
	XSpiPs_Config *SpiConfig;

	/*
	 * Initialize the SPI driver so that it's ready to use
	 */
	SpiConfig = XSpiPs_LookupConfig(SPI_DEVICE_ID);
	if (NULL == SpiConfig)
	{
		return XST_FAILURE;
	}

	Status = XSpiPs_CfgInitialize(&SpiInstancePtr, SpiConfig,
								  SpiConfig->BaseAddress);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to check hardware build
	 */
	Status = XSpiPs_SelfTest(&SpiInstancePtr);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	/*
	 * Set the Spi device as a master. External loopback is required.
	 */
	XSpiPs_SetOptions(&SpiInstancePtr, XSPIPS_MASTER_OPTION | XSPIPS_FORCE_SSELECT_OPTION);

	XSpiPs_SetClkPrescaler(&SpiInstancePtr, XSPIPS_CLK_PRESCALE_128);
	/*
	 * Assert the EEPROM chip select
	 */
	u32 data=0;
	XSpiPs_SetSlaveSelect(&SpiInstancePtr, 0);
	for (i = 0; i < 26; i++)
	{
		data=SPI_Run(&SpiInstancePtr, lmk_reg+i);
		if(i<=4)data=SPI_Run(&SpiInstancePtr, lmk_reg+i);
		usleep(3000);
	}
	u32 tx_data = 0x000001F;
	for(i=0;i<20;i++)
	{
		tx_data = 0x000001F|(i<<16);
	data=SPI_Run(&SpiInstancePtr, &tx_data);
	data=SPI_Run(&SpiInstancePtr, &tx_data);
	}
	return 0;
}
