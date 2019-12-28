/*
 * I2C adapter for the INGENIC I2C bus access.
 *
 * Copyright (C) 2006 - 2009 Ingenic Semiconductor Inc.
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/time.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/slab.h>

#include <asm/irq.h>
#include <asm/io.h>
#include <linux/module.h>
#include <asm/addrspace.h>

#include <asm/mach-jz4770/dma.h>
#include <asm/mach-jz4770/jz4770cpm.h>
#include <asm/mach-jz4770/jz4770dmac.h>
#include <asm/mach-jz4770/jz4770i2c.h>

#include "i2c-jz4770.h"


/* I2C protocol */
#define I2C_READ_CMD	(1 << 8)
#define I2C_WRITE_CMD	(0 << 8)
#define TIMEOUT         0xffff
#define I2C_CLIENT_NUM  20

//#define DEBUG
#ifdef DEBUG
#define dprintk(x...)	printk(x)
#else
#define dprintk(x...) do{}while(0)
#endif

#define DANGERS_WAIT_ON(cond, nsec)					\
	({								\
		unsigned long m_start_time = jiffies;			\
		int __nsec_i = 0;					\
									\
		while (cond) {						\
			if (time_after(jiffies, m_start_time + HZ)) {	\
				printk("WARNING: %s:%d condition never becomes to zero\n", __func__, __LINE__); \
				if (__nsec_i >= (int)((nsec)))		\
					break;				\
				m_start_time = jiffies;			\
			}						\
		}							\
									\
		(__nsec_i == (int)((nsec)));				\
	 })

struct jz_i2c_dma_info {
	int chan;
	volatile atomic_t is_waiting;
	//struct completion comp;
	int i2c_id;
	int dma_id;
	unsigned int dma_req;
	char name[12];
};

static struct jz_i2c_dma_info rx_dma_info[JZ_I2C_NUM] = {
	{
		.chan = -1,
		.i2c_id = 0,
		.dma_id = DMA_ID_I2C0_RX,
		.dma_req = 0x29,
		.name = "i2c0 read",
	},
	{
		.chan = -1,
		.i2c_id = 1,
		.dma_id = DMA_ID_I2C1_RX,
		.dma_req = 0x2b,
		.name = "i2c1 read",
	},
	{
		.chan = -1,
		.i2c_id = 2,
		.dma_id = DMA_ID_I2C2_RX,
		.dma_req = 0x3b,
		.name = "i2c2 read",
	}

};

static struct jz_i2c_dma_info tx_dma_info[JZ_I2C_NUM] = {
	{
		.chan = -1,
		.i2c_id = 0,
		.dma_id = DMA_ID_I2C0_TX,
		.dma_req = 0x28,
		.name = "i2c0 write",
	},
	{
		.chan = -1,
		.i2c_id = 1,
		.dma_id = DMA_ID_I2C1_TX,
		.dma_req = 0x2a,
		.name = "i2c1 write",
	},
	{
		.chan = -1,
		.i2c_id = 2,
		.dma_id = DMA_ID_I2C2_TX,
		.dma_req = 0x3a,
		.name = "i2c2 write",
	}
};

struct jz_i2c {
	int                     id;
	unsigned int            irq;
	struct i2c_adapter	adap;
	int (*write)(unsigned char device, unsigned char *buf,
		     int length, struct jz_i2c *i2c, int restart);
	int (*read)(unsigned char device, unsigned char *buf,
		    int length, struct jz_i2c *i2c, int restart);
	int (*read_offset)(unsigned char device,
			   unsigned char *offset, int off_len,
			   unsigned char *buf, int read_len,
			   struct jz_i2c *i2c);
};

#define PRINT_REG_WITH_ID(reg_name, id) \
  printk("" #reg_name "(%d) = 0x%08x\n", id, reg_name(id))

#ifdef DEBUG
static void jz_dump_i2c_regs(int i2c_id, int line) {
        printk("***** i2c%d regs, line = %d *****\n", i2c_id, line);
        PRINT_REG_WITH_ID(REG_I2C_CTRL, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_TAR, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_SAR, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_DC, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_SHCNT, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_SLCNT, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_FHCNT, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_FLCNT, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_INTST, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_INTM, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_RXTL, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_TXTL, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_CINTR, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_CRXUF, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_CRXOF, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_CTXOF, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_CRXREQ, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_CTXABRT, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_CRXDONE, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_CACT, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_CSTP, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_CSTT, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_CGC, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_ENB, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_STA, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_TXFLR, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_RXFLR, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_TXABRT, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_DMACR, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_DMATDLR, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_DMARDLR, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_SDASU, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_ACKGC, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_ENSTA, i2c_id);
        PRINT_REG_WITH_ID(REG_I2C_SDAHD, i2c_id);
}
#else  /* !DEBUG */
static void jz_dump_i2c_regs(int i2c_id, int line) {
}
#endif	/* DEBUG */

struct i2c_speed {
	unsigned int speed;
	unsigned char slave_addr;
};
static  struct i2c_speed jz4770_i2c_speed[I2C_CLIENT_NUM];
static unsigned char current_device[JZ_I2C_NUM] = { 0x0, 0x0, 0x0 };
static int client_cnt = 0;
static int i2c_ctrl_rest[JZ_I2C_NUM] = { 0, 0, 0 };

void i2c_jz_setclk(struct i2c_client *client,unsigned long i2cclk)
{
	if (i2cclk > 0 && i2cclk <= 400000) {
		jz4770_i2c_speed[client_cnt].slave_addr = client->addr;
		jz4770_i2c_speed[client_cnt].speed      = i2cclk / 1000;
	} else if (i2cclk <= 0) {
		jz4770_i2c_speed[client_cnt].slave_addr = client->addr;
		jz4770_i2c_speed[client_cnt].speed      = 100;
	} else {
		jz4770_i2c_speed[client_cnt].slave_addr = client->addr;
		jz4770_i2c_speed[client_cnt].speed      = 400;
	}

	client_cnt++;
}
EXPORT_SYMBOL_GPL(i2c_jz_setclk);

static int i2c_disable(int i2c_id)
{
	int timeout = TIMEOUT;

	__i2c_disable(i2c_id);
	while(__i2c_is_enable(i2c_id) && (timeout > 0)) {
		udelay(1);
		timeout--;
	}
	if(timeout)
		return 0;
	else
		return 1;
}

static int i2c_set_clk(int i2c_clk, int i2c_id)
{
	int dev_clk_khz = cpm_get_clock(CGU_PCLK) / 1000;
        int cnt_high = 0;       /* HIGH period count of the SCL clock */
        int cnt_low = 0;        /* LOW period count of the SCL clock */
        int cnt_period = 0;     /* period count of the SCL clock */

        if (i2c_clk <= 0 || i2c_clk > 400)
                goto set_clk_err;

        /* 1 I2C cycle equals to cnt_period PCLK(i2c_clk) */
        cnt_period = dev_clk_khz / i2c_clk;
        if (i2c_clk <= 100) {
                /* i2c standard mode, the min LOW and HIGH period are 4700 ns and 4000 ns */
                cnt_high = (cnt_period * 4000) / (4700 + 4000);
        } else {
                /* i2c fast mode, the min LOW and HIGH period are 1300 ns and 600 ns */
                cnt_high = (cnt_period * 600) / (1300 + 600);
        }

        cnt_low = cnt_period - cnt_high;

	//printk("dev_clk = %d, i2c_clk = %d cnt_period = %d, cnt_high = %d, cnt_low = %d, \n", dev_clk_khz, i2c_clk, cnt_period, cnt_high, cnt_low);

        if (i2c_clk <= 100) {
                REG_I2C_CTRL(i2c_id) = 0x43 | i2c_ctrl_rest[i2c_id];      /* standard speed mode*/
                REG_I2C_SHCNT(i2c_id) = I2CSHCNT_ADJUST(cnt_high);
                REG_I2C_SLCNT(i2c_id) = I2CSLCNT_ADJUST(cnt_low);
        } else {
                REG_I2C_CTRL(i2c_id) = 0x45 | i2c_ctrl_rest[i2c_id];       /* high speed mode*/
                REG_I2C_FHCNT(i2c_id) = I2CFLCNT_ADJUST(cnt_high);
                REG_I2C_FLCNT(i2c_id) = I2CFLCNT_ADJUST(cnt_low);
        }
	//__i2c_hold_time_enable(i2c_id);
        //__i2c_hold_time_disable(i2c_id);
	/*
	 * a i2c device must internally provide a hold time at least 300ns
	 * tHD:DAT
	 *	Standard Mode: min=300ns, max=3450ns
	 *	Fast Mode: min=0ns, max=900ns
	 * tSU:DAT
	 *	Standard Mode: min=250ns, max=infinite
	 *	Fast Mode: min=100(250ns is recommanded), max=infinite
	 *
	 * 1i2c_clk = 10^6 / dev_clk_khz
	 * on FPGA, dev_clk_khz = 12000, so 1i2c_clk = 1000/12 = 83ns
	 *
	 */
	__i2c_set_setup_time(i2c_id, 4);
        //__i2c_set_hold_time(i2c_id, 10);

	//printk("tSU:DAT = %d tHD:DAT = %d\n",
	//	REG_I2C_SDASU(i2c_id) & 0xff, REG_I2C_SDAHD(i2c_id) & 0xff);
	return 0;

set_clk_err:
	printk("i2c set sclk faild,i2c_clk=%d KHz,dev_clk=%dKHz.\n", i2c_clk, dev_clk_khz);
	return -1;
}

static int i2c_set_target(unsigned char address,int i2c_id)
{

	int res = DANGERS_WAIT_ON((!__i2c_txfifo_is_empty(i2c_id) || __i2c_master_active(i2c_id)), 2);
	if (res) {
		printk("WARNING: i2c%d failed to set slave address!\n", i2c_id);
		return -ETIMEDOUT;
	}
	REG_I2C_TAR(i2c_id) = address;  /* slave id needed write only once */

	return 0;
}

static int i2c_init_as_master(int i2c_id,unsigned char device)
{

	int i;
	unsigned int speed = 100; /* default to 100k */

	if(i2c_disable(i2c_id)) {
		printk("i2c not disable, check if any transfer pending!\n");
		return -ETIMEDOUT;
	}

	for (i = 0; i < I2C_CLIENT_NUM; i++) {
		if(device == jz4770_i2c_speed[i].slave_addr) {
			speed = jz4770_i2c_speed[i].speed;
			break;
		}
	}
	i2c_set_clk(speed,i2c_id);

	REG_I2C_INTM(i2c_id) = 0x2; /*mask all interrupt*/
	REG_I2C_TXTL(i2c_id) = 0xf;
	REG_I2C_RXTL(i2c_id) = 0;
	REG_I2C_ENB(i2c_id) = 1;   /*enable i2c*/

	return 0;
}

static irqreturn_t jz_i2c_dma_callback(int irq, void *devid)
{
	struct jz_i2c_dma_info *dma_info = (struct jz_i2c_dma_info *)devid;
	int chan = dma_info->chan;

	disable_dma(chan);
	if (__dmac_channel_address_error_detected(chan)) {
		printk("%s: DMAC address error.\n",
		       __FUNCTION__);
		__dmac_channel_clear_address_error(chan);
	}
	if (__dmac_channel_transmit_end_detected(chan)) {
		__dmac_channel_clear_transmit_end(chan);
	}

	if (atomic_read(&dma_info->is_waiting)) {
		//complete(&(dma_info->comp));
		atomic_set(&dma_info->is_waiting, 0);
		wmb();
	}

	return IRQ_HANDLED;
}

static int jz_i2c_dma_init_as_write(unsigned short *buf,int length, int bus_id, int need_wait)
{
	struct jz_i2c_dma_info *dma_info = tx_dma_info + bus_id;

	__i2c_set_dma_td_level(bus_id, 8); // half FIFO depth, 16/2
	__i2c_dma_td_enable(bus_id);
	SETREG8(I2C_CTRL(bus_id),I2C_CTRL_STPHLD);

	dma_cache_wback((unsigned long)buf, length * 2);

	if (need_wait)
		atomic_set(&dma_info->is_waiting, 1);
	else
		atomic_set(&dma_info->is_waiting, 0);

	// init_completion(&dma_info->comp);

	/* Init DMA module */
	REG_DMAC_DMACR(dma_info->chan/HALF_DMA_NUM) = 0;
	REG_DMAC_DCCSR(dma_info->chan) = 0;
	REG_DMAC_DRSR(dma_info->chan) = dma_info->dma_req;
	REG_DMAC_DSAR(dma_info->chan) = CPHYSADDR(buf);
	REG_DMAC_DTAR(dma_info->chan) = CPHYSADDR(I2C_DC(bus_id));
	REG_DMAC_DTCR(dma_info->chan) = length;
	REG_DMAC_DCMD(dma_info->chan) = DMAC_DCMD_SAI | DMAC_DCMD_SWDH_16 | DMAC_DCMD_DWDH_16 | DMAC_DCMD_DS_16BIT | DMAC_DCMD_TIE;
	REG_DMAC_DCCSR(dma_info->chan) = DMAC_DCCSR_NDES | DMAC_DCCSR_EN;
	REG_DMAC_DMACR(dma_info->chan/HALF_DMA_NUM) = DMAC_DMACR_DMAE; /* global DMA enable bit */

	if (need_wait) {
		//wait_for_completion(&dma_info->comp);
		int res = DANGERS_WAIT_ON(atomic_read(&dma_info->is_waiting), 2);
		__i2c_dma_td_disable(dma_info->i2c_id);
		if (res) {
			printk("WARNING: i2c%d write error, maybe I2C_CLK or I2C_SDA is pull down to zero by some one!\n", bus_id);
			jz_stop_dma(dma_info->chan);
			return -ETIMEDOUT;
		}
	}

	return 0;
}

static int jz_i2c_dma_init_as_read(unsigned char *buf,int length, int bus_id, int need_wait)
{
	struct jz_i2c_dma_info *dma_info = rx_dma_info + bus_id;

	__i2c_set_dma_rd_level(bus_id, 0);
	__i2c_dma_rd_enable(bus_id);

	memset((void *)buf, 0, length * sizeof(unsigned char));
	dma_cache_wback_inv((unsigned long)buf, length * sizeof(unsigned char));

	if (need_wait)
		atomic_set(&dma_info->is_waiting, 1);
	else
		atomic_set(&dma_info->is_waiting, 0);
	// init_completion(&dma_info->comp);

	/* Init DMA module */
	REG_DMAC_DMACR(dma_info->chan/HALF_DMA_NUM) = 0;
	REG_DMAC_DCCSR(dma_info->chan) = 0;
	REG_DMAC_DRSR(dma_info->chan) = dma_info->dma_req;
	REG_DMAC_DSAR(dma_info->chan) = CPHYSADDR(I2C_DC(bus_id));
	REG_DMAC_DTAR(dma_info->chan) = CPHYSADDR(buf);
	REG_DMAC_DTCR(dma_info->chan) = length;
	REG_DMAC_DCMD(dma_info->chan) = DMAC_DCMD_DAI | DMAC_DCMD_SWDH_8 | DMAC_DCMD_DWDH_8 | DMAC_DCMD_DS_8BIT | DMAC_DCMD_TIE;
	REG_DMAC_DCCSR(dma_info->chan) = DMAC_DCCSR_NDES | DMAC_DCCSR_EN;
	REG_DMAC_DMACR(dma_info->chan/HALF_DMA_NUM) = DMAC_DMACR_DMAE; /* global DMA enable bit */

	if (need_wait) {
		//wait_for_completion(&(dma_info->comp));
		int res = DANGERS_WAIT_ON(atomic_read(&dma_info->is_waiting), 2);
		__i2c_dma_rd_disable(bus_id);
		if (res) {
			printk("WARNNING: i2c%d read error, did not receive enough data from i2c slave!\n", bus_id);
			jz_stop_dma(dma_info->chan);
			return -ETIMEDOUT;
		}
	}

	return 0;
}

static int xfer_read(unsigned char device, unsigned char *buf,
		     int length, struct jz_i2c *i2c, int restart)
{
	int timeout;
	int i2c_id = i2c->id;
	int i;
	int ret = 0;
	unsigned short *rbuf;

	if (!restart) {
		ret = i2c_set_target(device,i2c_id);
		if (ret < 0)
			return ret;
	}

	rbuf = (unsigned short *)kzalloc(sizeof(unsigned short) * length,GFP_KERNEL);
	for (i = 0; i < length; i++)
		rbuf[i] = I2C_READ_CMD;

	/* must start read dma first */
	memset((void *)buf, 0, length * sizeof(unsigned char));
	jz_i2c_dma_init_as_read(buf,length,i2c_id, 0);

	atomic_set(&rx_dma_info[i2c_id].is_waiting, 1);
	jz_i2c_dma_init_as_write(rbuf,length,i2c_id, 0);
	/* must clear STPHLD here! , but we can NOT support more than 2 i2c_msg if we do this */
	CLRREG8(I2C_CTRL(i2c_id),I2C_CTRL_STPHLD);

	//wait_for_completion(&(rx_dma_info[i2c_id].comp));
	ret = DANGERS_WAIT_ON(atomic_read(&rx_dma_info[i2c_id].is_waiting), 2);
	__i2c_dma_rd_disable(i2c_id);
	__i2c_dma_td_disable(i2c_id);

	if (ret) {
		printk("WARNING: i2c%d did not receive enough data from slave!\n", i2c_id);
		jz_stop_dma(tx_dma_info[i2c_id].chan);
		jz_stop_dma(rx_dma_info[i2c_id].chan);

		goto out;
	}

	timeout = TIMEOUT;
	while ((REG_I2C_STA(i2c_id) & I2C_STA_MSTACT) && timeout) {
		timeout --;
		udelay(10);
	}
	if (!timeout) {
		printk("WARNING(%s:%d): i2c%d wait master inactive failed!\n", __func__, __LINE__, i2c_id);
		ret = -ETIMEDOUT;
	}

 out:
	kfree(rbuf);
	return ret;
}

static int xfer_write(unsigned char device, unsigned char *buf,
		      int length, struct jz_i2c *i2c, int restart)
{
	int timeout;
	int i2c_id = i2c->id;
	int i = 0;
	int ret = 0;
	unsigned short *wbuf;

	ret = i2c_set_target(device,i2c_id);
	if (ret)
		return ret;

	wbuf = (unsigned short *)kzalloc(sizeof(unsigned short) * length,GFP_KERNEL);
	for (i = 0; i < length; i++)
		wbuf[i] = I2C_WRITE_CMD | buf[i];

	ret = jz_i2c_dma_init_as_write(wbuf,length,i2c_id, 1);
	if (!restart)
		CLRREG8(I2C_CTRL(i2c_id),I2C_CTRL_STPHLD);

	if (ret)
		goto out;

	timeout = TIMEOUT;
	while((!(REG_I2C_STA(i2c_id) & I2C_STA_TFE)) && timeout){
		timeout --;
		udelay(10);
	}
	if (!timeout){
		printk("Write i2c device 0x%2x failed, wait TF buff empty timeout.\n",device);
		ret = -ETIMEDOUT;
		goto out;
	}

	if (!restart) {
		timeout = TIMEOUT;
		while (__i2c_master_active(i2c_id) && timeout)
			timeout--;
		if (!timeout){
			printk("Write i2c device 0x%2x failed, wait master inactive timeout.\n",device);
			ret = -ETIMEDOUT;
			goto out;
		}
	}

	udelay(500);	      /* the TXABRT bit seems not immediatly seted when error happen */
	if ((REG_I2C_INTST(i2c_id) & I2C_INTST_TXABT) ||
	    REG_I2C_TXABRT(i2c_id)) {
		volatile int tmp;
		printk("Write i2c device 0x%2x failed: device no ack or abort.\n",device);
		__i2c_clear_interrupts(tmp,i2c_id);
		ret = -ECANCELED;
		goto out;
	}


 out:
	kfree(wbuf);
	return ret;
}

static int xfer_read_pio(unsigned char device, unsigned char *buf,
			 int length, struct jz_i2c *i2c, int restart)
{
	int timeout;
	int i2c_id = i2c->id;
	int cnt;
	unsigned long start_time;
	int ret = 0;

	if (length  > 16)     /* FIFO depth is 16 */
		return -1;

	/* set target address */
	ret = i2c_set_target(device,i2c_id);
	if (ret)
		return ret;

	SETREG8(I2C_CTRL(i2c_id), I2C_CTRL_STPHLD);
	for (cnt = 0; cnt < length; cnt++) {
		udelay(100);
		__i2c_write(I2C_READ_CMD, i2c_id);
	}
	udelay(100);
	CLRREG8(I2C_CTRL(i2c_id),I2C_CTRL_STPHLD);

	memset((void *)buf, 0, length * sizeof(unsigned char));

	for (cnt = 0; cnt < length; cnt++) {
		start_time = jiffies;
		while (!(REG_I2C_STA(i2c_id) & I2C_STA_RFNE)) {
			if ((REG_I2C_INTST(i2c_id) & I2C_INTST_TXABT) ||
			    REG_I2C_TXABRT(i2c_id)) {
				volatile int tmp;
				printk("Read i2c device 0x%2x failed: i2c abort.\n",device);
				__i2c_clear_interrupts(tmp,i2c_id);
				ret = -ECANCELED;
				goto out;
			}
			if (time_after(jiffies, start_time + HZ)) {
				printk("WARNING: i2c%d: data not received after 1 second!\n", i2c_id);
				ret = -ETIMEDOUT;
				goto out;
			}
			udelay(10);
		}
		buf[cnt] = __i2c_read(i2c_id);
	}

	timeout = TIMEOUT;
	while ((REG_I2C_STA(i2c_id) & I2C_STA_MSTACT) && --timeout)
		udelay(10);
	if (!timeout){
		printk("Read i2c device 0x%2x failed: wait master inactive timeout.\n",device);
		ret = -ETIMEDOUT;
	}

 out:
	return ret;
}

static int xfer_read_offset_pio(unsigned char device,
				unsigned char *offset, int off_len,
				unsigned char *buf, int read_len,
				struct jz_i2c *i2c)
{
	int timeout;
	int i2c_id = i2c->id;
	int i;
	int ret;
	unsigned long start_time;
	unsigned short wbuf[32];
	int total_len = off_len + read_len;

	if (read_len  > 16)     /* FIFO depth is 16 */
		return -EINVAL;

	if (total_len > 32)
		return -EINVAL;

	for (i = 0; i < off_len; i++) {
		wbuf[i] = I2C_WRITE_CMD | *offset;
		offset++;
	}
	for (i = 0; i < read_len; i++)
		wbuf[i + off_len] = I2C_READ_CMD;

	/* set target address */
	ret = i2c_set_target(device,i2c_id);
	if (ret)
		return ret;

	SETREG8(I2C_CTRL(i2c_id), I2C_CTRL_STPHLD);
	for (i = 0; i < total_len; i++) {
		udelay(100);
		__i2c_write(wbuf[i], i2c_id);
	}
	udelay(100);
	CLRREG8(I2C_CTRL(i2c_id),I2C_CTRL_STPHLD);
	timeout = TIMEOUT;
	while((!(REG_I2C_STA(i2c_id) & I2C_STA_TFE)) && --timeout){
		udelay(10);
	}
	if (!timeout){
		printk("%s:%d: wait tx fifi empty timedout! dev_addr = 0x%02x\n",
		       __func__, __LINE__, device);
		ret = -ETIMEDOUT;
		goto out;
	}

	if ((REG_I2C_INTST(i2c_id) & I2C_INTST_TXABT) ||
	    REG_I2C_TXABRT(i2c_id)) {
		int tmp;
		printk("%s:%d: TX abart, dev(0x%02x0 no ack!\n",
		       __func__, __LINE__, device);
		__i2c_clear_interrupts(tmp,i2c_id);
		ret = -ECANCELED;
		goto out;
	}

	memset((void *)buf, 0, read_len * sizeof(unsigned char));
	for (i = 0; i < read_len; i++) {
		start_time = jiffies;
		while (!(REG_I2C_STA(i2c_id) & I2C_STA_RFNE)) {
			if ((REG_I2C_INTST(i2c_id) & I2C_INTST_TXABT) ||
			    REG_I2C_TXABRT(i2c_id)) {
				int tmp;
				jz_dump_i2c_regs(i2c_id, __LINE__);
				__i2c_clear_interrupts(tmp,i2c_id);
				printk("%s:%d: i2c transfer aborted, dev_addr = 0x%02x, intr = 0x%08x.\n",
				       __func__, __LINE__, device, tmp);
				ret = -ECANCELED;
				goto out;

			}
			if (time_after(jiffies, start_time + HZ)) {
				printk("WARNING: i2c%d: data not received after 1 second!\n", i2c_id);
				ret = -ETIMEDOUT;
				goto out;
			}
			udelay(10);
		}
		buf[i] = __i2c_read(i2c_id);
	}

	timeout = TIMEOUT;
	while ((REG_I2C_STA(i2c_id) & I2C_STA_MSTACT) && --timeout)
		udelay(10);
	if (!timeout){
		printk("%s:%d: waite master inactive timeout, dev_addr = 0x%02x\n",
		       __func__, __LINE__, device);
		ret = -ETIMEDOUT;
		goto out;
	}

 out:
	return ret;
}

static int xfer_write_pio(unsigned char device, unsigned char *buf,
			  int length, struct jz_i2c *i2c, int restart)
{
	int timeout;
	int i2c_id = i2c->id;
	unsigned short wdata;
	int i = 0;
	int ret = 0;

	ret = i2c_set_target(device,i2c_id);
	if (ret)
		return ret;

	SETREG8(I2C_CTRL(i2c_id), I2C_CTRL_STPHLD);
	for (i = 0; i < length; i++) {
		wdata = I2C_WRITE_CMD | *buf++;
		__i2c_write(wdata, i2c_id);
	}
	CLRREG8(I2C_CTRL(i2c_id), I2C_CTRL_STPHLD);

	timeout = TIMEOUT;
	while((!(REG_I2C_STA(i2c_id) & I2C_STA_TFE)) && --timeout){
		udelay(10);
	}
	if (!timeout){
		printk("Write i2c device 0x%2x failed: wait TF buff empty timeout.\n",device);
		ret = -ETIMEDOUT;
		goto out;
	}

	timeout = TIMEOUT;
	while (__i2c_master_active(i2c_id) && --timeout);
	if (!timeout){
		printk("Write i2c device 0x%2x failed: wait master inactive timeout.\n",device);
		ret = -ETIMEDOUT;
		goto out;
	}

	if ((REG_I2C_INTST(i2c_id) & I2C_INTST_TXABT) ||
	    REG_I2C_TXABRT(i2c_id)) {
		volatile int intr;
		printk("Write i2c device 0x%2x failed: device no ack or abort.\n",device);
		__i2c_clear_interrupts(intr,i2c_id);
		ret = -ETIMEDOUT;
		goto out;
	}

 out:
	return ret;
}

static int i2c_jz_xfer(struct i2c_adapter *adap, struct i2c_msg *pmsg, int num)
{
	int ret, i;
	struct jz_i2c *i2c = adap->algo_data;
	__u16 addr = pmsg->addr;

	BUG_ON(in_irq());     /* we can not run in hardirq */

	if (num > 2)	      /* sorry, our driver currently can not support more than two message
			       * if you have such requirements, contact Ingenic for support
			       */
		return -EINVAL;

	if (num > 1) {
		i2c_ctrl_rest[i2c->id] = I2C_CTRL_REST;
		current_device[i2c->id] = pmsg->addr;
		if (i2c_init_as_master(i2c->id, addr) < 0)
			return -EBUSY;
	} else {
		if (pmsg->addr != current_device[i2c->id]) {
			i2c_ctrl_rest[i2c->id] = 0;
			current_device[i2c->id] = pmsg->addr;
			if (i2c_init_as_master(i2c->id, addr) < 0)
				return -EBUSY;
		}
	}

	if ((num > 1) &&
	    i2c->read_offset &&
	    ((pmsg[0].flags & I2C_M_RD) == 0) &&
	    (pmsg[1].flags & I2C_M_RD)) {
		ret = i2c->read_offset(pmsg[0].addr,
					pmsg[0].buf, pmsg[0].len,
					pmsg[1].buf, pmsg[1].len,
					i2c);
		if (ret) {
			i2c_init_as_master(i2c->id, addr);
			return ret;
		} else
			return num;
	}

	for (i = 0; i < num; i++) {
		if (likely(pmsg->len && pmsg->buf)) {	/* sanity check */
			if (pmsg->flags & I2C_M_RD){
				ret = i2c->read(pmsg->addr, pmsg->buf, pmsg->len, i2c, (num > 1));
			} else {
				ret = i2c->write(pmsg->addr, pmsg->buf, pmsg->len, i2c, (num > 1));
			}
			if (ret) {
				i2c_init_as_master(i2c->id, addr);
				return ret;
			}
		}
		pmsg++;		/* next message */
	}

	return i;
}

static irqreturn_t jz_i2c_irq(int irqno, void *dev)
{
	struct jz_i2c *i2c = dev;
	volatile int ret;

	__i2c_clear_interrupts(ret, i2c->id);

	return IRQ_HANDLED;
}

static u32 i2c_jz_functionality(struct i2c_adapter *adap)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL;
}

static const struct i2c_algorithm i2c_jz_algorithm = {
	.master_xfer	= i2c_jz_xfer,
	.functionality	= i2c_jz_functionality,
};

static int i2c_jz_probe(struct platform_device *pdev)
{
	struct jz_i2c *i2c;
	struct i2c_jz4770_platform_data *plat = pdev->dev.platform_data;
	int ret;

	if (pdev->id < 0 || pdev->id > 2)
		return -EINVAL;
	if (!plat)
		return -EINVAL;

	if (plat->use_dma) {
		struct jz_i2c_dma_info *tx_dma = &tx_dma_info[pdev->id];
		struct jz_i2c_dma_info *rx_dma = &rx_dma_info[pdev->id];

		tx_dma->chan = jz_request_dma(
				tx_dma->dma_id, tx_dma->name,
				jz_i2c_dma_callback, IRQF_DISABLED, tx_dma);
		printk("i2c%d: tx chan = %d\n", pdev->id, tx_dma->chan);
		if (tx_dma->chan < 0) {
			printk("i2c%d: request TX dma failed\n", pdev->id);
			return -ENODEV;
		}

		rx_dma->chan = jz_request_dma(
				rx_dma->dma_id, rx_dma->name,
				jz_i2c_dma_callback, IRQF_DISABLED, rx_dma);
		printk("i2c%d: rx chan = %d\n", pdev->id, rx_dma->chan);
		if (rx_dma->chan < 0) {
			printk("i2c%d: request RX dma failed\n", pdev->id);
			return -ENODEV;
		}
	}

	i2c = kzalloc(sizeof(struct jz_i2c), GFP_KERNEL);
	if (!i2c) {
		printk("i2c%d: alloc jz_i2c failed!\n", pdev->id);
		ret = -ENOMEM;
		goto emalloc;
	}

	switch(pdev->id) {
	case 0:
		cpm_start_clock(CGM_I2C0);
		break;
	case 1:
		cpm_start_clock(CGM_I2C1);
		break;
	case 2:
		cpm_start_clock(CGM_I2C2);
		break;
	}

	if (plat->use_dma) {
		i2c->write = xfer_write;
		i2c->read = xfer_read;
		i2c->read_offset = NULL;
	} else {
		i2c->write = xfer_write_pio;
		i2c->read = xfer_read_pio;
		i2c->read_offset = xfer_read_offset_pio;
	}

	i2c->id			= pdev->id;
	i2c->adap.owner		= THIS_MODULE;
	i2c->adap.algo		= &i2c_jz_algorithm;
	i2c->adap.retries	= 5;
	sprintf(i2c->adap.name, "i2c-jz4770.%u", pdev->id);
	i2c->adap.algo_data	= i2c;
	i2c->adap.dev.parent	= &pdev->dev;

	i2c_init_as_master(i2c->id, 0xff);

	i2c->irq = platform_get_irq(pdev, 0);
	ret = request_irq(i2c->irq, jz_i2c_irq, IRQF_DISABLED,
			  dev_name(&pdev->dev), i2c);
	if (ret != 0) {
		dev_err(&pdev->dev, "cannot claim IRQ %d\n", i2c->irq);
		goto irq_err;
	}

	i2c->adap.nr = pdev->id;
	ret = i2c_add_numbered_adapter(&i2c->adap);
	if (ret < 0) {
		printk(KERN_INFO "i2c%d: Failed to add bus\n", pdev->id);
		goto eadapt;
	}

	platform_set_drvdata(pdev, i2c);
	dev_info(&pdev->dev, "JZ4770 I2C bus driver.\n");

	return 0;

 eadapt:
	free_irq(i2c->irq, i2c);
 emalloc:
	jz_free_dma(tx_dma_info[pdev->id].chan);
	jz_free_dma(rx_dma_info[pdev->id].chan);
 irq_err:
	kfree(i2c);
	return ret;
}

static int i2c_jz_remove(struct platform_device *pdev)
{
	struct jz_i2c *i2c = platform_get_drvdata(pdev);
	struct i2c_adapter *adapter = &i2c->adap;

	i2c_del_adapter(adapter);
	platform_set_drvdata(pdev, NULL);
	if (rx_dma_info[i2c->id].chan >= 0)
		jz_free_dma(rx_dma_info[i2c->id].chan);
	if (tx_dma_info[i2c->id].chan >= 0)
		jz_free_dma(tx_dma_info[i2c->id].chan);
	return 0;
}

static struct platform_driver i2c_jz_driver = {
	.probe		= i2c_jz_probe,
	.remove		= i2c_jz_remove,
	.driver		= {
		.name	= "i2c-jz4770",
	},
};

static int __init i2c_adap_jz_init(void)
{
	return platform_driver_register(&i2c_jz_driver);
}

static void __exit i2c_adap_jz_exit(void)
{
	platform_driver_unregister(&i2c_jz_driver);
}

MODULE_LICENSE("GPL");
subsys_initcall(i2c_adap_jz_init);
module_exit(i2c_adap_jz_exit);
