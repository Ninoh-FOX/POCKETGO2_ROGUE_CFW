/*
 *  linux/drivers/mmc/host/jz_mmc/msc/jz_mmc_msc.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Copyright (c) Ingenic Semiconductor Co., Ltd.
 */

#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/semaphore.h>
#include <linux/export.h>
#include <linux/kthread.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sd.h>
#include <linux/mmc/sdio.h>
#include <linux/mmc/host.h>
#include <linux/moduleparam.h>
#include <linux/scatterlist.h>

#include <asm/mach-jz4770/mmc.h>

#include "include/chip-msc.h"
#include "include/jz_mmc_dma.h"
#include "include/jz_mmc_host.h"
#include "include/jz_mmc_msc.h"


#define MSC_STAT_ERR_BITS 0x3f
#define WAITMASK							\
	(MSC_STAT_CRC_RES_ERR |						\
	 MSC_STAT_CRC_READ_ERROR | MSC_STAT_CRC_WRITE_ERROR_MASK |	\
	 MSC_STAT_TIME_OUT_RES | MSC_STAT_TIME_OUT_READ)

#define RSP_TYPE(x)	((x) & ~(MMC_RSP_BUSY|MMC_RSP_OPCODE))

#if 1

static int jzmmc_trace_level = 0;
static int jzmmc_trace_cmd_code = -1;
static int jzmmc_trace_data_len = -1;
static int jzmmc_trace_id = 0;
module_param(jzmmc_trace_level, int, 0644);
module_param(jzmmc_trace_cmd_code, int, 0644);
module_param(jzmmc_trace_data_len, int, 0644);
module_param(jzmmc_trace_id, int, 0644);

#define TRACE_CMD_REQ()							\
	({								\
		if ( (jzmmc_trace_id & (1 << host->pdev_id)) && (jzmmc_trace_level & 0x1)) \
			if ( (jzmmc_trace_cmd_code == -1) || (jzmmc_trace_cmd_code == cmd->opcode) ) \
				printk("%s:     execute_cmd: opcode = %d cmdat = %#0x arg = %#0x data_flags = %#0x\n", \
				       mmc_hostname(host->mmc), cmd->opcode, REG_MSC_CMDAT(host->pdev_id), REG_MSC_ARG(host->pdev_id), \
				       host->curr_mrq->data ? host->curr_mrq->data->flags : 0); \
	})

#define TRACE_CMD_RES()							\
	({								\
		if ( (jzmmc_trace_id & (1 << host->pdev_id)) && (jzmmc_trace_level & 0x1)) \
			if ( (jzmmc_trace_cmd_code == -1) || (jzmmc_trace_cmd_code == cmd->opcode) ) \
				printk("%s:     cmd done: curr_res_type = %d resp[0] = %#0x err = %d state = %#0x\n", \
				       mmc_hostname(host->mmc), host->curr_res_type, cmd->resp[0], cmd->error, \
				       REG_MSC_STAT(host->pdev_id));	\
	})

#define TRACE_DATA_REQ()						\
	({								\
		if ((jzmmc_trace_id & (1 << host->pdev_id)) && (jzmmc_trace_level & 0x2) && host->curr_mrq->data ) { \
			if ((jzmmc_trace_data_len == -1) ||		\
			    (jzmmc_trace_data_len == host->curr_mrq->data->blksz * host->curr_mrq->data->blocks) ) \
				printk("%s:     blksz %d blocks %d flags %08x "	\
				       "tsac %d ms nsac %d\n",		\
				       mmc_hostname(host->mmc), host->curr_mrq->data->blksz, \
				       host->curr_mrq->data->blocks, host->curr_mrq->data->flags, \
				       host->curr_mrq->data->timeout_ns / 1000000, \
				       host->curr_mrq->data->timeout_clks); \
		}							\
	})

#define TRACE_DATA_DONE()						\
	({								\
		if ((jzmmc_trace_id & (1 << host->pdev_id)) && (jzmmc_trace_level & 0x2)) \
			if ((jzmmc_trace_data_len == -1) ||		\
			    (jzmmc_trace_data_len == data->blksz * data->blocks) ) \
				printk("%s:     stat = 0x%08x error = %d bytes_xfered = %d stop = %p\n", \
				       mmc_hostname(host->mmc), stat, data->error, \
				       data->bytes_xfered, host->curr_mrq->stop); \
	})

#define JZ_MMC_P_REG_BY_ID(reg_name, id) \
  printk("" #reg_name "(%d) = 0x%08x\n", id, reg_name(id))

void jz_mmc_dump_regs(int msc_id, int line) {
        printk("***** msc%d regs, line = %d *****\n", msc_id, line);

	JZ_MMC_P_REG_BY_ID(REG_MSC_STRPCL, msc_id);
	JZ_MMC_P_REG_BY_ID(REG_MSC_STAT, msc_id);
	JZ_MMC_P_REG_BY_ID(REG_MSC_CLKRT, msc_id);
	JZ_MMC_P_REG_BY_ID(REG_MSC_CMDAT, msc_id);
	JZ_MMC_P_REG_BY_ID(REG_MSC_RESTO, msc_id);
	JZ_MMC_P_REG_BY_ID(REG_MSC_RDTO, msc_id);
	JZ_MMC_P_REG_BY_ID(REG_MSC_BLKLEN, msc_id);
	JZ_MMC_P_REG_BY_ID(REG_MSC_NOB, msc_id);
	JZ_MMC_P_REG_BY_ID(REG_MSC_SNOB, msc_id);
	JZ_MMC_P_REG_BY_ID(REG_MSC_IMASK, msc_id);
	JZ_MMC_P_REG_BY_ID(REG_MSC_IREG, msc_id);
	JZ_MMC_P_REG_BY_ID(REG_MSC_CMD, msc_id);
	JZ_MMC_P_REG_BY_ID(REG_MSC_ARG, msc_id);
	//JZ_MMC_P_REG_BY_ID(REG_MSC_RES, msc_id);
	//JZ_MMC_P_REG_BY_ID(REG_MSC_RXFIFO, msc_id);
	//JZ_MMC_P_REG_BY_ID(REG_MSC_TXFIFO, msc_id);
	JZ_MMC_P_REG_BY_ID(REG_MSC_LPM, msc_id);
}
EXPORT_SYMBOL(jz_mmc_dump_regs);

#else
#define TRACE_CMD_REQ() do {  } while(0)
#define TRACE_CMD_RES() do {  } while(0)
#define TRACE_DATA_REQ() do {  } while(0)
#define TRACE_DATA_DONE() do {  } while(0)
#define jz_mmc_dump_regs(__mid, __ln) do {  } while(0)
#endif

static int jz_mmc_data_done(struct jz_mmc_host *host);

static void msc_irq_mask_all(int msc_id)
{
	REG_MSC_IMASK(msc_id) = 0xffff;
	REG_MSC_IREG(msc_id) = 0xffff;
}

void jz_mmc_reset(struct jz_mmc_host *host)
{
	u32 clkrt = REG_MSC_CLKRT(host->pdev_id);

//	while (REG_MSC_STAT(host->pdev_id) & MSC_STAT_CLK_EN);

	clk_enable(host->clk);

	REG_MSC_STRPCL(host->pdev_id) = MSC_STRPCL_RESET;
 	while (REG_MSC_STAT(host->pdev_id) & MSC_STAT_IS_RESETTING);

	// __msc_start_clk(host->pdev_id);
	REG_MSC_LPM(host->pdev_id) = 0x1;	// Low power mode

	clk_disable(host->clk);

	msc_irq_mask_all(host->pdev_id);

	REG_MSC_RDTO(host->pdev_id) = 0xffffffff;
	REG_MSC_RESTO(host->pdev_id) = 0xff;

	REG_MSC_CLKRT(host->pdev_id) = clkrt;
}

static inline int msc_calc_clkrt(int is_low, u32 rate)
{
	u32 clkrt;
	u32 clk_src = is_low ? 24000000 : 48000000;

	clkrt = 0;
	while (rate < clk_src) {
		clkrt++;
		clk_src >>= 1;
	}
	return clkrt;
}

void jz_mmc_set_clock(struct jz_mmc_host *host, int rate)
{
	int clkrt;

	/* __cpm_select_msc_clk_high will select 48M clock for MMC/SD card
	 * perhaps this will made some card with bad quality init fail,or
	 * bad stabilization.
	*/

	if (rate > SD_CLOCK_FAST) {
		clk_set_rate(host->clk, 48 * 1000 * 1000);
		clkrt = msc_calc_clkrt(0, rate);
		// send cmd and data at clock rising
		REG_MSC_LPM(host->pdev_id) |= 0x1 << 31;
	} else {
		clk_set_rate(host->clk, 24 * 1000 * 1000);
		clkrt = msc_calc_clkrt(1, rate);
		// send cmd and data at clock falling
		REG_MSC_LPM(host->pdev_id) &= ~(0x1 << 31);
	}
	REG_MSC_CLKRT(host->pdev_id) = clkrt;
}

static void jz_mmc_enable_irq(struct jz_mmc_host *host, unsigned int mask)
{
	REG_MSC_IMASK(host->pdev_id) &= ~mask;
}

static void jz_mmc_disable_irq(struct jz_mmc_host *host, unsigned int mask)
{
	REG_MSC_IMASK(host->pdev_id) |= mask;
}

static int jz_mmc_parse_cmd_response(struct jz_mmc_host *host, unsigned int stat)
{
	struct mmc_command *cmd = host->curr_mrq->cmd;
	int i, temp[16] = {0};
	unsigned char *buf;
	unsigned int res, v, w1, w2;

	if (!cmd)
		return -EINVAL;

	/* NOTE: we must flush the FIFO,  despite of fail or success*/
	buf = (u8 *) temp;
	switch (host->curr_res_type) {
	case 1:
		{
			/*
			 * Did I mention this is Sick.  We always need to
			 * discard the upper 8 bits of the first 16-bit word.
			 */

			res = REG_MSC_RES(host->pdev_id);
			buf[0] = (res >> 8) & 0xff;
			buf[1] = res & 0xff;

			res = REG_MSC_RES(host->pdev_id);
			buf[2] = (res >> 8) & 0xff;
			buf[3] = res & 0xff;

			res = REG_MSC_RES(host->pdev_id);
			buf[4] = res & 0xff;

			cmd->resp[0] =
				buf[1] << 24 | buf[2] << 16 | buf[3] << 8 |
				buf[4];

			// printk("opcode = %d, cmd->resp = 0x%08x\n", cmd->opcode, cmd->resp[0]);
			break;
		}
	case 2:
		{
			res = REG_MSC_RES(host->pdev_id);
			v = res & 0xffff;
			for (i = 0; i < 4; i++) {
				res = REG_MSC_RES(host->pdev_id);
				w1 = res & 0xffff;
				res = REG_MSC_RES(host->pdev_id);
				w2 = res & 0xffff;
				cmd->resp[i] = v << 24 | w1 << 8 | w2 >> 8;
				v = w2;
			}
			break;
		}
	case 0:
		break;
	}

	if (stat & MSC_STAT_TIME_OUT_RES) {
		/* :-( our customer do not want to see SO MANY timeouts :-(
		   so only CMD5 can return timeout error!!! */

		/*
		 * Note: we can not return timeout when CMD SD_SWITCH or MMC_SWITCH
		 * because we declared that out host->caps support MMC_CAP_4_BIT_DATA | MMC_CAP_8_BIT_DATA and MMC_CAP_MMC_HIGHSPEED
		 * if in the future some error occured because of this, we must add some code to remember
		 * which mode(SDIO/SD/MMC)  the MSC is in
		 */
		switch(cmd->opcode) {
		case SD_IO_SEND_OP_COND:
			//case SD_SWITCH:
			//case MMC_SWITCH:
		case SD_SEND_IF_COND:
		case MMC_APP_CMD:
			cmd->error = -ETIMEDOUT;
			break;
		default:
			/* silly, isn't it??? */
			printk("jz-msc%d: ignored MSC_STAT_TIME_OUT_RES, cmd=%d\n", host->pdev_id, cmd->opcode);
		}
	} else if (stat & MSC_STAT_CRC_RES_ERR && cmd->flags & MMC_RSP_CRC) {
		printk("jz-msc%d: MSC_STAT_CRC, cmd=%d\n", host->pdev_id, cmd->opcode);
		if (cmd->opcode == MMC_ALL_SEND_CID ||
		    cmd->opcode == MMC_SEND_CSD ||
		    cmd->opcode == MMC_SEND_CID) {
			/* a bogus CRC error can appear if the msb of
			   the 15 byte response is a one */
			if ((cmd->resp[0] & 0x80000000) == 0)
				cmd->error = -EILSEQ;
		}
	}

	TRACE_CMD_RES();

	return cmd->error;
}

void jz_mmc_data_start(struct jz_mmc_host *host)
{
	struct mmc_data *data = host->curr_mrq->data;
	unsigned int nob = data->blocks;
	unsigned int block_size = data->blksz;

	/* NOTE: this flag is never test! */
	if (data->flags & MMC_DATA_STREAM)
		nob = 0xffff;

	REG_MSC_NOB(host->pdev_id) = nob;
	REG_MSC_BLKLEN(host->pdev_id) = block_size;

	jz_mmc_start_dma(host);
}

volatile u32 junk = 0;
EXPORT_SYMBOL(junk);

void jz_mmc_data_stop(struct jz_mmc_host *host) {
	int junked = 1;

	jz_mmc_stop_dma(host);

	/* What if the data not arrived imediately? our while exits, but data remain in fifo! */
	while (!(REG_MSC_STAT(host->pdev_id) & MSC_STAT_DATA_FIFO_EMPTY)) {
		if (junked)
			jz_mmc_dump_regs(host->pdev_id, __LINE__);
		junked = 0;
		junk = REG_MSC_RXFIFO(host->pdev_id);
		printk("warning: fifo not empty when dma stopped!!! junk = 0x%08x\n", junk);
	}
}

static int need_wait_prog_done(struct mmc_command *cmd) {
	if (cmd->flags & MMC_RSP_BUSY) {
		return 1;
	} else {
		switch(cmd->opcode) { /* R1b cmds need wait PROG_DONE */
		case 12:
		case 28:
		case 29:
		case 38:
			return 1;
			break;
		default:
			/* do nothing */
			break;
		}
	}

	return 0;
}

static void jz_mmc_set_cmdat(struct jz_mmc_host *host) {
	struct mmc_request *mrq = host->curr_mrq;
	struct mmc_command *cmd = mrq->cmd;
	u32 cmdat;

	cmdat = host->cmdat;
	rmb();
	host->cmdat &= ~MSC_CMDAT_INIT;

	if(mrq->data) {
		cmdat &= ~MSC_CMDAT_BUSY;

		if ((cmd->opcode == 51) | (cmd->opcode == 8)) {
			cmdat &= ~MSC_CMDAT_BUS_WIDTH_MASK;
			cmdat |= MSC_CMDAT_BUS_WIDTH_1BIT | MSC_CMDAT_DATA_EN;
		} else
			cmdat |= MSC_CMDAT_DATA_EN;

		cmdat |= MSC_CMDAT_DMA_EN;

		if (mrq->data->flags & MMC_DATA_WRITE)
			cmdat |= MSC_CMDAT_WRITE;

		if (mrq->data->flags & MMC_DATA_STREAM)
			cmdat |= MSC_CMDAT_STREAM_BLOCK;
	}

	if (cmd->flags & MMC_RSP_BUSY)
		cmdat |= MSC_CMDAT_BUSY;

	switch (RSP_TYPE(mmc_resp_type(cmd))) {
	case RSP_TYPE(MMC_RSP_R1):	// r1, r1b, r5, r6, r7
		cmdat |= MSC_CMDAT_RESPONSE_R1;
		host->curr_res_type = 1;
		break;
	case RSP_TYPE(MMC_RSP_R3):	// r3, r4
		cmdat |= MSC_CMDAT_RESPONSE_R3;
		host->curr_res_type = 1;
		break;
	case RSP_TYPE(MMC_RSP_R2):	// r2
		cmdat |= MSC_CMDAT_RESPONSE_R2;
		host->curr_res_type = 2;
		break;
	default:
		break;
	}

	// Multi-read || Multi-write
	//if(cmd->opcode == MMC_READ_MULTIPLE_BLOCK || cmd->opcode == MMC_WRITE_MULTIPLE_BLOCK)
	if (host->curr_mrq->stop)
		cmdat |= MSC_CMDAT_SEND_AS_STOP;

#ifdef USE_DMA_BUSRT_64
	cmdat |= MSC_CMDAT_RTRG_EQUALT_16 | MSC_CMDAT_TTRG_LESS_16;
#endif
	REG_MSC_CMDAT(host->pdev_id) = cmdat;
}

static void jz_mmc_set_cmdarg(struct jz_mmc_host *host) {
	struct mmc_command *cmd = host->curr_mrq->cmd;

	if(host->pdata->bus_width == 1) {
		if (cmd->opcode == 6) {
			/* set  1 bit sd card bus*/
			if (cmd->arg == 2) {
				REG_MSC_ARG(host->pdev_id) = 0;
			}

			/* set  1 bit mmc card bus*/
			if (cmd->arg == 0x3b70101) {
				REG_MSC_ARG(host->pdev_id) = 0x3b70001;
			}
		} else
			REG_MSC_ARG(host->pdev_id) = cmd->arg;
	} else if(host->pdata->bus_width == 8) {
		if (cmd->opcode == 6) {
			/* set  8 bit mmc card bus*/
			if (cmd->arg == 0x3b70101) {
				REG_MSC_ARG(host->pdev_id) = 0x3b70201;
			} else
				REG_MSC_ARG(host->pdev_id) = cmd->arg;
		} else
			REG_MSC_ARG(host->pdev_id) = cmd->arg;
	} else
		REG_MSC_ARG(host->pdev_id) = cmd->arg;
}

#if 0
static void jz_mmc_status_checker(unsigned long arg) {
	struct jz_mmc_host *host = (struct jz_mmc_host *)arg;

	host->status = REG_MSC_STAT(host->pdev_id);
	if ((host->status & host->st_mask) || (host->eject)) {
		if (host->en_usr_intr)
			wake_up_interruptible(&host->status_check_queue);
		else
			wake_up(&host->status_check_queue);
	} else if ((host->st_check_timeout < 0) ||
		 (host->st_check_timeout > host->st_check_interval)) {
		if (host->st_check_timeout < 0)
			host->st_check_timeout -= host->st_check_interval;
		host->status_check_timer.expires = jiffies + host->st_check_interval;
		host->status_check_timer.data = (unsigned long)host;


		add_timer(&host->status_check_timer);
	} else {
		host->st_check_timeout = 0;
		wake_up_interruptible(&host->status_check_queue);
	}
}


/**
  * timeout: -1 for wait forever until contition meet, otherwise the timeout value in jiffies
  * en_usr_intr: if allow user interrupt
  * Warning: if timeout == 0 && en_usr_intr == 0, this will wait forever if the condition never meet
 **/
static u32 jz_mmc_wait_status(struct jz_mmc_host *host, u32 st_mask,
			      int timeout, int interval, int en_usr_intr) {
	int ret = 0;

	init_timer(&host->status_check_timer);
	host->status_check_timer.expires = jiffies + interval;
	host->status_check_timer.data = (unsigned long)host;
	host->status = 0;
	host->st_mask = st_mask;
	host->st_check_timeout = timeout;
	host->st_check_interval = interval;
	host->en_usr_intr = en_usr_intr;

	add_timer(&host->status_check_timer);

	if (en_usr_intr)
		ret = wait_event_interruptible(host->status_check_queue,
					       (host->status & st_mask) ||
					       (host->st_check_timeout == 0) ||
					       (host->eject));
	else
		wait_event(host->status_check_queue,
			   (host->status & st_mask) ||
			   (host->st_check_timeout == 0) ||
			   (host->eject));

	/* in case when the condition is meet before wait_event, the timer must del right away */
	del_timer_sync(&host->status_check_timer);
	return ret;
}
#endif

//int wait_cmd_done = 0;
//extern volatile int error_may_happen;

static u32 jz_mmc_wait_cmd_done(struct jz_mmc_host *host) {
	u32 timeout = 0x7fffffff;
	struct mmc_command *cmd = host->curr_mrq->cmd;
	int cmd_succ = 0;
	u32 stat = 0;

#if 0
	/* this may slow down the card response from the usrs' view, but more friendly to other kernel parts */
	jz_mmc_wait_status(host, MSC_STAT_END_CMD_RES | MSC_STAT_TIME_OUT_RES | MSC_STAT_CRC_RES_ERR,
			   -1, 1, 0); /* interval: 1jiffie = 10ms */
#else
	//wait_cmd_done = 1;
	while (!(REG_MSC_STAT(host->pdev_id) & (MSC_STAT_END_CMD_RES | MSC_STAT_TIME_OUT_RES | MSC_STAT_CRC_RES_ERR))) {
#if 0
		if (error_may_happen)
			jz_mmc_dump_regs(host->pdev_id, __LINE__);
#endif
	}
	//error_may_happen = 0;
	//wait_cmd_done = 0;
#endif

	if (REG_MSC_STAT(host->pdev_id) & MSC_STAT_TIME_OUT_RES)
		cmd->error = -ETIMEDOUT;

	/* Check for status, avoid be cleaned by following command*/
	stat = REG_MSC_STAT(host->pdev_id);
	if ((stat & MSC_STAT_END_CMD_RES) &&
	    !(stat & (MSC_STAT_TIME_OUT_RES | MSC_STAT_CRC_RES_ERR)))
		cmd_succ = 1;

	REG_MSC_IREG(host->pdev_id) = MSC_IREG_END_CMD_RES;	/* clear irq flag */

	if (cmd_succ && need_wait_prog_done(cmd)) {
		timeout = 0x7fffffff;
		while (--timeout && !(REG_MSC_IREG(host->pdev_id) & MSC_IREG_PRG_DONE))
			;

		stat |= (REG_MSC_STAT(host->pdev_id) & MSC_STAT_ERR_BITS);
		REG_MSC_IREG(host->pdev_id) = MSC_IREG_PRG_DONE;	/* clear status */
		if (timeout == 0) {
			cmd->error = -ETIMEDOUT;
			printk("JZ-MSC%d: wait prog_done error when execute_cmd!, state = 0x%08x\n", host->pdev_id, stat);
		}
	}

	return stat;
}

static void jz_mmc_send_stop_cmd(struct jz_mmc_host *host) {
	struct mmc_command *stop_cmd = host->curr_mrq->stop;

	REG_MSC_CMD(host->pdev_id) = stop_cmd->opcode;
	REG_MSC_ARG(host->pdev_id) = stop_cmd->arg;

	REG_MSC_CMDAT(host->pdev_id) = MSC_CMDAT_BUSY | MSC_CMDAT_RESPONSE_R1;

	REG_MSC_RESTO(host->pdev_id) = 0xff;

	REG_MSC_STRPCL(host->pdev_id) |= MSC_STRPCL_START_OP;

	/* Becarefull, maybe endless */
	while(!(REG_MSC_STAT(host->pdev_id) & (MSC_STAT_PRG_DONE | MSC_STAT_ERR_BITS)));

	if (REG_MSC_STAT(host->pdev_id) | MSC_STAT_ERR_BITS)
		stop_cmd->error = -ETIMEDOUT;

	REG_MSC_IREG(host->pdev_id) = MSC_IREG_PRG_DONE;
}

static int jz_mmc_data_done(struct jz_mmc_host *host)
{
	struct mmc_data *data = host->curr_mrq->data;
	int stat = 0;
	u32 timeout = 0x7fffffff;

	if (!data)
		return -EINVAL;

	stat = REG_MSC_STAT(host->pdev_id);
	REG_MSC_IREG(host->pdev_id) = MSC_IREG_DATA_TRAN_DONE;	/* clear status */

	if (host->curr_mrq && (host->curr_mrq->data->flags & MMC_DATA_WRITE)) {
		while (--timeout && !(REG_MSC_IREG(host->pdev_id) & MSC_IREG_PRG_DONE))
			;
		if (timeout == 0) {
			/* FIXME: aha, we never see this situation happen, what can we do if it happened???
			 * block.c will send cmd13??? */
			//host->curr.mrq->cmd->error = -ETIMEDOUT;
			printk(KERN_ERR"PRG_DONE not done!!!\n");
		}
		stat |= REG_MSC_STAT(host->pdev_id);
		REG_MSC_IREG(host->pdev_id) = MSC_IREG_PRG_DONE;	/* clear status */
	}

	dma_unmap_sg(mmc_dev(host->mmc), data->sg, host->dma.len,
		     host->dma.dir);

	if (stat & MSC_STAT_TIME_OUT_READ) {
		printk("MMC/SD/SDIO timeout, MMC_STAT 0x%x opcode = %d data flags = 0x%0x blocks = %d blksz = %d\n",
		       stat,
		       host->curr_mrq? host->curr_mrq->cmd->opcode : -1,
		       data->flags,
		       data->blocks,
		       data->blksz);
		data->error = -ETIMEDOUT;
	} else if (stat & (MSC_STAT_CRC_READ_ERROR | MSC_STAT_CRC_WRITE_ERROR)) {
		printk("jz-msc%d: MMC/SD/SDIO CRC error, MMC_STAT 0x%x, cmd=%d\n",
		       host->pdev_id, stat,
		       host->curr_mrq? host->curr_mrq->cmd->opcode : -1);
		data->error = -EILSEQ;
	}
	/*
	 * There appears to be a hardware design bug here.  There seems to
	 * be no way to find out how much data was transferred to the card.
	 * This means that if there was an error on any block, we mark all
	 * data blocks as being in error.
	 */
	if (data->error == 0)
		data->bytes_xfered = data->blocks * data->blksz;
	else
		data->bytes_xfered = 0;

	TRACE_DATA_DONE();

	// jz_mmc_disable_irq(host, MSC_IMASK_DATA_TRAN_DONE);
	if (host->curr_mrq->stop) {
		if ((!(REG_MSC_STAT(host->pdev_id) & MSC_STAT_AUTO_CMD_DONE)) && data->error)
			jz_mmc_send_stop_cmd(host);
		else
			while(!(REG_MSC_STAT(host->pdev_id) & (MSC_STAT_AUTO_CMD_DONE | MSC_STAT_ERR_BITS)));

		REG_MSC_CMDAT(host->pdev_id) &= ~(MSC_CMDAT_SEND_AS_STOP);
	}

	if (host->data_err) {
		data->bytes_xfered = 0;
		host->data_err = 0;
	}

	return 0;
}

void jz_mmc_execute_cmd(struct jz_mmc_host *host)
{
	struct mmc_request *mrq = host->curr_mrq;
	struct mmc_data *data = mrq->data;
	struct mmc_command *cmd = mrq->cmd;
	unsigned int stat;
	int err = 0;

	/* mask interrupts */
	REG_MSC_IMASK(host->pdev_id) = 0xffff;
	/* clear status */
	REG_MSC_IREG(host->pdev_id) = 0xffff;

	jz_mmc_set_cmdat(host);
	REG_MSC_CMD(host->pdev_id) = cmd->opcode;
	jz_mmc_set_cmdarg(host);

	/* reset NOB and BLKLEN */
	//REG_MSC_NOB(host->pdev_id) = 0;
	//REG_MSC_BLKLEN(host->pdev_id) = 0;

	TRACE_CMD_REQ();

	if(data && (data->flags & MMC_DATA_READ))
		jz_mmc_data_start(host);

	REG_MSC_RESTO(host->pdev_id) = 0xff;
	/* Send command */
	REG_MSC_STRPCL(host->pdev_id) = MSC_STRPCL_START_OP;
	stat = jz_mmc_wait_cmd_done(host);
	if (cmd->error)
		goto cmd_err;

	TRACE_DATA_REQ();

	if (jz_mmc_parse_cmd_response(host, stat))
		goto cmd_err;

	if (host->curr_mrq->data) {
		int acked = 0;
		if(host->curr_mrq->data->flags & MMC_DATA_WRITE) {
			jz_mmc_enable_irq(host, MSC_IMASK_DATA_TRAN_DONE);
			jz_mmc_data_start(host);
		}

		err = wait_event_interruptible_timeout(
				host->data_wait_queue,
				host->data_ack ||
					(REG_MSC_STAT(host->pdev_id) & WAITMASK),
				6 * HZ);

		while(!(REG_MSC_STAT(host->pdev_id) & MSC_STAT_DATA_TRAN_DONE));
		REG_MSC_STAT(host->pdev_id) &= ~(MSC_STAT_DATA_TRAN_DONE);

		acked = host->data_ack;
		host->data_ack = 0;

		if (acked)
			jz_mmc_data_done(host);
		else {
			if (err == -ERESTARTSYS) /* user cancelled */
				cmd->error = -ECANCELED;
			else if (!err) {
				printk("Timeout while IRQ_dma, opcode = %d\n", cmd->opcode);
				printk("REG_MSC_STAT(host->pdev_id) = %x\n", REG_MSC_STAT(host->pdev_id));
				jz_mmc_dump_regs(host->pdev_id, __LINE__);
				cmd->error = -ETIMEDOUT;
			}
			goto data_wait_err;
		}

	}
	return;

cmd_err:
data_wait_err:
	if (host->curr_mrq->data)
		host->curr_mrq->data->bytes_xfered = 0;

	if (host->curr_mrq->data)
		jz_mmc_data_stop(host);
}

static irqreturn_t jz_mmc_irq(int irq, void *devid)
{
	struct jz_mmc_host *host = devid;
	unsigned int ireg = 0;

	ireg = REG_MSC_IREG(host->pdev_id);
	if (ireg) {
		if (ireg & MSC_IREG_DATA_TRAN_DONE) {
			jz_mmc_disable_irq(host, MSC_IMASK_DATA_TRAN_DONE);
			BUG_ON(host->data_ack);
			host->data_ack = 1;
			wmb();
			wake_up_interruptible(&host->data_wait_queue);
		}
	}


	return IRQ_HANDLED;
}

static char *msc_trans_irq_name[] = {
	"msc_trans_0",
	"msc_trans_1",
	"msc_trans_2",
};

int jz_mmc_msc_init(struct jz_mmc_host *host)
{
	int ret = 0;

	jz_mmc_reset(host);

	host->data_ack = 0;
	init_waitqueue_head(&host->data_wait_queue);
#if 0
	init_waitqueue_head(&host->status_check_queue);
	init_timer(&host->status_check_timer);
	host->status_check_timer.function = jz_mmc_status_checker;
#endif

	ret = request_irq(host->irq, jz_mmc_irq, 0, msc_trans_irq_name[host->pdev_id], host);
	if (ret) {
		printk(KERN_ERR "MMC/SD: can't request MMC/SD IRQ\n");
		return ret;
	}

	return 0;
}

void jz_mmc_msc_deinit(struct jz_mmc_host *host)
{
	free_irq(host->irq, &host);
}
