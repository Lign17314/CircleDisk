/*
 * bcm_phy.c
 *
 *  Created on: 2021Äê7ÔÂ22ÈÕ
 *      Author: L
 */

#include "xil_types.h"
#include "xil_io.h"
#include "xil_assert.h"
#include "xparameters.h"
#include "stdio.h"
#include "sleep.h"
#include "xparameters.h"
#include "xparameters_ps.h" /* defines XPAR values */
#include "xil_types.h"
#include "xil_assert.h"
#include "xil_io.h"
#include "xil_exception.h"
#include "xpseudo_asm.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "xscugic.h"
#include "xscutimer.h"
#include "xemacps.h" /* defines XEmacPs API */
#include "xil_mmu.h"
#include "bcm.h"
/* Command codes */
#define CMD_GET_PAIR_SWAP 0x8000
#define CMD_SET_PAIR_SWAP 0x8001
#define CMD_GET_1588_ENABLE 0x8004
#define CMD_SET_1588_ENABLE 0x8005
#define CMD_GET_SHORT_REACH_MODE_ENABLE 0x8006
#define CMD_SET_SHORT_REACH_MODE_ENABLE 0x8007
#define CMD_GET_EEE_MODE 0x8008
#define CMD_SET_EEE_MODE 0x8009
#define CMD_GET_EMI_MODE_ENABLE 0x800A
#define CMD_SET_EMI_MODE_ENABLE 0x800B
#define CMD_GET_SUB_LF_RF_STATUS 0x800D
#define CMD_GET_SERDES_KR_MODE_ENABLE 0x800E
#define CMD_SET_SERDES_KR_MODE_ENABLE 0x800F
#define CMD_CLEAR_SUB_LF_RF 0x8010
#define CMD_SET_SUB_LF_RF 0x8011
#define CMD_READ_INDIRECT_GPHY_REG_BITS 0x8014
#define CMD_WRITE_INDIRECT_GPHY_REG_BITS 0x8015
#define CMD_GET_XFI_2P5G_5G_MODE 0x8016
#define CMD_SET_XFI_2P5G_5G_MODE 0x8017
#define CMD_GET_TWO_PAIR_1G_MODE 0x8018
#define CMD_SET_TWO_PAIR_1G_MODE 0x8019
#define CMD_SET_EEE_STATISTICS 0x801A
#define CMD_GET_EEE_STATISTICS 0x801B
#define CMD_SET_JUMBO_PACKET 0x801C
#define CMD_GET_JUMBO_PACKET 0x801D
#define CMD_GET_MSE 0x801E
#define CMD_GET_PAUSE_FRAME_MODE 0x801F
#define CMD_SET_PAUSE_FRAME_MODE 0x8020
#define CMD_GET_LED_TYPE 0x8021
#define CMD_SET_LED_TYPE 0x8022
#define CMD_GET_MGBASE_T_802_3BZ_TYPE 0x8023
#define CMD_SET_MGBASE_T_802_3BZ_TYPE 0x8024
#define CMD_GET_MSE_GPHY 0x8025
#define CMD_SET_USXGMII 0x8026
#define CMD_GET_USXGMII 0x8027
#define CMD_GET_XL_MODE 0x8029
#define CMD_SET_XL_MODE 0x802A
#define CMD_GET_XFI_TX_FILTERS 0x802B
#define CMD_SET_XFI_TX_FILTERS 0x802C
#define CMD_GET_XFI_POLARITY 0x802D
#define CMD_SET_XFI_POLARITY 0x802E
#define CMD_GET_CURRENT_VOLTAGE 0x802F
#define CMD_GET_SNR 0x8030
#define CMD_GET_CURRENT_TEMP 0x8031
#define CMD_SET_UPPER_TEMP_WARNING_LEVEL 0x8032
#define CMD_GET_UPPER_TEMP_WARNING_LEVEL 0x8033
#define CMD_SET_LOWER_TEMP_WARNING_LEVEL 0x8034
#define CMD_GET_LOWER_TEMP_WARNING_LEVEL 0x8035
#define CMD_GET_HW_FR_EMI_MODE_ENABLE 0x803A
#define CMD_SET_HW_FR_EMI_MODE_ENABLE 0x803B
#define CMD_GET_CUSTOMER_REQUESTED_TX_PWR_ADJUST 0x8040
#define CMD_SET_CUSTOMER_REQUESTED_TX_PWR_ADJUST 0x8041
#define CMD_GET_DYNAMIC_PARTITION_SELECT 0x8042
#define CMD_SET_DYNAMIC_PARTITION_SELECT 0x8043
#define CMD_RESET_STAT_LOG 0xC017
#define CMD_RECEIVED 0x0001
#define CMD_IN_PROGRESS 0x0002
#define CMD_COMPLETE_PASS 0x0004
#define CMD_COMPLETE_ERROR 0x0008
#define CMD_SYSTEM_BUSY 0xBBBB

#define PHY_READ(a, b, c, d) XEmacPs_PhyRead_C45(a, 1, (b << 16) | c, d)
#define PHY_WRITE(a, b, c, d) XEmacPs_PhyWrite_C45(a, 1, (b << 16) | c, d)


#define BCM848XX_DOWNLOAD_CTRL_REG 0x1A817
#define BCM848XX_DOWNLOAD_STATUS_REG 0x1A818
#define BCM848XX_DOWNLOAD_ADDR_LOW_REG 0x1A819
#define BCM848XX_DOWNLOAD_ADDR_HIGH_REG 0x1A81A
#define BCM848XX_DOWNLOAD_DATA_LOW_REG 0x1A81B
#define BCM848XX_DOWNLOAD_DATA_HIGH_REG 0x1A81C

#define PHY_BCM8488X_FEATURES (SUPPORTED_10000baseR_FEC | \
                               PHY_BASIC_FEATURES)

#define BCM848XX_EXEC_FW_SUCCESSFULLY_VAL 0x2040

#define BCM848XX_10G_RPMD_CTRL_REG (0x10000)
#define BCM848XX_DEVICE_IDENTIFIER_REG (0x10002)
#define BCM848XX_PMD_RX_SIGNAL_DETECT (0x1000a)

#define BCM848XX_MAILBOX_REG (0x1A008)

#define BCM848XX_1G_100M_MII_CTRL_REG (0x7FFE0)
#define BCM848XX_1G_100M_AUX_STATUS_REG (0x7FFF9)

#define BCM848XX_USER_CONTROL_REG (0x1E4005)
#define BCM848XX_USER_STATUS_REG (0x1E400D)
#define BCM848XX_USER_FW_REV_REG (0x1E400F)
#define BCM848XX_USER_FW_DATE_REG (0x1E4010)

#define BCM848XX_USER_DEFINE_REG_4181 (0x1E4181)
#define BCM848XX_USER_DEFINE_REG_4186 (0x1E4186)
#define BCM848XX_USER_DEFINE_REG_4188 (0x1E4188)
#define BCM848XX_USER_DEFINE_REG_418C (0x1E418C)

#define BCM848XX_CONTROL_REG_8004 (0x1E8004)

#define BCM848XX_10GBASER_PCS_STATUS (0x30020)

#define PHY848XX_MII_R00_CONTROL_RESTART_AUTONEG_SHIFT 9
#define PHY848XX_1G_100M_AUX_HCD_MASK 0x700
#define PHY848XX_USER_STATUS_CRC_CHK_MASK 0xC000
#define PHY848XX_USER_STATUS_CRC_GOOD_VAL 0x4000
#define PHY848XX_USER_STATUS_MAC_LINK_STATUS_MASK 0x2000
#define PHY848XX_USER_STATUS_MAC_LINK_STATUS_SHIFT 13
#define PHY848XX_USER_STATUS_COP_LINK_STATUS_MASK 0x20
#define PHY848XX_USER_STATUS_COP_LINK_STATUS_SHIFT 5
#define PHY848XX_USER_STATUS_COP_SPEED_MASK 0x1c
#define PHY848XX_USER_STATUS_COP_SPEED_SHIFT 2
#define PHY848XX_USER_STATUS_COP_DETECT_MASK 0x2
#define PHY848XX_USER_STATUS_COP_DETECT_SHIFT 1
#define XEMACPS_PHYMNTNC_OP_MASK_C45 0x00020000U	  /**< operation mask bits */
#define XEMACPS_PHYMNTNC_OP_R_MASK_C45 0x30000000U	  /**< read operation */
#define XEMACPS_PHYMNTNC_OP_W_MASK_C45 0x10000000U	  /**< write operation */
#define XEMACPS_PHYMNTNC_OP_ADDR_MASK_C45 0x00000000U /**< write operation */
LONG XEmacPs_PhyRead_C45(XEmacPs *InstancePtr, u32 PhyAddress,
						 u32 RegisterNum, u16 *PhyDataPtr)
{
	u32 Mgtcr;
	volatile u32 Ipisr;
	u32 IpReadTemp;
	LONG Status;

	Xil_AssertNonvoid(InstancePtr != NULL);

	/* Make sure no other PHY operation is currently in progress */
	if ((!(XEmacPs_ReadReg(InstancePtr->Config.BaseAddress,
						   XEMACPS_NWSR_OFFSET) &
		   XEMACPS_NWSR_MDIOIDLE_MASK)) == TRUE)
	{
		Status = (LONG)(XST_EMAC_MII_BUSY);
	}
	else
	{

		/* Construct Mgtcr mask for the operation */
		Mgtcr = XEMACPS_PHYMNTNC_OP_MASK_C45 | XEMACPS_PHYMNTNC_OP_ADDR_MASK_C45 |
				(PhyAddress << XEMACPS_PHYMNTNC_PHAD_SHFT_MSK) |
				(((RegisterNum >> 16) & 0x1f) << XEMACPS_PHYMNTNC_PREG_SHFT_MSK) |
				(RegisterNum & 0xffff);

		/* Write Mgtcr and wait for completion */
		XEmacPs_WriteReg(InstancePtr->Config.BaseAddress,
						 XEMACPS_PHYMNTNC_OFFSET, Mgtcr);

		do
		{
			Ipisr = XEmacPs_ReadReg(InstancePtr->Config.BaseAddress,
									XEMACPS_NWSR_OFFSET);
			IpReadTemp = Ipisr;
		} while ((IpReadTemp & XEMACPS_NWSR_MDIOIDLE_MASK) == 0x00000000U);

		/* Construct Mgtcr mask for the operation */
		Mgtcr = XEMACPS_PHYMNTNC_OP_MASK_C45 | XEMACPS_PHYMNTNC_OP_R_MASK_C45 |
				(PhyAddress << XEMACPS_PHYMNTNC_PHAD_SHFT_MSK) |
				(((RegisterNum >> 16) & 0x1f) << XEMACPS_PHYMNTNC_PREG_SHFT_MSK);

		/* Write Mgtcr and wait for completion */
		XEmacPs_WriteReg(InstancePtr->Config.BaseAddress,
						 XEMACPS_PHYMNTNC_OFFSET, Mgtcr);

		do
		{
			Ipisr = XEmacPs_ReadReg(InstancePtr->Config.BaseAddress,
									XEMACPS_NWSR_OFFSET);
			IpReadTemp = Ipisr;
		} while ((IpReadTemp & XEMACPS_NWSR_MDIOIDLE_MASK) == 0x00000000U);

		/* Read data */
		*PhyDataPtr = (u16)XEmacPs_ReadReg(InstancePtr->Config.BaseAddress,
										   XEMACPS_PHYMNTNC_OFFSET);
		Status = (LONG)(XST_SUCCESS);
	}
	return Status;
}
LONG XEmacPs_PhyWrite_C45(XEmacPs *InstancePtr, u32 PhyAddress,
						  u32 RegisterNum, u16 PhyData)
{
	u32 Mgtcr;
	volatile u32 Ipisr;
	u32 IpWriteTemp;
	LONG Status;

	Xil_AssertNonvoid(InstancePtr != NULL);

	/* Make sure no other PHY operation is currently in progress */
	if ((!(XEmacPs_ReadReg(InstancePtr->Config.BaseAddress,
						   XEMACPS_NWSR_OFFSET) &
		   XEMACPS_NWSR_MDIOIDLE_MASK)) == TRUE)
	{
		Status = (LONG)(XST_EMAC_MII_BUSY);
	}
	else
	{

		/* Construct Mgtcr mask for the operation */
		Mgtcr = XEMACPS_PHYMNTNC_OP_MASK_C45 | XEMACPS_PHYMNTNC_OP_ADDR_MASK_C45 |
				(PhyAddress << XEMACPS_PHYMNTNC_PHAD_SHFT_MSK) |
				(((RegisterNum >> 16) & 0x1f) << XEMACPS_PHYMNTNC_PREG_SHFT_MSK) |
				(RegisterNum & 0xffff);

		/* Write Mgtcr and wait for completion */
		XEmacPs_WriteReg(InstancePtr->Config.BaseAddress,
						 XEMACPS_PHYMNTNC_OFFSET, Mgtcr);

		do
		{
			Ipisr = XEmacPs_ReadReg(InstancePtr->Config.BaseAddress,
									XEMACPS_NWSR_OFFSET);
			IpWriteTemp = Ipisr;
		} while ((IpWriteTemp & XEMACPS_NWSR_MDIOIDLE_MASK) == 0x00000000U);

		/* Construct Mgtcr mask for the operation */
		Mgtcr = XEMACPS_PHYMNTNC_OP_MASK_C45 | XEMACPS_PHYMNTNC_OP_W_MASK_C45 |
				(PhyAddress << XEMACPS_PHYMNTNC_PHAD_SHFT_MSK) |
				(((RegisterNum >> 16) & 0x1f) << XEMACPS_PHYMNTNC_PREG_SHFT_MSK) | (u32)PhyData;

		/* Write Mgtcr and wait for completion */
		XEmacPs_WriteReg(InstancePtr->Config.BaseAddress,
						 XEMACPS_PHYMNTNC_OFFSET, Mgtcr);

		do
		{
			Ipisr = XEmacPs_ReadReg(InstancePtr->Config.BaseAddress,
									XEMACPS_NWSR_OFFSET);
			IpWriteTemp = Ipisr;
		} while ((IpWriteTemp & XEMACPS_NWSR_MDIOIDLE_MASK) == 0x00000000U);

		Status = (LONG)(XST_SUCCESS);
	}
	return Status;
}
static void bcm848XX_write_reg32(XEmacPs *phydev, u32 ad, u32 value)
{
    u16 val = 0;

    val = (u16)(ad & 0xFFFF);
    XEmacPs_PhyWrite_C45(phydev, 0x01, BCM848XX_DOWNLOAD_ADDR_LOW_REG, val);

    val = (u16)(ad >> 16);
    XEmacPs_PhyWrite_C45(phydev, 0x01, BCM848XX_DOWNLOAD_ADDR_HIGH_REG, val);

    val = (u16)(value & 0xFFFF);
    XEmacPs_PhyWrite_C45(phydev, 0x01, BCM848XX_DOWNLOAD_DATA_LOW_REG, val);

    val = (u16)(value >> 16);
    XEmacPs_PhyWrite_C45(phydev, 0x01, BCM848XX_DOWNLOAD_DATA_HIGH_REG, val);

    XEmacPs_PhyWrite_C45(phydev, 0x01, BCM848XX_DOWNLOAD_CTRL_REG, 0x0009);
}
static void bcm848XX_read_reg32(XEmacPs *phydev, u32 ad, u32 *value)
{
    u16 val = 0;

    val = (u16)(ad & 0xFFFF);
    XEmacPs_PhyWrite_C45(phydev, 0x01, BCM848XX_DOWNLOAD_ADDR_LOW_REG, val);

    val = (u16)(ad >> 16);
    XEmacPs_PhyWrite_C45(phydev, 0x01, BCM848XX_DOWNLOAD_ADDR_HIGH_REG, val);

    XEmacPs_PhyWrite_C45(phydev, 0x01, BCM848XX_DOWNLOAD_CTRL_REG, 0x000a);

    //val = (u16)(value & 0xFFFF);
    val = 0;
    XEmacPs_PhyRead_C45(phydev, 0x01, BCM848XX_DOWNLOAD_DATA_LOW_REG, &val);
    *value = (val & 0xFFFF);

    //val = (u16)(value >> 16);
    XEmacPs_PhyRead_C45(phydev, 0x01, BCM848XX_DOWNLOAD_DATA_HIGH_REG, &val);
    *value = *value | (val << 16);
}

static int _wait_for_cmd_ready(XEmacPs *phy_dev)
{
    int i;
    uint16_t val;

    for (i = 0; i < 1000; i++)
    {
        /* Read status of command */
        PHY_READ(phy_dev, 0x1e, 0x4037, &val);

        if (val != CMD_IN_PROGRESS && val != CMD_SYSTEM_BUSY)
            return 0;

        usleep(2000);
    }

    xil_printf("Timed out waiting for command ready");
}

static uint16_t _wait_for_cmd_complete(XEmacPs *phy_dev)
{
    int i;
    uint16_t val = 0;

    for (i = 0; i < 1000; i++)
    {
        /* Read status of command */
        PHY_READ(phy_dev, 0x1e, 0x4037, &val);

        if (val == CMD_COMPLETE_PASS || val == CMD_COMPLETE_ERROR)
            goto Exit;

        usleep(2000);
    }

    xil_printf("Timed out waiting for command complete\n");

Exit:
    return val;
}

static int cmd_handler(XEmacPs *phy_dev, uint16_t cmd_code, uint16_t *data1, uint16_t *data2, uint16_t *data3, uint16_t *data4, uint16_t *data5)
{
    int ret;
    uint16_t cmd_status = 0;

    /* Make sure command interface is open */
    if ((ret = _wait_for_cmd_ready(phy_dev)))
        goto Exit;

    switch (cmd_code)
    {
    case CMD_SET_PAIR_SWAP:
    case CMD_SET_1588_ENABLE:
    case CMD_SET_SHORT_REACH_MODE_ENABLE:
    case CMD_SET_EEE_MODE:
    case CMD_SET_EMI_MODE_ENABLE:
    case CMD_SET_SERDES_KR_MODE_ENABLE:
    case CMD_CLEAR_SUB_LF_RF:
    case CMD_SET_SUB_LF_RF:
    case CMD_WRITE_INDIRECT_GPHY_REG_BITS:
    case CMD_SET_XFI_2P5G_5G_MODE:
    case CMD_SET_TWO_PAIR_1G_MODE:
    case CMD_SET_PAUSE_FRAME_MODE:
    case CMD_SET_LED_TYPE:
    case CMD_SET_MGBASE_T_802_3BZ_TYPE:
    case CMD_SET_USXGMII:
    case CMD_SET_EEE_STATISTICS:
    case CMD_SET_JUMBO_PACKET:
    case CMD_SET_XL_MODE:
    case CMD_SET_XFI_TX_FILTERS:
    case CMD_SET_XFI_POLARITY:
    case CMD_SET_UPPER_TEMP_WARNING_LEVEL:
    case CMD_SET_LOWER_TEMP_WARNING_LEVEL:
    case CMD_SET_HW_FR_EMI_MODE_ENABLE:
    case CMD_SET_CUSTOMER_REQUESTED_TX_PWR_ADJUST:
    case CMD_SET_DYNAMIC_PARTITION_SELECT:
    {
        if (data1)
            PHY_WRITE(phy_dev, 0x1e, 0x4038, *data1);
        if (data2)
            PHY_WRITE(phy_dev, 0x1e, 0x4039, *data2);
        if (data3)
            PHY_WRITE(phy_dev, 0x1e, 0x403a, *data3);
        if (data4)
            PHY_WRITE(phy_dev, 0x1e, 0x403b, *data4);
        if (data5)
            PHY_WRITE(phy_dev, 0x1e, 0x403c, *data5);

        PHY_WRITE(phy_dev, 0x1e, 0x4005, cmd_code);
        cmd_status = _wait_for_cmd_complete(phy_dev);

        break;
    }
    case CMD_GET_PAIR_SWAP:
    case CMD_GET_1588_ENABLE:
    case CMD_GET_SHORT_REACH_MODE_ENABLE:
    case CMD_GET_EEE_MODE:
    case CMD_GET_EMI_MODE_ENABLE:
    case CMD_GET_SERDES_KR_MODE_ENABLE:
    case CMD_GET_SUB_LF_RF_STATUS:
    case CMD_GET_XFI_2P5G_5G_MODE:
    case CMD_GET_TWO_PAIR_1G_MODE:
    case CMD_GET_PAUSE_FRAME_MODE:
    case CMD_GET_LED_TYPE:
    case CMD_GET_MGBASE_T_802_3BZ_TYPE:
    case CMD_GET_MSE_GPHY:
    case CMD_GET_USXGMII:
    case CMD_GET_JUMBO_PACKET:
    case CMD_GET_MSE:
    case CMD_GET_XL_MODE:
    case CMD_GET_XFI_TX_FILTERS:
    case CMD_GET_XFI_POLARITY:
    case CMD_GET_CURRENT_VOLTAGE:
    case CMD_GET_SNR:
    case CMD_GET_CURRENT_TEMP:
    case CMD_GET_UPPER_TEMP_WARNING_LEVEL:
    case CMD_GET_LOWER_TEMP_WARNING_LEVEL:
    case CMD_GET_HW_FR_EMI_MODE_ENABLE:
    case CMD_GET_CUSTOMER_REQUESTED_TX_PWR_ADJUST:
    case CMD_GET_DYNAMIC_PARTITION_SELECT:
    case CMD_RESET_STAT_LOG:
    {
        PHY_WRITE(phy_dev, 0x1e, 0x4005, cmd_code);
        cmd_status = _wait_for_cmd_complete(phy_dev);

        if (data1)
            PHY_READ(phy_dev, 0x1e, 0x4038, data1);
        if (data2)
            PHY_READ(phy_dev, 0x1e, 0x4039, data2);
        if (data3)
            PHY_READ(phy_dev, 0x1e, 0x403a, data3);
        if (data4)
            PHY_READ(phy_dev, 0x1e, 0x403b, data4);
        if (data5)
            PHY_READ(phy_dev, 0x1e, 0x403c, data5);

        break;
    }
    case CMD_READ_INDIRECT_GPHY_REG_BITS:
    {
        if (data1)
            PHY_WRITE(phy_dev, 0x1e, 0x4038, *data1);
        if (data2)
            PHY_WRITE(phy_dev, 0x1e, 0x4039, *data2);
        if (data3)
            PHY_WRITE(phy_dev, 0x1e, 0x403a, *data3);
        if (data4)
            PHY_WRITE(phy_dev, 0x1e, 0x403b, *data4);

        PHY_WRITE(phy_dev, 0x1e, 0x4005, cmd_code);
        cmd_status = _wait_for_cmd_complete(phy_dev);

        if (data5)
            PHY_READ(phy_dev, 0x1e, 0x403c, data5);

        break;
    }
    case CMD_GET_EEE_STATISTICS:
    {
        if (data1)
            PHY_WRITE(phy_dev, 0x1e, 0x4038, *data1);

        PHY_WRITE(phy_dev, 0x1e, 0x4005, cmd_code);
        cmd_status = _wait_for_cmd_complete(phy_dev);

        if (data2)
            PHY_READ(phy_dev, 0x1e, 0x4039, data2);
        if (data3)
            PHY_READ(phy_dev, 0x1e, 0x403a, data3);
        if (data4)
            PHY_READ(phy_dev, 0x1e, 0x403b, data4);
        if (data5)
            PHY_READ(phy_dev, 0x1e, 0x403c, data5);

        break;
    }
    default:
        xil_printf("Unsupported cmd code: 0x%x\n", cmd_code);
        break;
    }

    if (cmd_status != CMD_COMPLETE_PASS)
    {
        xil_printf("Failed to execute cmd code: 0x%x\n", cmd_code);
        return -1;
    }

Exit:
    return ret;
}

static int _phy_power_set(XEmacPs *phy_dev, int enable)
{
    uint16_t val;
    int ret;

    PHY_READ(phy_dev, 0x07, 0xffe0, &val);

    if (enable)
        val &= ~(1 << 11); /* Power up */
    else
        val |= (1 << 11); /* Power down */

    PHY_WRITE(phy_dev, 0x07, 0xffe0, val);

Exit:
    return ret;
}

static int bcm848XX_soft_reset(XEmacPs *phydev)
{
    int timeout = 1000; /* wait for 1000ms */
    u16 val;

    XEmacPs_PhyWrite_C45(phydev, 0x01, BCM848XX_10G_RPMD_CTRL_REG, 0x8000);
    do
    {
        XEmacPs_PhyRead_C45(phydev, 0x01, BCM848XX_10G_RPMD_CTRL_REG, &val);
        if (val == BCM848XX_EXEC_FW_SUCCESSFULLY_VAL)
            break;

        usleep(1000);
    } while (timeout--);

    if (timeout == 0)
    {
        xil_printf("Verify PHY status timeout!\n");
        return 0;
    }

    return 0;
}
static int load_848xx(XEmacPs *phy)
{
    int i, cnt, step, ret;
    uint32_t phy_map, *firmware_data, firmware_size;

    // xil_printf("Firmware version: %s\n", phy->firmware->version);

    // phy_map = phy->map;
    //firmware_data = phy->firmware->data;
    firmware_size = sizeof(bcm8486x_a0_firmware);

    cnt = firmware_size / sizeof(uint32_t);
    step = cnt / 100;

    /* Download firmware with broadcast mode to up to 32 phys, according to the phy map */
    xil_printf("Loading firmware into phys\r\n");

    /* 1. Turn on broadcast mode to accept write operations for addr = 0 */
    xil_printf("Turn on broadcast mode to accept write operations\r\n");

    PHY_WRITE(phy, 0x1e, 0x4117, 0xf003);
    PHY_WRITE(phy, 0x1e, 0x4107, 0x0401);

    /* 2. Halt the BCM848XX processors operation */
    xil_printf("Halt the phys processors operation\r\n");

    PHY_WRITE(phy, 0x1e, 0x4188, 0x48f0);
    PHY_WRITE(phy, 0x1e, 0x4186, 0x8000);
    PHY_WRITE(phy, 0x1e, 0x4181, 0x017c);
    PHY_WRITE(phy, 0x1e, 0x4181, 0x0040);

    PHY_WRITE(phy, 0x01, 0xa819, 0x0000);
    PHY_WRITE(phy, 0x01, 0xa81a, 0xc300);
    PHY_WRITE(phy, 0x01, 0xa81b, 0x0010);
    PHY_WRITE(phy, 0x01, 0xa81c, 0x0000);
    PHY_WRITE(phy, 0x01, 0xa817, 0x0009);

    PHY_WRITE(phy, 0x01, 0xa819, 0x0000);
    PHY_WRITE(phy, 0x01, 0xa81a, 0xffff);
    PHY_WRITE(phy, 0x01, 0xa81b, 0x1018);
    PHY_WRITE(phy, 0x01, 0xa81c, 0xe59f);
    PHY_WRITE(phy, 0x01, 0xa817, 0x0009);

    PHY_WRITE(phy, 0x01, 0xa819, 0x0004);
    PHY_WRITE(phy, 0x01, 0xa81a, 0xffff);
    PHY_WRITE(phy, 0x01, 0xa81b, 0x1f11);
    PHY_WRITE(phy, 0x01, 0xa81c, 0xee09);
    PHY_WRITE(phy, 0x01, 0xa817, 0x0009);

    PHY_WRITE(phy, 0x01, 0xa819, 0x0008);
    PHY_WRITE(phy, 0x01, 0xa81a, 0xffff);
    PHY_WRITE(phy, 0x01, 0xa81b, 0x0000);
    PHY_WRITE(phy, 0x01, 0xa81c, 0xe3a0);
    PHY_WRITE(phy, 0x01, 0xa817, 0x0009);

    PHY_WRITE(phy, 0x01, 0xa819, 0x000c);
    PHY_WRITE(phy, 0x01, 0xa81a, 0xffff);
    PHY_WRITE(phy, 0x01, 0xa81b, 0x1806);
    PHY_WRITE(phy, 0x01, 0xa81c, 0xe3a0);
    PHY_WRITE(phy, 0x01, 0xa817, 0x0009);

    PHY_WRITE(phy, 0x01, 0xa819, 0x0010);
    PHY_WRITE(phy, 0x01, 0xa81a, 0xffff);
    PHY_WRITE(phy, 0x01, 0xa81b, 0x0002);
    PHY_WRITE(phy, 0x01, 0xa81c, 0xe8a0);
    PHY_WRITE(phy, 0x01, 0xa817, 0x0009);

    PHY_WRITE(phy, 0x01, 0xa819, 0x0014);
    PHY_WRITE(phy, 0x01, 0xa81a, 0xffff);
    PHY_WRITE(phy, 0x01, 0xa81b, 0x0001);
    PHY_WRITE(phy, 0x01, 0xa81c, 0xe150);
    PHY_WRITE(phy, 0x01, 0xa817, 0x0009);

    PHY_WRITE(phy, 0x01, 0xa819, 0x0018);
    PHY_WRITE(phy, 0x01, 0xa81a, 0xffff);
    PHY_WRITE(phy, 0x01, 0xa81b, 0xfffc);
    PHY_WRITE(phy, 0x01, 0xa81c, 0x3aff);
    PHY_WRITE(phy, 0x01, 0xa817, 0x0009);

    PHY_WRITE(phy, 0x01, 0xa819, 0x001c);
    PHY_WRITE(phy, 0x01, 0xa81a, 0xffff);
    PHY_WRITE(phy, 0x01, 0xa81b, 0xfffe);
    PHY_WRITE(phy, 0x01, 0xa81c, 0xeaff);
    PHY_WRITE(phy, 0x01, 0xa817, 0x0009);

    PHY_WRITE(phy, 0x01, 0xa819, 0x0020);
    PHY_WRITE(phy, 0x01, 0xa81a, 0xffff);
    PHY_WRITE(phy, 0x01, 0xa81b, 0x0021);
    PHY_WRITE(phy, 0x01, 0xa81c, 0x0004);
    PHY_WRITE(phy, 0x01, 0xa817, 0x0009);

    PHY_WRITE(phy, 0x1e, 0x4181, 0x0000);

    /* 3. Upload the firmware into the on-chip memory of the devices */
    xil_printf("Upload the firmware into the on-chip memory\r\n");

    PHY_WRITE(phy, 0x01, 0xa81a, 0x0000);
    PHY_WRITE(phy, 0x01, 0xa819, 0x0000);
    PHY_WRITE(phy, 0x01, 0xa817, 0x0038);

    for (i = 0; i < cnt; i++)
    {
        PHY_WRITE(phy, 0x01, 0xa81c, bcm8486x_a0_firmware[i] >> 16);    /* upper 16 bits */
        PHY_WRITE(phy, 0x01, 0xa81b, bcm8486x_a0_firmware[i] & 0xffff); /* lower 16 bits */

        if (i == i / step * step)
            xil_printf("\r%d%%", i / step);
    }
    xil_printf("\r\n");

    PHY_WRITE(phy, 0x01, 0xa817, 0x0000);
    PHY_WRITE(phy, 0x01, 0xa819, 0x0000);
    PHY_WRITE(phy, 0x01, 0xa81a, 0xc300);
    PHY_WRITE(phy, 0x01, 0xa81b, 0x0000);
    PHY_WRITE(phy, 0x01, 0xa81c, 0x0000);
    PHY_WRITE(phy, 0x01, 0xa817, 0x0009);

    /* 4. Reset the processors to start execution of the code in the on-chip memory */
    xil_printf("Reset the processors to start execution of the code in the on-chip memory\r\n");

    PHY_WRITE(phy, 0x01, 0xa008, 0x0000);
    PHY_WRITE(phy, 0x1e, 0x8004, 0x5555);
    PHY_WRITE(phy, 0x01, 0x0000, 0x8000);

    /* 5. Verify that the processors are running */
    xil_printf("Verify that the processors are running: ");

    i = 1000;
    u16 rc;
    do
    {
        usleep(2000);
        PHY_READ(phy, 0x01, 0x0, &rc);
        if ((rc & 0xffff) == 0x2040)
            break;
    } while (i--);

    xil_printf("%s\r\n", i == 0 ? "Failed" : "OK");

    if (i == 0)
        goto Exit;
    /* 6. Verify that the firmware has been loaded into the on-chip memory with a good CRC */
    xil_printf("Verify that the firmware has been loaded with good CRC: ");

    i = 1000;
    do
    {
        usleep(2000);
        PHY_READ(phy, 0x1e, 0x400d, &rc);
        if ((rc & 0xc000) == 0x4000)
            break;
        // ret  = _bus_read_all(phy_map, 0x1e, 0x400d, 0x4000, 0xc000);
    } while (i--);

    xil_printf("%s\r\n", i == 0 ? "Failed" : "OK");

    if (i == 0)
        goto Exit;
    xil_printf("Firmware loading completed successfully\r\n");

Exit:
    return ret;
}
static int _phy_set_mode(XEmacPs *phy_dev, int line_side)
{
    uint16_t val1, val2, val3;

    val1 = line_side ? 0x0001 : 0x2004;
    val2 = line_side ? 0x0001 : 0x2004;
    val3 = line_side ? 0x1002 : 0x2004;

    /* Set base-pointer mode */
    PHY_WRITE(phy_dev, 0x1e, 0x4110, val1);
    PHY_WRITE(phy_dev, 0x1e, 0x4111, val2);
    PHY_WRITE(phy_dev, 0x1e, 0x4113, val3);
    return 0;
}
#define phy_dev_t XEmacPs
static int _phy_idle_stuffing_mode_set(phy_dev_t *phy_dev, int enable)
{
    int ret;
    uint16_t val = enable ? 0 : 1;

    /* Set idle stuffing mode in 2.5GBASE-T and 5GBASE-T modes */
    cmd_handler(phy_dev, CMD_SET_XFI_2P5G_5G_MODE, &val, &val, NULL, NULL, NULL);

Exit:
    return ret;
}
static int _phy_force_auto_mdix_set(phy_dev_t *phy_dev, int enable)
{
    int ret;
    uint16_t val;

    PHY_READ(phy_dev, 0x07, 0x902f, &val);

    if (enable)
        val |= (1 << 9); /* Auto-MDIX enabled */
    else
        val &= ~(1 << 9); /* Auto-MDIX disabled */

    PHY_WRITE(phy_dev, 0x07, 0x902f, val);

Exit:
    return ret;
}

static int _phy_eth_wirespeed_set(phy_dev_t *phy_dev, int enable)
{
    int ret;
    uint16_t val;

    PHY_READ(phy_dev, 0x07, 0x902f, &val);

    if (enable)
        val |= (1 << 4); /* Ethernet@Wirespeed enabled */
    else
        val &= ~(1 << 4); /* Ethernet@Wirespeed disabled */

    PHY_WRITE(phy_dev, 0x07, 0x902f, val);

Exit:
    return ret;
}

static int _phy_pair_swap_set(phy_dev_t *phy_dev, int enable)
{
    int ret;
    uint16_t data;

    if (enable)
        data = 0x1b;
    else
        data = 0xe4;

    cmd_handler(phy_dev, CMD_SET_PAIR_SWAP, NULL, &data, NULL, NULL, NULL);

Exit:
    return ret;
}
static int _phy_xfi_polarity_set(phy_dev_t *phy_dev, int enable)
{
    int ret;
    uint16_t data = enable ? 1 : 0;
    uint16_t type = 0;

    if ((ret = cmd_handler(phy_dev, CMD_SET_XFI_POLARITY, &type, &data, &data, NULL, NULL)))
        goto Exit;

Exit:
    return ret;
}
static int _phy_apd_get(phy_dev_t *phy_dev, int *enable)
{
    int ret;
    uint16_t val;

    PHY_READ(phy_dev, 0x07, 0x901a, &val);
    *enable = val & (1 << 5) ? 1 : 0; /* Auto power-down mode enabled */

Exit:
    return ret;
}

static int _phy_apd_set(phy_dev_t *phy_dev, int enable)
{
    int ret;
    uint16_t val;

    /* Auto-Power Down */
    PHY_READ(phy_dev, 0x07, 0x901a, &val);

    if (enable)
    {
        val |= (1 << 5); /* Auto power-down mode enabled */
        val |= (1 << 8); /* Enable energy detect single link pulse */
    }
    else
    {
        val &= ~(1 << 5); /* Auto power-down mode disabled */
        val &= ~(1 << 8); /* Disable energy detect single link pulse */
    }

    PHY_WRITE(phy_dev, 0x07, 0x901a, val);

    /* Reserved Control 3 */
    PHY_READ(phy_dev, 0x07, 0x9015, &val);

    if (enable)
    {
        val |= (1 << 0);  /* Disable CLK125 output */
        val &= ~(1 << 1); /* Enable powering down of dll during auto-power down */
    }
    else
    {
        val &= ~(1 << 0); /* Enable CLK125 output */
        val |= (1 << 1);  /* Disable powering down of dll during auto-power down */
    }

    PHY_WRITE(phy_dev, 0x07, 0x9015, val);

Exit:
    return ret;
}

#define PHY_CAP_10_HALF (1 << 0)
#define PHY_CAP_10_FULL (1 << 1)
#define PHY_CAP_100_HALF (1 << 2)
#define PHY_CAP_100_FULL (1 << 3)
#define PHY_CAP_1000_HALF (1 << 4)
#define PHY_CAP_1000_FULL (1 << 5)
#define PHY_CAP_2500 (1 << 6)
#define PHY_CAP_5000 (1 << 7)
#define PHY_CAP_10000 (1 << 8)
#define PHY_CAP_AUTONEG (1 << 9)
#define PHY_CAP_PAUSE (1 << 10)
#define PHY_CAP_PAUSE_ASYM (1 << 11)
#define PHY_CAP_REPEATER (1 << 12)
typedef enum
{
    PHY_SPEED_UNKNOWN,
    PHY_SPEED_AUTO = PHY_SPEED_UNKNOWN,
    PHY_SPEED_10,
    PHY_SPEED_100,
    PHY_SPEED_1000,
    PHY_SPEED_2500,
    PHY_SPEED_5000,
    PHY_SPEED_10000,
} phy_speed_t;

typedef enum
{
    PHY_DUPLEX_UNKNOWN,
    PHY_DUPLEX_HALF,
    PHY_DUPLEX_FULL,
} phy_duplex_t;

static int _phy_eee_mode_get(phy_dev_t *phy_dev, uint16_t *mode)
{
    int ret;
    uint16_t data1, data2, data3, data4;

    /* Get EEE mode */
    if ((ret = cmd_handler(phy_dev, CMD_GET_EEE_MODE, &data1, &data2, &data3, &data4, NULL)))
        goto Exit;

    *mode = data1;

Exit:
    return ret;
}

static int _phy_eee_mode_set(phy_dev_t *phy_dev, uint32_t caps)
{
    int ret;
    uint16_t val, data1, data2, data3, data4;
    uint8_t mode;

    val = 0;
    mode = 1; /* Native EEE */

    val |= ((caps & PHY_CAP_100_HALF) || (caps & PHY_CAP_100_FULL)) ? (mode << 2) : 0;
    val |= ((caps & PHY_CAP_1000_HALF) || (caps & PHY_CAP_1000_FULL)) ? (mode << 2) : 0;
    val |= ((caps & PHY_CAP_2500) ? (mode << 4) : 0);
    val |= ((caps & PHY_CAP_5000) ? (mode << 6) : 0);
    val |= ((caps & PHY_CAP_10000)) ? (mode << 0) : 0;

    data1 = val;
    data2 = 0;
    data3 = 0;
    data4 = 0;

    /* Set EEE mode */
    if ((ret = cmd_handler(phy_dev, CMD_SET_EEE_MODE, &data1, &data2, &data3, &data4, NULL)))
        goto Exit;

Exit:
    return ret;
}
#define PHY_FLAG_NOT_PRESENTED (1 << 0) /* for SFP module indicating not inserted */
#define PHY_FLAG_POWER_SET_ENABLED (1 << 1)
#define PHY_FLAG_DYNAMIC (1 << 2)

#define CAPS_TYPE_ADVERTISE 0
#define CAPS_TYPE_SUPPORTED 1
#define CAPS_TYPE_LP_ADVERTISED 2
int _phy_caps_set(phy_dev_t *phy_dev, uint32_t caps)
{
    int ret;
    uint16_t val, mode;
    caps &= ~(PHY_CAP_2500);
    caps &= ~(PHY_CAP_100_HALF | PHY_CAP_1000_HALF);

    /* Copper auto-negotiation advertisement */
    PHY_READ(phy_dev, 0x07, 0xffe4, &val);

    val &= ~((1 << 7) | (1 << 8) | (1 << 10));

    if (caps & PHY_CAP_100_HALF)
        val |= (1 << 7);

    if (caps & PHY_CAP_100_FULL)
        val |= (1 << 8);

    if (caps & PHY_CAP_PAUSE)
        val |= (1 << 10);

    if (caps & PHY_CAP_PAUSE_ASYM)
        val |= (1 << 11);

    PHY_WRITE(phy_dev, 0x07, 0xffe4, val);

    /* 1000Base-T control */
    PHY_READ(phy_dev, 0x07, 0xffe9, &val);

    val &= ~((1 << 8) | (1 << 9) | (1 << 10));

    if (caps & PHY_CAP_1000_HALF)
        val |= (1 << 8);

    if (caps & PHY_CAP_1000_FULL)
        val |= (1 << 9);

    if (caps & PHY_CAP_REPEATER)
        val |= (1 << 10);

    PHY_WRITE(phy_dev, 0x07, 0xffe9, val);

    /* 10GBase-T AN control */
    PHY_READ(phy_dev, 0x07, 0x0020, &val);

    val &= ~((1 << 7) | (1 << 8) | (1 << 12));

    if (caps & PHY_CAP_2500)
        val |= (1 << 7);

    if (caps & PHY_CAP_5000)
        val |= (1 << 8);

    if (caps & PHY_CAP_10000)
        val |= (1 << 12);

    PHY_WRITE(phy_dev, 0x07, 0x0020, val);

    /* 1000Base-T/100Base-TX MII control */
    PHY_READ(phy_dev, 0x07, 0xffe0, &val);

    val &= ~(1 << 12);

    if (caps & PHY_CAP_AUTONEG)
        val |= (1 << 12);

    PHY_WRITE(phy_dev, 0x07, 0xffe0, val);

    if (!(caps & PHY_CAP_AUTONEG))
        goto Exit;

    /* Check if EEE mode is configured */
    if ((ret = _phy_eee_mode_get(phy_dev, &mode)))
        goto Exit;

    /* Reset the EEE mode according to the phy capabilites, if it was set before */
    if (mode && (ret = _phy_eee_mode_set(phy_dev, caps)))
        goto Exit;

    /* Restart auto negotiation */
    val |= (1 << 9);
    PHY_WRITE(phy_dev, 0x07, 0xffe0, val);

Exit:
    return ret;
}

int _phy_caps_get(phy_dev_t *phy_dev, int caps_type, uint32_t *pcaps)
{
    int ret = 0;
    uint16_t val = 0;
    uint32_t caps = 0;

    if (caps_type != CAPS_TYPE_ADVERTISE)
        goto Exit;

    /* 1000Base-T/100Base-TX MII control */
    PHY_READ(phy_dev, 0x07, 0xffe0, &val);

    if (val & (1 << 12))
        caps |= PHY_CAP_AUTONEG;

    /* Copper auto-negotiation advertisement */
    PHY_READ(phy_dev, 0x07, 0xffe4, &val);

    if (val & (1 << 10))
        caps |= PHY_CAP_PAUSE;

    if (val & (1 << 11))
        caps |= PHY_CAP_PAUSE_ASYM;

    if (val & (1 << 7))
        caps |= PHY_CAP_100_HALF;

    if (val & (1 << 8))
        caps |= PHY_CAP_100_FULL;

    /* 1000Base-T control */
    PHY_READ(phy_dev, 0x07, 0xffe9, &val);

    if (val & (1 << 8))
        caps |= PHY_CAP_1000_HALF;

    if (val & (1 << 9))
        caps |= PHY_CAP_1000_FULL;

    if (val & (1 << 10))
        caps |= PHY_CAP_REPEATER;

    /* 10GBase-T AN control */
    PHY_READ(phy_dev, 0x07, 0x0020, &val);

    if (val & (1 << 7))
        caps |= PHY_CAP_2500;

    if (val & (1 << 8))
        caps |= PHY_CAP_5000;

    if (val & (1 << 12))
        caps |= PHY_CAP_10000;

    *pcaps = caps;
Exit:
    return ret;
}
int _phy_caps_set(phy_dev_t *phy_dev, uint32_t caps);
static int _phy_speed_set(phy_dev_t *phy_dev, phy_speed_t speed, phy_duplex_t duplex)
{
    int ret;
    uint32_t caps;

    if (speed == PHY_SPEED_UNKNOWN)
    {
        speed = PHY_SPEED_10000;
        duplex = PHY_DUPLEX_FULL;
    }

    if ((ret = _phy_caps_get(phy_dev, CAPS_TYPE_ADVERTISE, &caps)))
        goto Exit;

    caps &= ~(PHY_CAP_100_HALF | PHY_CAP_100_FULL |
              PHY_CAP_1000_HALF | PHY_CAP_1000_FULL |
              PHY_CAP_2500 | PHY_CAP_5000 | PHY_CAP_10000);

    caps |= PHY_CAP_AUTONEG;

    switch (speed)
    {
    case PHY_SPEED_10000:
        caps |= PHY_CAP_10000;
    case PHY_SPEED_5000:
        caps |= PHY_CAP_5000;
    case PHY_SPEED_2500:
        caps |= PHY_CAP_2500;
    case PHY_SPEED_1000:
        caps |= PHY_CAP_1000_HALF | ((duplex == PHY_DUPLEX_FULL) ? PHY_CAP_1000_FULL : 0);
    case PHY_SPEED_100:
        caps |= PHY_CAP_100_HALF | ((duplex == PHY_DUPLEX_FULL) ? PHY_CAP_100_FULL : 0);
        break;
    default:
        xil_printf("Ignoring unsupported speed\n");
        goto Exit;
        break;
    }

    if ((ret = _phy_caps_set(phy_dev, caps)))
        goto Exit;

Exit:
    return ret;
}
static int _phy_eee_resolution_get(phy_dev_t *phy_dev, int *enable)
{
    int ret;
    uint16_t val;

    /* EEE Resolution Status */
    PHY_READ(phy_dev, 7, 0x803e, &val);

    /* Check if the link partner auto-negotiated EEE capability */
    *enable = val & 0x3e ? 1 : 0;

Exit:
    return ret;
};
static int bcm848XX_config_aneg(XEmacPs  *phydev)
{
	int timeout = 1000; /* wait for 1000ms */
	u16 val = 0;
	u16 chk_restart;

    //phy_read(phydev, BCM848XX_1G_100M_MII_CTRL_REG,);
    PHY_READ(phydev,0x7,0xFFE0,&val);
	val |= (1 << PHY848XX_MII_R00_CONTROL_RESTART_AUTONEG_SHIFT);
	//phy_write(phydev, BCM848XX_1G_100M_MII_CTRL_REG, val);
    PHY_WRITE(phydev,0x7,0xFFE0,val);
	do {
		//val = phy_read(phydev, BCM848XX_1G_100M_MII_CTRL_REG);
        PHY_READ(phydev,0x7,0xFFE0,&val);
		chk_restart = val &
			(1 << PHY848XX_MII_R00_CONTROL_RESTART_AUTONEG_SHIFT);
		if (chk_restart == 0)
			break;
		usleep(500);
	} while (timeout--);

	if (timeout <= 0) {
		xil_printf( "AutoNeg timeout!\n");
		return 0;
	}

	return 0;
}
void bcm_init(XEmacPs *phy_dev)
{
    u16 val;
    int ret, i;
    
    
    load_848xx(phy_dev);
/*
    _phy_set_mode(phy_dev, 1);
    _phy_idle_stuffing_mode_set(phy_dev, 0);
    _phy_force_auto_mdix_set(phy_dev, 1);
   _phy_eth_wirespeed_set(phy_dev, 1);
    _phy_pair_swap_set(phy_dev, 1);
   _phy_xfi_polarity_set(phy_dev, 0);
     _phy_caps_set(phy_dev, PHY_CAP_PAUSE | PHY_CAP_REPEATER);

     _phy_apd_set(phy_dev,0);*/
    if ((ret = _phy_speed_set(phy_dev, PHY_SPEED_1000, PHY_DUPLEX_FULL)))
        goto Exit;

    bcm848XX_config_aneg(phy_dev);
   // _phy_eee_resolution_get(phy_dev,&val);
    xil_printf("eee :%d\r\n",val);
    PHY_READ(phy_dev, 0x1e, 0x400d, &val);
    /* Copper link status */
    xil_printf("link :%d %x\r\n", ((val >> 5) & 0x1),val);

Exit:
    return ret;
}
#define EMACPS_DEVICE_ID XPAR_XEMACPS_0_DEVICE_ID
#define INTC_DEVICE_ID XPAR_SCUGIC_SINGLE_DEVICE_ID
#define TIMER_DEVICE_ID XPAR_SCUTIMER_DEVICE_ID
#define EMACPS_IRPT_INTR XPS_GEM0_INT_ID
#define TIMER_IRPT_INTR XPAR_SCUTIMER_INTR
#define TIMER_LOAD_VALUE XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 4
XEmacPs Mac;
u8 UnicastMAC[] = {0x00, 0x0A, 0x35, 0x01, 0x02, 0x09};
//static XScuGic IntcInstance;	/* The instance of the SCUGic Driver */
static XScuTimer TimerInstance; /* The instance of the SCUTimer Driver */

int XEmacPs_InitScuTimer(void)
{
	int Status = XST_SUCCESS;
	XScuTimer_Config *ConfigPtr;

	/*
	 * Get the configuration of Timer hardware.
	 */
	ConfigPtr = XScuTimer_LookupConfig(TIMER_DEVICE_ID);

	/*
	 * Initialize ScuTimer hardware.
	 */
	Status = XScuTimer_CfgInitialize(&TimerInstance, ConfigPtr,
									 ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	XScuTimer_EnableAutoReload(&TimerInstance);

	/*
	 * Initialize ScuTimer with a count so that the interrupt
	 * comes every 500 msec.
	 */
	XScuTimer_LoadTimer(&TimerInstance, TIMER_LOAD_VALUE);
	return XST_SUCCESS;
}
void bcm_cfg(void)
{
    XEmacPs_Config *Cfg;
	int Status = XST_SUCCESS;
	XEmacPs *EmacPsInstancePtr = &Mac;
	//unsigned int NSIncrementVal;

    //xil_printf("Entering into main() \r\n");

	Xil_SetTlbAttributes(0x0FF00000, 0xc02); // addr, attr

	/* Initialize SCUTIMER */

	if (XEmacPs_InitScuTimer() != XST_SUCCESS)
		while (1)
			;

	/*
	 * Get the configuration of EmacPs hardware.
	 */
	Cfg = XEmacPs_LookupConfig(EMACPS_DEVICE_ID);

	/*
	 * Initialize EmacPs hardware.
	 */
	Status = XEmacPs_CfgInitialize(EmacPsInstancePtr, Cfg,
								   Cfg->BaseAddress);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	/*
	 * Set the MAC address
	 */
	Status = XEmacPs_SetMacAddress(EmacPsInstancePtr,
								   (unsigned char *)UnicastMAC, 1);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	XEmacPs_SetMdioDivisor(EmacPsInstancePtr, MDC_DIV_224);
	/*
	 * Detect and initialize the PHY
	 */
    bcm_init(EmacPsInstancePtr);
}
