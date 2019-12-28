/*
 * radio-rda5807.h - Board related data for the RDA5807 FM tuner chip driver
 *
 * Copyright (c) 2011 Maarten ter Huurne <maarten@treewalker.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef RADIO_RDA5807_H
#define RADIO_RDA5807_H

/* The driver uses random access I/O to the registers via I2C address 0x11. */
#define RDA5807_I2C_ADDR		0x11

/* Working current: 1.8, 2.1, 2.5 or 3.0 mA. */
#define RDA5807_INPUT_LNA_WC_18		(0 << 0)
#define RDA5807_INPUT_LNA_WC_21		(1 << 0)
#define RDA5807_INPUT_LNA_WC_25		(2 << 0)
#define RDA5807_INPUT_LNA_WC_30		(3 << 0)
/* Use antenna signal connected to LNAN and/or LNAP pin? */
#define RDA5807_LNA_PORT_N		(1 << 2)
#define RDA5807_LNA_PORT_P		(1 << 3)

/* Ouput analog audio on LOUT+ROUT pins */
#define RDA5807_OUTPUT_AUDIO_ANALOG	(1 << 0)
/* Output digital audio using I2S on GPIO1-3 pins */
#define RDA5807_OUTPUT_AUDIO_I2S	(1 << 1)
/* Output stereo indicator signal on GPIO3 pin */
#define RDA5807_OUTPUT_STEREO_INDICATOR	(1 << 2)

struct rda5807_platform_data {
	u8 input_flags;
	u8 output_flags;
};

#endif /* RADIO_RDA5807_H */
