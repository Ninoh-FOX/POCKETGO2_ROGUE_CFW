/*
 * drivers/i2c/busses/i2c-jz4770.h
 *
 * JZ4770 I2C register definition.
 *
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 */


#ifndef __I2C_JZ4770_H__
#define __I2C_JZ4770_H__

// TODO: Use an ioremapped pointer instead.
#define	I2C_BASE(n)		(0xB0050000 | !!(n) << 12 | ((n) & 2) << 13)


/*************************************************************************
 * I2C
 *************************************************************************/
#define	I2C_CTRL(n)		(I2C_BASE(n) + 0x00)
#define	I2C_TAR(n)     		(I2C_BASE(n) + 0x04)
#define	I2C_SAR(n)     		(I2C_BASE(n) + 0x08)
#define	I2C_DC(n)      		(I2C_BASE(n) + 0x10)
#define	I2C_SHCNT(n)		(I2C_BASE(n) + 0x14)
#define	I2C_SLCNT(n)		(I2C_BASE(n) + 0x18)
#define	I2C_FHCNT(n)		(I2C_BASE(n) + 0x1C)
#define	I2C_FLCNT(n)		(I2C_BASE(n) + 0x20)
#define	I2C_INTST(n)		(I2C_BASE(n) + 0x2C)
#define	I2C_INTM(n)		(I2C_BASE(n) + 0x30)
#define I2C_RXTL(n)		(I2C_BASE(n) + 0x38)
#define I2C_TXTL(n)		(I2C_BASE(n) + 0x3c)
#define	I2C_CINTR(n)		(I2C_BASE(n) + 0x40)
#define	I2C_CRXUF(n)		(I2C_BASE(n) + 0x44)
#define	I2C_CRXOF(n)		(I2C_BASE(n) + 0x48)
#define	I2C_CTXOF(n)		(I2C_BASE(n) + 0x4C)
#define	I2C_CRXREQ(n)		(I2C_BASE(n) + 0x50)
#define	I2C_CTXABRT(n)		(I2C_BASE(n) + 0x54)
#define	I2C_CRXDONE(n)		(I2C_BASE(n) + 0x58)
#define	I2C_CACT(n)		(I2C_BASE(n) + 0x5C)
#define	I2C_CSTP(n)		(I2C_BASE(n) + 0x60)
#define	I2C_CSTT(n)		(I2C_BASE(n) + 0x64)
#define	I2C_CGC(n)    		(I2C_BASE(n) + 0x68)
#define	I2C_ENB(n)     		(I2C_BASE(n) + 0x6C)
#define	I2C_STA(n)     		(I2C_BASE(n) + 0x70)
#define I2C_TXFLR(n)		(I2C_BASE(n) + 0x74)
#define I2C_RXFLR(n)		(I2C_BASE(n) + 0x78)
#define	I2C_TXABRT(n)		(I2C_BASE(n) + 0x80)
#define I2C_DMACR(n)            (I2C_BASE(n) + 0x88)
#define I2C_DMATDLR(n)          (I2C_BASE(n) + 0x8c)
#define I2C_DMARDLR(n)          (I2C_BASE(n) + 0x90)
#define	I2C_SDASU(n)		(I2C_BASE(n) + 0x94)
#define	I2C_ACKGC(n)		(I2C_BASE(n) + 0x98)
#define	I2C_ENSTA(n)		(I2C_BASE(n) + 0x9C)
#define I2C_SDAHD(n)		(I2C_BASE(n) + 0xD0)

#define	REG_I2C_CTRL(n)		REG8(I2C_CTRL(n)) /* I2C Control Register (I2C_CTRL) */
#define	REG_I2C_TAR(n)		REG16(I2C_TAR(n)) /* I2C target address (I2C_TAR) */
#define REG_I2C_SAR(n)		REG16(I2C_SAR(n))
#define REG_I2C_DC(n)		REG16(I2C_DC(n))
#define REG_I2C_SHCNT(n)       	REG16(I2C_SHCNT(n))
#define REG_I2C_SLCNT(n)       	REG16(I2C_SLCNT(n))
#define REG_I2C_FHCNT(n)       	REG16(I2C_FHCNT(n))
#define REG_I2C_FLCNT(n)       	REG16(I2C_FLCNT(n))
#define REG_I2C_INTST(n)       	REG16(I2C_INTST(n)) /* i2c interrupt status (I2C_INTST) */
#define REG_I2C_INTM(n)		REG16(I2C_INTM(n)) /* i2c interrupt mask status (I2C_INTM) */
#define REG_I2C_RXTL(n)		REG8(I2C_RXTL(n))
#define REG_I2C_TXTL(n)		REG8(I2C_TXTL(n))
#define REG_I2C_CINTR(n)       	REG8(I2C_CINTR(n))
#define REG_I2C_CRXUF(n)       	REG8(I2C_CRXUF(n))
#define REG_I2C_CRXOF(n)       	REG8(I2C_CRXOF(n))
#define REG_I2C_CTXOF(n)       	REG8(I2C_CTXOF(n))
#define REG_I2C_CRXREQ(n)      	REG8(I2C_CRXREQ(n))
#define REG_I2C_CTXABRT(n)     	REG8(I2C_CTXABRT(n))
#define REG_I2C_CRXDONE(n)     	REG8(I2C_CRXDONE(n))
#define REG_I2C_CACT(n)		REG8(I2C_CACT(n))
#define REG_I2C_CSTP(n)		REG8(I2C_CSTP(n))
#define REG_I2C_CSTT(n)		REG16(I2C_CSTT(n))
#define REG_I2C_CGC(n)		REG8(I2C_CGC(n))
#define REG_I2C_ENB(n)		REG8(I2C_ENB(n))
#define REG_I2C_STA(n)		REG8(I2C_STA(n))
#define REG_I2C_TXFLR(n)	REG8(I2C_TXFLR(n))
#define REG_I2C_RXFLR(n)	REG8(I2C_RXFLR(n))
#define REG_I2C_TXABRT(n)      	REG16(I2C_TXABRT(n))
#define REG_I2C_DMACR(n)        REG8(I2C_DMACR(n))
#define REG_I2C_DMATDLR(n)      REG8(I2C_DMATDLR(n))
#define REG_I2C_DMARDLR(n)      REG8(I2C_DMARDLR(n))
#define REG_I2C_SDASU(n)       	REG8(I2C_SDASU(n))
#define REG_I2C_ACKGC(n)       	REG8(I2C_ACKGC(n))
#define REG_I2C_ENSTA(n)       	REG8(I2C_ENSTA(n))
#define REG_I2C_SDAHD(n)	REG16(I2C_SDAHD(n))

/* I2C Control Register (I2C_CTRL) */

#define I2C_CTRL_STPHLD		(1 << 7)
#define I2C_CTRL_SLVDIS		(1 << 6) /* after reset slave is disabled*/
#define I2C_CTRL_REST		(1 << 5)
#define I2C_CTRL_MATP		(1 << 4) /* 1: 10bit address 0: 7bit addressing*/
#define I2C_CTRL_SATP		(1 << 3) /* standard mode 100kbps */
#define I2C_CTRL_SPDF		(2 << 1) /* fast mode 400kbps */
#define I2C_CTRL_SPDS		(1 << 1) /* standard mode 100kbps */
#define I2C_CTRL_MD		(1 << 0) /* master enabled*/

/* I2C target address (I2C_TAR) */

#define I2C_TAR_MATP		(1 << 12)
#define I2C_TAR_SPECIAL		(1 << 11)
#define I2C_TAR_GC_OR_START	(1 << 10)

/* I2C slave address  */
/* I2C data buffer and command (I2C_DC) */

#define I2C_DC_CMD			(1 << 8) /* 1 read 0  write*/

/* i2c interrupt status (I2C_INTST) */

#define I2C_INTST_IGC			(1 << 11) /* */
#define I2C_INTST_ISTT			(1 << 10)
#define I2C_INTST_ISTP			(1 << 9)
#define I2C_INTST_IACT			(1 << 8)
#define I2C_INTST_RXDN			(1 << 7)
#define I2C_INTST_TXABT			(1 << 6)
#define I2C_INTST_RDREQ			(1 << 5)
#define I2C_INTST_TXEMP			(1 << 4)
#define I2C_INTST_TXOF			(1 << 3)
#define I2C_INTST_RXFL			(1 << 2)
#define I2C_INTST_RXOF			(1 << 1)
#define I2C_INTST_RXUF			(1 << 0)

/* i2c interrupt mask status (I2C_INTM) */

#define I2C_INTM_MIGC			(1 << 11) /* */
#define I2C_INTM_MISTT			(1 << 10)
#define I2C_INTM_MISTP			(1 << 9)
#define I2C_INTM_MIACT			(1 << 8)
#define I2C_INTM_MRXDN			(1 << 7)
#define I2C_INTM_MTXABT			(1 << 6)
#define I2C_INTM_MRDREQ			(1 << 5)
#define I2C_INTM_MTXEMP			(1 << 4)
#define I2C_INTM_MTXOF			(1 << 3)
#define I2C_INTM_MRXFL			(1 << 2)
#define I2C_INTM_MRXOF			(1 << 1)
#define I2C_INTM_MRXUF			(1 << 0)

/* I2C Clear Combined and Individual Interrupts (I2C_CINTR) */

#define I2C_CINTR_CINT

/* I2C Clear TX_OVER Interrupt */
/* I2C Clear RDREQ Interrupt */
/* I2C Clear TX_ABRT Interrupt */
/* I2C Clear RX_DONE Interrupt */
/* I2C Clear ACTIVITY Interrupt */
/* I2C Clear STOP Interrupts */
/* I2C Clear START Interrupts */
/* I2C Clear GEN_CALL Interrupts */

/* I2C Enable (I2C_ENB) */

#define I2C_ENB_I2CENB 		(1 << 0) /* Enable the i2c */

/* I2C Status Register (I2C_STA) */

#define I2C_STA_SLVACT		(1 << 6) /* Slave FSM is not in IDLE state */
#define I2C_STA_MSTACT		(1 << 5) /* Master FSM is not in IDLE state */
#define I2C_STA_RFF		(1 << 4) /* RFIFO if full */
#define I2C_STA_RFNE		(1 << 3) /* RFIFO is not empty */
#define I2C_STA_TFE		(1 << 2) /* TFIFO is empty */
#define I2C_STA_TFNF		(1 << 1) /* TFIFO is not full  */
#define I2C_STA_ACT		(1 << 0) /* I2C Activity Status */

/* I2C Transmit Abort Status Register (I2C_TXABRT) */

#define I2C_TXABRT_SLVRD_INTX		(1 << 15)
#define I2C_TXABRT_SLV_ARBLOST		(1 << 14)
#define I2C_TXABRT_SLVFLUSH_TXFIFO	(1 << 13)
#define I2C_TXABRT_ARB_LOST		(1 << 12)
#define I2C_TXABRT_ABRT_MASTER_DIS	(1 << 11)
#define I2C_TXABRT_ABRT_10B_RD_NORSTRT	(1 << 10)
#define I2C_TXABRT_SBYTE_NORSTRT	(1 << 9)
#define I2C_TXABRT_ABRT_HS_NORSTRT	(1 << 8)
#define I2C_TXABRT_SBYTE_ACKDET		(1 << 7)
#define I2C_TXABRT_ABRT_HS_ACKD		(1 << 6)
#define I2C_TXABRT_ABRT_GCALL_READ	(1 << 5)
#define I2C_TXABRT_ABRT_GCALL_NOACK	(1 << 4)
#define I2C_TXABRT_ABRT_XDATA_NOACK	(1 << 3)
#define I2C_TXABRT_ABRT_10ADDR2_NOACK	(1 << 2)
#define I2C_TXABRT_ABRT_10ADDR1_NOACK	(1 << 1)
#define I2C_TXABRT_ABRT_7B_ADDR_NOACK	(1 << 0)

/* I2C Enable Status Register (I2C_ENSTA) */

#define I2C_ENSTA_SLVRDLST		(1 << 2)
#define I2C_ENSTA_SLVDISB 		(1 << 1)
#define I2C_ENSTA_I2CEN 		(1 << 0) /* when read as 1, i2c is deemed to be in an enabled state
						    when read as 0, i2c is deemed completely inactive. The cpu can
						 safely read this bit anytime .When this bit is read as 0 ,the cpu can
						 safely read SLVRDLST and SLVDISB */

#define I2C_SDASU_SETUP_TIME_BASE	0
#define I2C_SDASU_SETUP_TIME_MASK	0xff

#define I2C_SDAHD_HOLD_TIME_BASE	0
#define I2C_SDAHD_HOLD_TIME_MASK	0xff
#define I2C_SDAHD_HOLD_TIME_EN		(1 << 8)

/***************************************************************************
 * I2C
 ***************************************************************************/

#define __i2c_enable(n)		( REG_I2C_ENB(n) = 1 )
#define __i2c_disable(n)       	( REG_I2C_ENB(n) = 0 )

#define __i2c_is_enable(n)       ( REG_I2C_ENSTA(n) & I2C_ENB_I2CENB )
#define __i2c_is_disable(n)      ( !(REG_I2C_ENSTA(n) & I2C_ENB_I2CENB) )

#define __i2c_abrt(n)            ( REG_I2C_TXABRT(n) != 0 )
#define __i2c_master_active(n)   ( REG_I2C_STA(n) & I2C_STA_MSTACT )
#define __i2c_abrt_7b_addr_nack(n)  ( REG_I2C_TXABRT(n) & I2C_TXABRT_ABRT_7B_ADDR_NOACK )
#define __i2c_txfifo_is_empty(n)     ( REG_I2C_STA(n) & I2C_STA_TFE )
#define __i2c_clear_interrupts(ret,n)    ( ret = REG_I2C_CINTR(n) )

#define __i2c_dma_rd_enable(n)        SETREG8(I2C_DMACR(n),1 << 0)
#define __i2c_dma_rd_disable(n)       CLRREG8(I2C_DMACR(n),1 << 0)
#define __i2c_dma_td_enable(n)        SETREG8(I2C_DMACR(n),1 << 1)
#define __i2c_dma_td_disable(n)       CLRREG8(I2C_DMACR(n),1 << 1)

#define __i2c_send_stop(n)           CLRREG8(I2C_SHCNT(n), I2C_CTRL_STPHLD)
#define __i2c_nsend_stop(n)          SETREG8(I2C_SHCNT(n), I2C_CTRL_STPHLD)

#define __i2c_set_dma_td_level(n,data) OUTREG8(I2C_DMATDLR(n),data)
#define __i2c_set_dma_rd_level(n,data) OUTREG8(I2C_DMARDLR(n),data)

#define __i2c_hold_time_enable(n) SETREG16(I2C_SDAHD(n), I2C_SDAHD_HOLD_TIME_EN)
#define __i2c_hold_time_disable(n) CLRREG16(I2C_SDAHD(n), I2C_SDAHD_HOLD_TIME_EN)
#define __i2c_set_hold_time(n, ht)					\
	do {								\
		CLRREG16(I2C_SDAHD(n), I2C_SDAHD_HOLD_TIME_MASK);	\
		SETREG16(I2C_SDAHD(n), ((ht) & I2C_SDAHD_HOLD_TIME_MASK)); \
	} while(0)

#define __i2c_set_setup_time(n, su)					\
	do {								\
		CLRREG16(I2C_SDASU(n), I2C_SDASU_SETUP_TIME_MASK);	\
		SETREG16(I2C_SDASU(n), ((su) & I2C_SDASU_SETUP_TIME_MASK)); \
	} while(0)

/* I2C standard mode high count register(I2CSHCNT) */
#define I2CSHCNT_ADJUST(n)      (((n) - 8) < 6 ? 6 : ((n) - 8))

/* I2C standard mode low count register(I2CSLCNT) */
#define I2CSLCNT_ADJUST(n)      (((n) - 1) < 8 ? 8 : ((n) - 1))

/* I2C fast mode high count register(I2CFHCNT) */
#define I2CFHCNT_ADJUST(n)      (((n) - 8) < 6 ? 6 : ((n) - 8))

/* I2C fast mode low count register(I2CFLCNT) */
#define I2CFLCNT_ADJUST(n)      (((n) - 1) < 8 ? 8 : ((n) - 1))

/*
#define __i2c_set_clk(dev_clk, i2c_clk) \
  ( REG_I2C_GR = (dev_clk) / (16*(i2c_clk)) - 1 )
*/

#define __i2c_read(n)		( REG_I2C_DC(n) & 0xff )
#define __i2c_write(val,n)	( REG_I2C_DC(n) = (val) )

#endif /* __I2C_JZ4770_H__ */
