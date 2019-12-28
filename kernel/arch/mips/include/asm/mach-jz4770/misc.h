/*
 *  linux/include/asm-mips/mach-jz4770/misc.h
 *
 *  Ingenic's JZ4770 common include.
 *
 *  Copyright (C) 2008 Ingenic Semiconductor Inc.
 *
 *  Author: <cwjia@ingenic.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_JZ4770_MISC_H__
#define __ASM_JZ4770_MISC_H__

/*==========================================================
 * I2C
 *===========================================================*/

#define I2C_EEPROM_DEV	0xA	/* b'1010 */
#define I2C_RTC_DEV	0xD	/* b'1101 */
#define DIMM0_SPD_ADDR	0
#define DIMM1_SPD_ADDR	1
#define DIMM2_SPD_ADDR	2
#define DIMM3_SPD_ADDR	3
#define JZ_HCI_ADDR	7

#define DIMM_SPD_LEN	128
#define JZ_HCI_LEN	512    /* 4K bits E2PROM */
#define I2C_RTC_LEN	16
#define HCI_MAC_OFFSET	64

extern void i2c_open(void);
extern void i2c_close(void);
extern void i2c_setclk(unsigned int i2cclk);

extern int i2c_read(unsigned char device, unsigned char *buf,
		    unsigned char address, int count);
extern int i2c_write(unsigned char device, unsigned char *buf,
		     unsigned char address, int count);

#endif /* __ASM_JZ4770_MISC_H__ */
