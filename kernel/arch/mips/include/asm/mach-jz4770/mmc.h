/*
 * arch/mips/include/asm/mach-jz4770/mmc.h
 *
 * JZ4770 MMC/SD Controller platform data
 *
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 */

#ifndef __ASM_MACH_JZ4770_MMC_H__
#define __ASM_MACH_JZ4770_MMC_H__

struct jz_mmc_platform_data {
	unsigned int ocr_mask;			/* available voltages */
	unsigned char support_sdio;
	unsigned char bus_width;

	int gpio_card_detect;
	int gpio_read_only;
	int gpio_power;
	unsigned card_detect_active_low:1;
	unsigned read_only_active_low:1;
	unsigned power_active_low:1;

	/*
	 * Set this if the card cannot be physically removed from the device.
	 */
	unsigned nonremovable:1;

	/*
	 * Each of the three MMC/SD Controllers can either use a private pin
	 * section with 4 data pins, or a shared pin section with 8 data pins.
	 * Set this flag if this MSC is the one using the shared pins.
	 * Note that there are 8 data pins available, but bus_width determines
	 * how many are actually used.
	 */
	unsigned use_shared_8bit_pins:1;
};

#endif /* __ASM_MACH_JZ4770_MMC_H__ */
