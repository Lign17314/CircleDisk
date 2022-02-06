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
#include "xgpiops.h"
#include "sleep.h"

// MISO 45
// CLK 40
// CS 42
static XGpioPs psGpioInstancePtr;
void lmk_wdata(uint32_t data)
{
    int i = 0;
    usleep(10);
    XGpioPs_WritePin(&psGpioInstancePtr, 42, 1);
    usleep(10);
    XGpioPs_WritePin(&psGpioInstancePtr, 42, 0);
    usleep(10);
    for (i = 31; i >= 0; i--)
    {
        if (data & (1 << i))
            XGpioPs_WritePin(&psGpioInstancePtr, 45, 1);
        else
            XGpioPs_WritePin(&psGpioInstancePtr, 45, 0);
        usleep(10);
        XGpioPs_WritePin(&psGpioInstancePtr, 40, 0);
        usleep(10);
        XGpioPs_WritePin(&psGpioInstancePtr, 40, 1);
        usleep(10);
        ;
        XGpioPs_WritePin(&psGpioInstancePtr, 40, 0);
        usleep(10);
    }
    usleep(10);
    XGpioPs_WritePin(&psGpioInstancePtr, 42, 1);
    usleep(10);
    XGpioPs_WritePin(&psGpioInstancePtr, 42, 0);
    XGpioPs_WritePin(&psGpioInstancePtr, 40, 0);
    XGpioPs_WritePin(&psGpioInstancePtr, 45, 0);
    usleep(10);
}
u32 lmk_read(uint32_t addr)
{
    int i = 0;
    u32 data = 0x1f | (addr << 16), data1 = 0, data2 = 0;
    XGpioPs_WritePin(&psGpioInstancePtr, 42, 1);
    usleep(10);
    XGpioPs_WritePin(&psGpioInstancePtr, 42, 0);
    usleep(10);
    // lmk_wdata(0x1f);
    for (i = 31; i >= 0; i--)
    {
        if (data & (1 << i))
            XGpioPs_WritePin(&psGpioInstancePtr, 45, 1);
        else
            XGpioPs_WritePin(&psGpioInstancePtr, 45, 0);
        XGpioPs_WritePin(&psGpioInstancePtr, 40, 0);
        usleep(10);
        XGpioPs_WritePin(&psGpioInstancePtr, 40, 1);
        usleep(10);
        XGpioPs_WritePin(&psGpioInstancePtr, 40, 0);
    }
    XGpioPs_WritePin(&psGpioInstancePtr, 42, 1);
    usleep(10);
    XGpioPs_WritePin(&psGpioInstancePtr, 42, 0);
    XGpioPs_WritePin(&psGpioInstancePtr, 45, 0);
    usleep(10);
    data = 0x1f | (addr << 16);
    for (i = 31; i >= 0; i--)
    {
        XGpioPs_WritePin(&psGpioInstancePtr, 40, 1);
        usleep(10);
        XGpioPs_WritePin(&psGpioInstancePtr, 40, 0);
        data2 = data2 | (XGpioPs_ReadPin(&psGpioInstancePtr, 41) << i);
        usleep(10);
    }
    XGpioPs_WritePin(&psGpioInstancePtr, 42, 1);
    usleep(10);
    XGpioPs_WritePin(&psGpioInstancePtr, 42, 0);
    return data2;
}
int lmk_init()
{
    uint32_t lmk_reg[] = {
        0x800200C0,
        0x800000C0,
        0x00000301,
        0x00000642,
        0x00000303,
        0x00000F04,
        0x00001905,
        0x44040006,
        0x44CC0007,
        0xC0C60008,
        0x55555549,
        0x94C0440A,
        0x33FD300B,
        0x1B0C016C,
        0x2363102D,
        0x1300000E,
        0x8000800F,
        0x01550410,
        0x000000D8,
        0x80010019,
        0x0FA8001A,
        0x1800011B,
        0x0010005C,
        0x0100019D,
        0x0100019E,
        0x0000003f};

    XGpioPs_Config *GpioConfigPtr;
    int i;
    int xStatus;

    GpioConfigPtr = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
    if (GpioConfigPtr == NULL)
        return XST_FAILURE;
    xStatus = XGpioPs_CfgInitialize(&psGpioInstancePtr, GpioConfigPtr, GpioConfigPtr->BaseAddr);
    if (XST_SUCCESS != xStatus)
        return 0;

    XGpioPs_SetDirectionPin(&psGpioInstancePtr, 40, 1);
    XGpioPs_SetOutputEnablePin(&psGpioInstancePtr, 40, 1);
    XGpioPs_WritePin(&psGpioInstancePtr, 40, 1);
    XGpioPs_SetDirectionPin(&psGpioInstancePtr, 42, 1);
    XGpioPs_SetOutputEnablePin(&psGpioInstancePtr, 42, 1);
    XGpioPs_WritePin(&psGpioInstancePtr, 42, 1);
    XGpioPs_SetDirectionPin(&psGpioInstancePtr, 45, 1);
    XGpioPs_SetOutputEnablePin(&psGpioInstancePtr, 45, 1);
    XGpioPs_WritePin(&psGpioInstancePtr, 45, 1);

    XGpioPs_SetDirectionPin(&psGpioInstancePtr, 41, 0);

    for (i = 0; i < sizeof(lmk_reg) / 4; i++)
    {
        lmk_wdata(lmk_reg[i]);
        if(i==0)usleep(1000);
        if ((i <= 6) & (i >= 1))
            lmk_wdata(lmk_reg[i]);
    }
    u32 data1;
    for (i = 0; i <= 0x1f; i++)
    {
        data1 = lmk_read(i) | i;
    }
    lmk_wdata(0x0f05);
    lmk_wdata(0x0f05);
    for (i = 1; i <= 6; i++)
    {
        lmk_wdata(lmk_reg[i]);
        lmk_wdata(lmk_reg[i]);
    }
}

int main()
{
    init_platform();

    print("Hello World\n\r");
    lmk_init();
    cleanup_platform();
    return 0;
}
