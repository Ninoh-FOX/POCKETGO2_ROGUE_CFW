/*
 * jz4770-pcm.h  --  JZ4770 SoC audio driver
 */

#ifndef _JZ4770_PCM_H
#define _JZ4770_PCM_H

#include <linux/types.h>


struct jz4770_pcm_dma_params {
	int channel;				/* channel ID */
	dma_addr_t dma_addr;
	int dma_size;				/* size of the DMA transfer */
};

#endif /* _JZ4770_PCM_H */
