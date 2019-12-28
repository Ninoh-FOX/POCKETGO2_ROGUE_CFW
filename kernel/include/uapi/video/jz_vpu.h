/*
 * linux/drivers/misc/jz_vpu.h
 *
 * User space interface to jz_vpu driver.
 *
 * Copyright (C) 2013  Wladimir J. van der Laan
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 */
#ifndef __JZ_VPU_H
#define __JZ_VPU_H

#define JZ_VPU_IOCTL_WAIT_COMPLETE	(0x99 + 0x2)
#define JZ_VPU_IOCTL_ALLOC		(0x99 + 0x3)
#define JZ_VPU_IOCTL_FREE			(0x99 + 0x4)

/* ioctl structure for TCSM_IOCTL_ALLOC */
struct jz_vpu_alloc {
	size_t size;
	unsigned long physical;
};

#endif
