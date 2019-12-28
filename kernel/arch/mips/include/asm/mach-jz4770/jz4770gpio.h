/*
 * linux/include/asm-mips/mach-jz4770/jz4770gpio.h
 *
 * JZ4770 GPIO register definition.
 *
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ4770GPIO_H__
#define __JZ4770GPIO_H__

#include <asm/addrspace.h>
#include <asm/mach-jz4770/base.h>
#include <asm/mach-jz4770/jz4770misc.h>


/***************************************************************************
 * GPIO
 ***************************************************************************/

//------------------------------------------------------
// GPIO Pins Description
//
// PORT 0:
//
// PIN/BIT N	FUNC0		FUNC1		FUNC2         NOTE
//	0	SD0		-		-
//	1	SD1		-		-
//	2	SD2		-		-
//	3	SD3		-		-
//	4	SD4		-		-
//	5	SD5		-		-
//	6	SD6		-		-
//	7	SD7		-		-
//	8	SD8		-		-
//	9	SD9		-		-
//	10	SD10		-		-
//	11	SD11		-		-
//	12	SD12		-		-
//	13	SD13		-		-
//	14	SD14		-		-
//	15	SD15		-		-
//      16      RD_             -		-
//      17      WE_             -		-
//      18      FRE_            MSC0_CLK        SSI0_CLK
//      19      FWE_            MSC0_CMD        SSI0_CE0_
//      20      MSC0_D0         SSI0_DR		-	       1
//      21      CS1_            MSC0_D1         SSI0_DT
//      22      CS2_            MSC0_D2		-
//      23      CS3_		-		-
//      24      CS4_ 		-		-
//      25      CS5_		-		-
//      26      CS6_		-		-
//      27      WAIT_		-		-
//      28      DREQ0		-		-
//      29      DACK0           OWI		-
//      30	-		-		-	     6
//      31	-		-		-	     7

//Note1. PA20: GPIO group A bit 20. If NAND flash is used, this pin must be used as NAND FRB. (NAND flash ready/busy)
//Note6. PA30: GPIO group A bit 30 can only be used as input and interrupt, no pull-up and pull-down.
//Note7. PA31: GPIO group A bit 31. No corresponding pin exists for this GPIO. It is only used to select the function between UART and JTAG, which share the same set of pins, by using register PASEL [31]
//       When PASEL [31]=0, select JTAG function.
//       When PASEL [31]=1, select UART function

//------------------------------------------------------
// PORT 1:
//
// PIN/BIT N	FUNC0		FUNC1	       FUNC2         NOTE
//	0	SA0		-              -
//	1	SA1		-              -
//	2	SA2		-              -             CL
//	3	SA3		-              -             AL
//	4	SA4		-              -
//	5	SA5		-              -
//	6	CIM_PCLK 	TSCLK	       -
//	7	CIM_HSYN  	TSFRM          -
//	8	CIM_VSYN 	TSSTR          -
//	9	CIM_MCLK 	TSFAIL         -
//	10	CIM_D0 	 	TSDI0          -
//	11	CIM_D1 	 	TSDI1          -
//	12	CIM_D2 	 	TSDI2          -
//	13	CIM_D3 		TSDI3          -
//	14	CIM_D4 		TSDI4          -
//	15	CIM_D5		TSDI5          -
//	16 	CIM_D6 		TSDI6          -
//	17 	CIM_D7 		TSDI7          -
//	18	-               -	       -
//	19	-               -	       -
//	20 	MSC2_D0 	SSI2_DR        TSDI0
//	21 	MSC2_D1 	SSI2_DT        TSDI1
//	22 	TSDI2		-              -
//	23 	TSDI3		-              -
//	24 	TSDI4		-              -
//	25 	TSDI5		-              -
//	26 	TSDI6		-              -
//	27 	TSDI7		-              -
//	28 	MSC2_CLK        SSI2_CLK       TSCLK
//	29 	MSC2_CMD        SSI2_CE0_      TSSTR
//	30 	MSC2_D2         SSI2_GPC       TSFAIL
//	31 	MSC2_D3         SSI2_CE1_      TSFRM

//------------------------------------------------------
// PORT 2:
// PIN/BIT N	FUNC0		FUNC1		FUNC2 		FUNC3		NOTE
//	0	LCD_B0 (O)	LCD_REV (O)	-               -
//	1	LCD_B1 (O)	LCD_PS (O)	-               -
//	2	LCD_B2 (O)	-               -	        -
//	3	LCD_B3 (O)	-               -	        -
//	4	LCD_B4 (O)	-               -	        -
//	5	LCD_B5 (O)	-               -	        -
//	6	LCD_B6 (O)	-               -	        -
//	7	LCD_B7 (O)	-               -	        -
//	8	LCD_PCLK (O)	-               -	        -
//	9	LCD_DE (O)	-               -	        -
//	10	LCD_G0 (O)	LCD_SPL (O)	-               -
//	11	LCD_G1 (O)	-               -	        -
//	12	LCD_G2 (O)	-               -	        -
//	13	LCD_G3 (O)	-               -	        -
//	14	LCD_G4 (O)	-               -	        -
//	15	LCD_G5 (O)	-               -	        -
//	16	LCD_G6 (O)	-               -	        -
//	17	LCD_G7 (O)	-               -	        -
//	18	LCD_HSYN (IO)	-               -	        -
//	19	LCD_VSYN (IO)	-               -	        -
//	20	LCD_R0 (O)	LCD_CLS (O)	-               -
//	21	LCD_R1 (O)	-               -	        -
//	22	LCD_R2 (O)	-               -	        -
//	23	LCD_R3 (O)	-               -	        -
//	24	LCD_R4 (O)	-               -	        -
//	25	LCD_R5 (O)	-               -	        -
//	26	LCD_R6 (O)	-               -	        -
//	27	LCD_R7 (O)	-               -	        -
//	28	UART2_RxD (I)	-               -	        -
//	29	UART2_CTS_ (I)	-               -	        -
//	30	UART2_TxD (O)	-               -	        -
//	31	UART2_RTS_ (O)	-               -	        -

//------------------------------------------------------
// PORT 3:
//
// PIN/BIT N	FUNC0		FUNC1		FUNC2 		FUNC3		NOTE
//	0 	MII_TXD0 	-     		-  		-
//	1 	MII_TXD1	-     		-  		-
//	2 	MII_TXD2	-     		-  		-
//	3 	MII_TXD3	- 		-  		-
//	4 	MII_TXEN	-		-		-
//	5  MII_TXCLK(RMII_CLK)	-		-  		-
//	6 	MII_COL   	-		-  		-
//	7 	MII_RXER	-		-  		-
//	8 	MII_RXDV	-		-  		-
//	9 	MII_RXCLK	-		-		-
//	10 	MII_RXD0	-		-  		-
//	11 	MII_RXD1	-		-  		-
//	12 	MII_RXD2	-		-  		-
//	13 	MII_RXD3	-		-  		-
//	14 	MII_CRS		-		-  		-
//	15 	MII_MDC		-		-  		-
//	16 	MII_MDIO	-		-  		-
//	17 	BOOT_SEL0	-		-  		-		Note2,5
//	18 	BOOT_SEL1	-		-  		- 		Note3,5
//	19 	BOOT_SEL2	-		-  		- 		Note4,5
//	20 	MSC1_D0 	SSI1_DR 	-  		-
//	21	MSC1_D1 	SSI1_DT 	-  		-
//	22 	MSC1_D2  	SSI1_GPC 	-  		-
//	23 	MSC1_D3  	SSI1_CE1_ 	-  		-
//	24 	MSC1_CLK  	SSI1_CLK 	-  		-
//	25 	MSC1_CMD  	SSI1_CE0_ 	-  		-
//	26 	UART1_RxD 	-		-  		-
//	27 	UART1_CTS_ 	-		-  		-
//	28 	UART1_TxD 	-		-  		-
//	29 	UART1_RTS_ 	-		-  		-
//	30 	I2C0_SDA 	-		-  		-
//	31 	I2C0_SCK 	-		-  		-
//
// Note2. PD17: GPIO group D bit 17 is used as BOOT_SEL0 input during boot.
// Note3. PD18: GPIO group D bit 18 is used as BOOT_SEL1 input during boot.
// Note4. PD19: GPIO group D bit 19 is used as BOOT_SEL2 input during boot.
// Note5. BOOT_SEL2, BOOT_SEL1, BOOT_SEL0 are used to select boot source and function during the processor boot.
//
//------------------------------------------------------
// PORT 4:
//
// PIN/BIT N	FUNC0		FUNC1	       FUNC2         FUNC3         NOTE
//	0  	PWM0		- 		- 		-
//	1  	PWM1		- 		- 		-
//	2  	PWM2 		SYNC 		- 		-
//	3  	PWM3 		UART3_RxD 	BCLK 		-
//	4  	PWM4 		- 		- 		-
//	5  	PWM5 		UART3_TxD 	SCLK_RSTN 	-
//	6  	SDATI		- 		- 		-
//	7  	SDATO 		- 		- 		-
//	8  	UART3_CTS_ 	- 		- 		-
//	9  	UART3_RTS_ 	- 		- 		-
//	10  	- 		- 		- 		-
//	11  	SDATO1 		- 		- 		-
//	12  	SDATO2 		- 		- 		-
//	13  	SDATO3 		-		-		-
//	14  	SSI0_DR 	SSI1_DR 	SSI2_DR 	-
//	15  	SSI0_CLK 	SI1_CLK 	SSI2_CLK 	-
//	16  	SSI0_CE0_ 	SI1_CE0_ 	SSI2_CE0_ 	-
//	17  	SSI0_DT 	SSI1_DT 	SSI2_DT 	-
//	18  	SSI0_CE1_ 	SSI1_CE1_ 	SSI2_CE1_ 	-
//	19  	SSI0_GPC 	SSI1_GPC 	SSI2_GPC 	-
//	20  	MSC0_D0 	MSC1_D0 	MSC2_D0 	-
//	21  	MSC0_D1 	MSC1_D1 	MSC2_D1 	-
//	22  	MSC0_D2 	MSC1_D2 	MSC2_D2 	-
//	23  	MSC0_D3 	MSC1_D3 	MSC2_D3 	-
//	24  	MSC0_CLK 	MSC1_CLK 	MSC2_CLK 	-
//	25  	MSC0_CMD 	MSC1_CMD 	MSC2_CMD 	-
//	26  	MSC0_D4 	MSC0_D4 	MSC0_D4 	PS2_MCLK
//	27  	MSC0_D5 	MSC0_D5 	MSC0_D5 	PS2_MDATA
//	28  	MSC0_D6 	MSC0_D6 	MSC0_D6 	PS2_KCLK
//	29  	MSC0_D7 	MSC0_D7 	MSC0_D7 	PS2_KDATA
//	30  	I2C1_SDA 	SCC_DATA 	- 		-
//	31  	I2C1_SCK 	SCC_CLK 	- 		-
//
//------------------------------------------------------
// PORT 5:
//
// PIN/BIT N	FUNC0		FUNC1		FUNC2		FUNC3		NOTE
//	0   	UART0_RxD 	GPS_CLK 	- 		-
//	1   	UART0_CTS_ 	GPS_MAG 	- 		-
//	2   	UART0_TxD 	GPS_SIG 	- 		-
//	3   	UART0_RTS_ 	- 		-		-
//
//////////////////////////////////////////////////////////



#define	GPIO_BASE	CKSEG1ADDR(JZ4770_GPIO_BASE_ADDR)

/* GPIO group offset */
#define GPIO_GOS	0x100


#define GPIO_PXFUNS_OFFSET	(0x44)  /*  w, 32, 0x???????? */
#define GPIO_PXSELC_OFFSET	(0x58)  /*  w, 32, 0x???????? */
#define GPIO_PXPES_OFFSET	(0x34)  /*  w, 32, 0x???????? */
#define GPIO_PXFUNC_OFFSET	(0x48)  /*  w, 32, 0x???????? */
#define GPIO_PXPEC_OFFSET	(0x38)  /*  w, 32, 0x???????? */
#define GPIO_PXIMC_OFFSET	(0x28)  /*  w, 32, 0x???????? */
#define GPIO_PXDATC_OFFSET	(0x18)  /*  w, 32, 0x???????? */

#define GPIO_PXDATC(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXDATC_OFFSET)
#define GPIO_PXIMC(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXIMC_OFFSET)
#define GPIO_PXPEC(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXPEC_OFFSET)
#define GPIO_PXFUNC(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXFUNC_OFFSET)


#define GPIO_PXFUNS(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXFUNS_OFFSET)
#define GPIO_PXSELC(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXSELC_OFFSET)
#define GPIO_PXPES(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXPES_OFFSET)

#define REG_GPIO_PXFUNS(n)	REG32(GPIO_PXFUNS(n))
#define REG_GPIO_PXFUNC(n)	REG32(GPIO_PXFUNC(n))
#define REG_GPIO_PXSELC(n)	REG32(GPIO_PXSELC(n))
#define REG_GPIO_PXPES(n)	REG32(GPIO_PXPES(n))
#define REG_GPIO_PXPEC(n)	REG32(GPIO_PXPEC(n))

/*************************************************************************
 * GPIO (General-Purpose I/O Ports)
 *************************************************************************/
#define GPIO_BASEA GPIO_BASE +(0)*0x100
#define GPIO_BASEB GPIO_BASE +(1)*0x100
#define GPIO_BASEC GPIO_BASE +(2)*0x100
#define GPIO_BASED GPIO_BASE +(3)*0x100
#define GPIO_BASEE GPIO_BASE +(4)*0x100
#define GPIO_BASEF GPIO_BASE +(5)*0x100

#define GPA(x) 		(32 * 0 + (x))
#define GPB(x) 		(32 * 1 + (x))
#define GPC(x) 		(32 * 2 + (x))
#define GPD(x) 		(32 * 3 + (x))
#define GPE(x) 		(32 * 4 + (x))
#define GPF(x) 		(32 * 5 + (x))

#define GPIO_PORT_NUM   6
#define MAX_GPIO_NUM	192
#define GPIO_WAKEUP     (30)  //WAKE_UP key PA30

//n = 0,1,2,3,4,5 (PORTA, PORTB, PORTC, PORTD, PORTE, PORTF)
#define GPIO_PXPIN(n)	(GPIO_BASE + (0x00 + (n)*0x100)) /* PIN Level Register */
#define GPIO_PXINT(n)	(GPIO_BASE + (0x10 + (n)*0x100)) /* Port Interrupt Register */
#define GPIO_PXINTS(n)	(GPIO_BASE + (0x14 + (n)*0x100)) /* Port Interrupt Set Register */
#define GPIO_PXINTC(n)	(GPIO_BASE + (0x18 + (n)*0x100)) /* Port Interrupt Clear Register */

#define GPIO_PXMASK(n)	(GPIO_BASE + (0x20 + (n)*0x100)) /* Port Interrupt Mask Register */
#define GPIO_PXMASKS(n)	(GPIO_BASE + (0x24 + (n)*0x100)) /* Port Interrupt Mask Set Reg */
#define GPIO_PXMASKC(n)	(GPIO_BASE + (0x28 + (n)*0x100)) /* Port Interrupt Mask Clear Reg */

#define GPIO_PXPAT1(n)	(GPIO_BASE + (0x30 + (n)*0x100)) /* Port Pattern 1 Register */
#define GPIO_PXPAT1S(n)	(GPIO_BASE + (0x34 + (n)*0x100)) /* Port Pattern 1 Set Reg. */
#define GPIO_PXPAT1C(n)	(GPIO_BASE + (0x38 + (n)*0x100)) /* Port Pattern 1 Clear Reg. */
#define GPIO_PXPAT0(n)	(GPIO_BASE + (0x40 + (n)*0x100)) /* Port Pattern 0 Register */
#define GPIO_PXPAT0S(n)	(GPIO_BASE + (0x44 + (n)*0x100)) /* Port Pattern 0 Set Register */
#define GPIO_PXPAT0C(n)	(GPIO_BASE + (0x48 + (n)*0x100)) /* Port Pattern 0 Clear Register */
#define GPIO_PXFLG(n)	(GPIO_BASE + (0x50 + (n)*0x100)) /* Port Flag Register */
#define GPIO_PXFLGC(n)	(GPIO_BASE + (0x58 + (n)*0x100)) /* Port Flag clear Register */
#define GPIO_PXOEN(n)	(GPIO_BASE + (0x60 + (n)*0x100)) /* Port Output Disable Register */
#define GPIO_PXOENS(n)	(GPIO_BASE + (0x64 + (n)*0x100)) /* Port Output Disable Set Register */
#define GPIO_PXOENC(n)	(GPIO_BASE + (0x68 + (n)*0x100)) /* Port Output Disable Clear Register */
#define GPIO_PXPEN(n)	(GPIO_BASE + (0x70 + (n)*0x100)) /* Port Pull Disable Register */
#define GPIO_PXPENS(n)	(GPIO_BASE + (0x74 + (n)*0x100)) /* Port Pull Disable Set Register */
#define GPIO_PXPENC(n)	(GPIO_BASE + (0x78 + (n)*0x100)) /* Port Pull Disable Clear Register */
#define GPIO_PXDS(n)	(GPIO_BASE + (0x80 + (n)*0x100)) /* Port Drive Strength Register */
#define GPIO_PXDSS(n)	(GPIO_BASE + (0x84 + (n)*0x100)) /* Port Drive Strength set Register */
#define GPIO_PXDSC(n)	(GPIO_BASE + (0x88 + (n)*0x100)) /* Port Drive Strength clear Register */

#define REG_GPIO_PXPIN(n)	REG32(GPIO_PXPIN((n)))  /* PIN level */
#define REG_GPIO_PXINT(n)	REG32(GPIO_PXINT((n)))  /* 1: interrupt pending */
#define REG_GPIO_PXINTS(n)	REG32(GPIO_PXINTS((n)))
#define REG_GPIO_PXINTC(n)	REG32(GPIO_PXINTC((n)))

#define REG_GPIO_PXMASK(n)	REG32(GPIO_PXMASK((n)))   /* 1: mask pin interrupt */
#define REG_GPIO_PXMASKS(n)	REG32(GPIO_PXMASKS((n)))
#define REG_GPIO_PXMASKC(n)	REG32(GPIO_PXMASKC((n)))
#define REG_GPIO_PXMASK(n)	REG32(GPIO_PXMASK((n)))   /* 1: mask pin interrupt */
#define REG_GPIO_PXMASKS(n)	REG32(GPIO_PXMASKS((n)))
#define REG_GPIO_PXMASKC(n)	REG32(GPIO_PXMASKC((n)))
#define REG_GPIO_PXPAT1(n)	REG32(GPIO_PXPAT1((n)))   /* 1: disable pull up/down */
#define REG_GPIO_PXPAT1S(n)	REG32(GPIO_PXPAT1S((n)))
#define REG_GPIO_PXPAT1C(n)	REG32(GPIO_PXPAT1C((n)))
#define REG_GPIO_PXPAT0(n)	REG32(GPIO_PXPAT0((n)))  /* 0:GPIO/INTR, 1:FUNC */
#define REG_GPIO_PXPAT0S(n)	REG32(GPIO_PXPAT0S((n)))
#define REG_GPIO_PXPAT0C(n)	REG32(GPIO_PXPAT0C((n)))
#define REG_GPIO_PXFLG(n)	REG32(GPIO_PXFLG((n))) /* 0:GPIO/Fun0,1:intr/fun1*/
#define REG_GPIO_PXFLGC(n)	REG32(GPIO_PXFLGC((n)))
#define REG_GPIO_PXOEN(n)	REG32(GPIO_PXOEN((n)))
#define REG_GPIO_PXOENS(n)	REG32(GPIO_PXOENS((n))) /* 0:input/low-level-trig/falling-edge-trig, 1:output/high-level-trig/rising-edge-trig */
#define REG_GPIO_PXOENC(n)	REG32(GPIO_PXOENC((n)))
#define REG_GPIO_PXPEN(n)	REG32(GPIO_PXPEN((n)))
#define REG_GPIO_PXPENS(n)	REG32(GPIO_PXPENS((n))) /* 0:Level-trigger/Fun0, 1:Edge-trigger/Fun1 */
#define REG_GPIO_PXPENC(n)	REG32(GPIO_PXPENC((n)))
#define REG_GPIO_PXDS(n)	REG32(GPIO_PXDS((n)))
#define REG_GPIO_PXDSS(n)	REG32(GPIO_PXDSS((n))) /* interrupt flag */
#define REG_GPIO_PXDSC(n)	REG32(GPIO_PXDSC((n))) /* interrupt flag */

#ifndef __MIPS_ASSEMBLER

/*----------------------------------------------------------------
 * p is the port number (0,1,2,3,4,5)
 * o is the pin offset (0-31) inside the port
 * n is the absolute number of a pin (0-127), regardless of the port
 */

//----------------------------------------------------------------
// Function Pins Mode
#define __gpio_as_func0(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXINTC(p) = (1 << o);		\
	REG_GPIO_PXMASKC(p) = (1 << o);		\
	REG_GPIO_PXPAT1C(p) = (1 << o);		\
	REG_GPIO_PXPAT0C(p) = (1 << o);		\
} while (0)

#define __gpio_as_func1(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXINTC(p) = (1 << o);		\
	REG_GPIO_PXMASKC(p) = (1 << o);		\
	REG_GPIO_PXPAT1C(p) = (1 << o);		\
	REG_GPIO_PXPAT0S(p) = (1 << o);		\
} while (0)

#define __gpio_as_func2(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXINTC(p) = (1 << o);		\
	REG_GPIO_PXMASKC(p) = (1 << o);		\
	REG_GPIO_PXPAT1S(p) = (1 << o);		\
	REG_GPIO_PXPAT0C(p) = (1 << o);		\
} while (0)

#define __gpio_as_func3(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXINTC(p) = (1 << o);		\
	REG_GPIO_PXMASKC(p) = (1 << o);		\
	REG_GPIO_PXPAT1S(p) = (1 << o);		\
	REG_GPIO_PXPAT0S(p) = (1 << o);		\
} while (0)

#if 0
/*
 * MII_TXD0- D3 MII_TXEN MII_TXCLK MII_COL
 * MII_RXER MII_RXDV MII_RXCLK MII_RXD0 - D3
 * MII_CRS MII_MDC MII_MDIO
 */
#define __gpio_as_eth()				\
do {						\
	REG_GPIO_PXINTC(1) =  0x00000010;	\
	REG_GPIO_PXMASKC(1) = 0x00000010;	\
	REG_GPIO_PXPAT1S(1) = 0x00000010;	\
	REG_GPIO_PXPAT0C(1) = 0x00000010;	\
	REG_GPIO_PXINTC(3) =  0x3c000000;	\
	REG_GPIO_PXMASKC(3) = 0x3c000000;	\
	REG_GPIO_PXPAT1C(3) = 0x3c000000;	\
	REG_GPIO_PXPAT0S(3) = 0x3c000000;	\
	REG_GPIO_PXINTC(5) =  0x0000fff0;	\
	REG_GPIO_PXMASKC(5) = 0x0000fff0;	\
	REG_GPIO_PXPAT1C(5) = 0x0000fff0;	\
	REG_GPIO_PXPAT0C(5) = 0x0000fff0;	\
} while (0)
#endif

#define __gpio_as_eth()				\
do {						\
	REG_GPIO_PXINTC(1) =  0x00000010;	\
	REG_GPIO_PXMASKC(1) = 0x00000010;	\
	REG_GPIO_PXPAT1S(1) = 0x00000010;	\
	REG_GPIO_PXPAT0C(1) = 0x00000010;	\
	REG_GPIO_PXINTC(3) =  0x3c000000;	\
	REG_GPIO_PXMASKC(3) = 0x3c000000;	\
	REG_GPIO_PXPAT1C(3) = 0x3c000000;	\
	REG_GPIO_PXPAT0S(3) = 0x3c000000;	\
	REG_GPIO_PXINTC(5) =  0x0000fff0;	\
	REG_GPIO_PXMASKC(5) = 0x0000fff0;	\
	REG_GPIO_PXPAT1C(5) = 0x0000fff0;	\
	REG_GPIO_PXPAT0C(5) = 0x0000fff0;	\
} while (0)


/*
 * UART0_TxD, UART0_RxD
 */
#define __gpio_as_uart0()			\
do {						\
	REG_GPIO_PXINTC(5) = 0x00000009;	\
	REG_GPIO_PXMASKC(5) = 0x00000009;	\
	REG_GPIO_PXPAT1C(5) = 0x00000009;	\
	REG_GPIO_PXPAT0C(5) = 0x00000009;	\
} while (0)


/*
 * UART0_TxD, UART0_RxD, UART0_CTS, UART0_RTS
 */
#define __gpio_as_uart0_ctsrts()		\
do {						\
	REG_GPIO_PXFUN1S(5) = 0x0000000f;	\
	REG_GPIO_PXTRGC(5) = 0x0000000f;	\
	REG_GPIO_PXSELC(5) = 0x0000000f;	\
	REG_GPIO_PXPENS(5) = 0x0000000f;		\
} while (0)
/*
 * GPS_CLK GPS_MAG GPS_SIG
 */
#define __gpio_as_gps()			\
do {						\
	REG_GPIO_PXFUNS(5) = 0x00000007;	\
	REG_GPIO_PXTRGC(5) = 0x00000007;	\
	REG_GPIO_PXSELS(5) = 0x00000007;	\
	REG_GPIO_PXPENS(5) = 0x00000007;		\
} while (0)

/*
 * UART1_TxD, UART1_RxD
 */
#define __gpio_as_uart1()			\
do {						\
	REG_GPIO_PXINTC(3) = 0x14000000;	\
	REG_GPIO_PXMASKC(3) = 0x14000000;	\
	REG_GPIO_PXPAT0C(3) = 0x14000000;	\
	REG_GPIO_PXPAT1C(3)  = 0x14000000;	\
} while (0)

/*
 * UART1_TxD, UART1_RxD, UART1_CTS, UART1_RTS
 */
#define __gpio_as_uart1_ctsrts()		\
do {						\
	REG_GPIO_PXFUNS(3) = 0x3c000000;	\
	REG_GPIO_PXTRGC(3) = 0x3c000000;	\
	REG_GPIO_PXSELC(3) = 0x3c000000;	\
	REG_GPIO_PXPENS(3)  = 0x3c000000;	\
} while (0)

/*
 * UART2_TxD, UART2_RxD
 */
#define __gpio_as_uart2()			\
do {						\
	REG_GPIO_PXINTC(2) = 0x50000000;	\
	REG_GPIO_PXMASKC(2) = 0x50000000;	\
	REG_GPIO_PXPAT1C(2) = 0x50000000;	\
	REG_GPIO_PXPAT0C(2)  = 0x50000000;	\
} while (0)

/*
 * UART2_TxD, UART2_RxD, UART2_CTS, UART2_RTS
 */
#define __gpio_as_uart2_ctsrts()		\
do {						\
	REG_GPIO_PXFUNS(2) = 0xf0000000;	\
	REG_GPIO_PXTRGC(2) = 0xf0000000;	\
	REG_GPIO_PXSELC(2) = 0xf0000000;	\
	REG_GPIO_PXPENS(2)  = 0xf0000000;	\
} while (0)

/*
 * UART3_TxD, UART3_RxD
 */
#define __gpio_as_uart3()			\
do {						\
	REG_GPIO_PXFUNS(4) = 0x00000028;	\
	REG_GPIO_PXTRGC(4) = 0x00000028;	\
	REG_GPIO_PXSELS(4) = 0x00000028;	\
	REG_GPIO_PXPENS(4)  = 0x00000028;	\
} while (0)

/*
 * UART3_TxD, UART3_RxD, UART3_CTS, UART3_RTS
 */
#define __gpio_as_uart3_ctsrts()		\
do {						\
	REG_GPIO_PXFUNS(4) = 0x00000028;	\
	REG_GPIO_PXTRGC(4) = 0x00000028;	\
	REG_GPIO_PXSELS(4) = 0x00000028;	\
	REG_GPIO_PXFUNS(4) = 0x00000300;	\
	REG_GPIO_PXTRGC(4) = 0x00000300;	\
	REG_GPIO_PXSELC(4) = 0x00000300;	\
	REG_GPIO_PXPENS(4)  = 0x00000328;	\
}

/*
 * SD0 ~ SD7, CS1#, CLE, ALE, FRE#, FWE#, FRB#
 * @n: chip select number(1 ~ 6)
 */
#define __gpio_as_nand_8bit(n)						\
do {		              						\
									\
	REG_GPIO_PXINTC(0) = 0x000c00ff; /* SD0 ~ SD7, CS1#, FRE#, FWE# */ \
	REG_GPIO_PXMASKC(0) = 0x000c00ff;				\
	REG_GPIO_PXPAT1C(0) = 0x000c00ff;				\
	REG_GPIO_PXPAT0C(0) = 0x000c00ff;				\
	REG_GPIO_PXPENS(0) = 0x000c00ff;				\
									\
	REG_GPIO_PXINTC(1) = 0x00000003; /* CLE(SA2), ALE(SA3) */	\
	REG_GPIO_PXMASKC(1) = 0x00000003;				\
	REG_GPIO_PXPAT1C(1) = 0x00000003;				\
	REG_GPIO_PXPAT0C(1) = 0x00000003;				\
	REG_GPIO_PXPENS(1) = 0x00000003;				\
									\
	REG_GPIO_PXINTC(0) = 0x00200000 << ((n)-1); /* CSn */		\
	REG_GPIO_PXMASKC(0) = 0x00200000 << ((n)-1);			\
	REG_GPIO_PXPAT1C(0) = 0x00200000 << ((n)-1);			\
	REG_GPIO_PXPAT0C(0) = 0x00200000 << ((n)-1);			\
	REG_GPIO_PXPENS(0) = 0x00200000 << ((n)-1);			\
									\
	REG_GPIO_PXINTC(0) = 0x00100000; /* FRB#(input) */		\
	REG_GPIO_PXMASKS(0) = 0x00100000;				\
	REG_GPIO_PXPAT1S(0) = 0x00100000;				\
	REG_GPIO_PXPENS(0) = 0x00100000;				\
} while (0)

#define __gpio_as_nand_16bit(n)						\
do {		              						\
									\
	REG_GPIO_PXINTC(0) = 0x000cffff; /* SD0 ~ SD15, CS1#, FRE#, FWE# */ \
	REG_GPIO_PXMASKC(0) = 0x000cffff;				\
	REG_GPIO_PXPAT1C(0) = 0x000cffff;				\
	REG_GPIO_PXPAT0C(0) = 0x000cffff;				\
	REG_GPIO_PXPENS(0) = 0x000cffff;				\
									\
	REG_GPIO_PXINTC(1) = 0x00000003; /* CLE(SA2), ALE(SA3) */	\
	REG_GPIO_PXMASKC(1) = 0x00000003;				\
	REG_GPIO_PXPAT1C(1) = 0x00000003;				\
	REG_GPIO_PXPAT0C(1) = 0x00000003;				\
	REG_GPIO_PXPENS(1) = 0x00000003;				\
									\
	REG_GPIO_PXINTC(0) = 0x00200000 << ((n)-1); /* CSn */		\
	REG_GPIO_PXMASKC(0) = 0x00200000 << ((n)-1);			\
	REG_GPIO_PXPAT1C(0) = 0x00200000 << ((n)-1);			\
	REG_GPIO_PXPAT0C(0) = 0x00200000 << ((n)-1);			\
	REG_GPIO_PXPENS(0) = 0x00200000 << ((n)-1);			\
									\
	REG_GPIO_PXINTC(0) = 0x00100000; /* FRB#(input) */		\
	REG_GPIO_PXMASKS(0) = 0x00100000;				\
	REG_GPIO_PXPAT1S(0) = 0x00100000;				\
	REG_GPIO_PXPENS(0) = 0x00100000;					\
} while (0)

/*
 * CS4#, RD#, WR#, WAIT#, A0 ~ A22, D0 ~ D7
 * @n: chip select number(1 ~ 4)
 */
#define __gpio_as_nor_8bit(n)						\
do {								        \
	/* 32/16-bit data bus */					\
	REG_GPIO_PXFUNS(0) = 0x000000ff;				\
	REG_GPIO_PXSELC(0) = 0x000000ff;				\
	REG_GPIO_PXPENS(0) = 0x000000ff;					\
									\
	REG_GPIO_PXFUNS(2) = 0x00200000 << ((n)-1); /* CSn */		\
	REG_GPIO_PXSELC(2) = 0x00200000 << ((n)-1);			\
	REG_GPIO_PXPENS(2) = 0x00200000 << ((n)-1);			\
									\
	REG_GPIO_PXFUNS(1) = 0x0000ffff; /* A0~A15 */			\
	REG_GPIO_PXSELC(1) = 0x0000ffff;				\
	REG_GPIO_PXPENS(1) = 0x0000ffff;					\
	REG_GPIO_PXFUNS(2) = 0x06110007; /* RD#, WR#, WAIT#, A20~A22 */	\
	REG_GPIO_PXSELC(2) = 0x06110007;				\
	REG_GPIO_PXPENS(2) = 0x06110007;					\
	REG_GPIO_PXFUNS(2) = 0x000e0000; /* A17~A19 */	        	\
	REG_GPIO_PXSELS(2) = 0x000e0000;				\
	REG_GPIO_PXPENS(2) = 0x000e0000;					\
} while (0)

/*
 * CS4#, RD#, WR#, WAIT#, A0 ~ A22, D0 ~ D15
 * @n: chip select number(1 ~ 4)
 */
#define __gpio_as_nor_16bit(n)						\
do {	               							\
	/* 32/16-bit data normal order */				\
	REG_GPIO_PXFUNS(0) = 0x0000ffff;				\
	REG_GPIO_PXSELC(0) = 0x0000ffff;				\
	REG_GPIO_PXPENS(0) = 0x0000ffff;					\
									\
	REG_GPIO_PXFUNS(2) = 0x00200000 << ((n)-1); /* CSn */		\
	REG_GPIO_PXSELC(2) = 0x00200000 << ((n)-1);			\
	REG_GPIO_PXPENS(2) = 0x00200000 << ((n)-1);			\
									\
	REG_GPIO_PXFUNS(1) = 0x0000ffff; /* A0~A15 */			\
	REG_GPIO_PXSELC(1) = 0x0000ffff;				\
	REG_GPIO_PXPENS(1) = 0x0000ffff;					\
	REG_GPIO_PXFUNS(2) = 0x06110007; /* RD#, WR#, WAIT#, A20~A22 */	\
	REG_GPIO_PXSELC(2) = 0x06110007;				\
	REG_GPIO_PXPENS(2) = 0x06110007;					\
	REG_GPIO_PXFUNS(2) = 0x000e0000; /* A17~A19 */	        	\
	REG_GPIO_PXSELS(2) = 0x000e0000;				\
	REG_GPIO_PXPENS(2) = 0x000e0000;					\
} while (0)

/*
 * LCD_D0~LCD_D7, LCD_PCLK, LCD_HSYNC, LCD_VSYNC, LCD_DE
 */
#define __gpio_as_lcd_8bit()			\
do {						\
	REG_GPIO_PXINTC(2) = 0x000c03ff;	\
	REG_GPIO_PXMASKC(2)  = 0x000c03ff;		\
	REG_GPIO_PXPAT0C(2) = 0x000c03ff;	\
	REG_GPIO_PXPAT1C(2) = 0x000c03ff;		\
} while (0)

/*
 * LCD_R3~LCD_R7, LCD_G2~LCD_G7, LCD_B3~LCD_B7,
 * LCD_PCLK, LCD_HSYNC, LCD_VSYNC, LCD_DE
 */
#define __gpio_as_lcd_16bit()			\
do {						\
	REG_GPIO_PXINTC(2) = 0x0f8ff3f8;	\
	REG_GPIO_PXMASKC(2) = 0x0f8ff3f8;	\
	REG_GPIO_PXPAT0C(2) = 0x0f8ff3f8;	\
	REG_GPIO_PXPAT1C(2) = 0x0f8ff3f8;		\
} while (0)

/*
 * LCD_R2~LCD_R7, LCD_G2~LCD_G7, LCD_B2~LCD_B7,
 * LCD_PCLK, LCD_HSYNC, LCD_VSYNC, LCD_DE
 */
#define __gpio_as_lcd_18bit()			\
do {						\
	REG_GPIO_PXINTC(2) = 0x0fcff3fc;	\
	REG_GPIO_PXMASKC(2) = 0x0fcff3fc;	\
	REG_GPIO_PXPAT0C(2) = 0x0fcff3fc;	\
	REG_GPIO_PXPAT1C(2) = 0x0fcff3fc;		\
} while (0)

/*
 * LCD_R0~LCD_R7, LCD_G0~LCD_G7, LCD_B0~LCD_B7,
 * LCD_PCLK, LCD_HSYNC, LCD_VSYNC, LCD_DE
 */
#define __gpio_as_lcd_24bit()			\
do {						\
	REG_GPIO_PXINTC(2) = 0x0fffffff;	\
	REG_GPIO_PXMASKC(2)  = 0x0fffffff;	\
	REG_GPIO_PXPAT0C(2) = 0x0fffffff;	\
	REG_GPIO_PXPAT1C(2) = 0x0fffffff;		\
} while (0)

/*
 *  LCD_CLS, LCD_SPL, LCD_PS, LCD_REV
 */
#define __gpio_as_lcd_special()			\
do {						\
	REG_GPIO_PXINTC(2) = 0x0fffffff;	\
	REG_GPIO_PXMASKC(2) = 0x0fffffff;	\
	REG_GPIO_PXPAT0C(2) = 0x0feffbfc;	\
	REG_GPIO_PXPAT0S(2) = 0x00100403;	\
	REG_GPIO_PXPAT1C(2) = 0x0fffffff;		\
} while (0)

/*
 * CIM_D0~CIM_D7, CIM_MCLK, CIM_PCLK, CIM_VSYNC, CIM_HSYNC
 */
#define __gpio_as_cim()					\
	do {						\
		REG_GPIO_PXINTC(1) = 0x0003ffc0;	\
		REG_GPIO_PXMASKC(1) = 0x0003ffc0;		\
		REG_GPIO_PXPAT1C(1) = 0x0003ffc0;	\
		REG_GPIO_PXPAT0C(1) = 0x0003ffc0;	\
	} while (0)

#define __gpio_as_cim_10bit()					\
	do {						\
		REG_GPIO_PXINTC(1) = 0x000fffc0;	\
		REG_GPIO_PXMASKC(1) = 0x000fffc0;	\
		REG_GPIO_PXPAT1C(1) = 0x000fffc0;	\
		REG_GPIO_PXPAT0C(1) = 0x000fffc0;	\
	} while (0)

/*
 * SCLK_RSTN, BCLK_AD, SYNC_AD, SDATI, BCLK, SYNC,
 * SDATO, SDATO1, SDATO2, SDATO3
 */
#if 1
#define __gpio_as_aic()					\
	do {						\
		__gpio_as_func2(GPE(5));		\
		__gpio_as_func0(GPE(6));		\
		__gpio_as_func0(GPE(7));		\
		__gpio_as_func1(GPE(8));		\
		__gpio_as_func1(GPE(9));		\
		__gpio_as_func1(GPD(12));		\
		__gpio_as_func0(GPD(13));		\
		__gpio_as_func0(GPE(11));		\
		__gpio_as_func0(GPE(12));		\
		__gpio_as_func0(GPE(13));		\
	} while (0)
#else

#define __gpio_as_aic()				\
do {						\
	REG_GPIO_PXFUNS(4) = 0x16c00000;	\
	REG_GPIO_PXTRGC(4) = 0x02c00000;	\
	REG_GPIO_PXTRGS(4) = 0x14000000;	\
	REG_GPIO_PXSELC(4) = 0x14c00000;	\
	REG_GPIO_PXSELS(4) = 0x02000000;	\
	REG_GPIO_PXPENS(4)  = 0x16c00000;	\
} while (0)
#endif

#define __gpio_as_pcm0()			\
	do {					\
		__gpio_as_func0(GPD(0));	\
		__gpio_as_func0(GPD(1));	\
		__gpio_as_func0(GPD(2));	\
		__gpio_as_func0(GPD(3));	\
	} while (0)

#define __gpio_as_pcm1()			\
	do {					\
		__gpio_as_func1(GPF(12));	\
		__gpio_as_func1(GPF(13));	\
		__gpio_as_func1(GPF(14));	\
		__gpio_as_func1(GPF(15));	\
	} while (0)

/*
 * TSCLK, TSSTR, TSFRM, TSFAIL, TSDI0~7
 */
#define __gpio_as_tssi()                        \
do {                                            \
        REG_GPIO_PXINTC(1) = 0xf0300000;        \
        REG_GPIO_PXMASKC(1)  = 0xf0300000;                \
        REG_GPIO_PXPAT0S(1) = 0xf0300000;       \
        REG_GPIO_PXPAT1S(1) = 0xf0300000;               \
                                                        \
        REG_GPIO_PXINTC(1) = 0x0fc00000;        \
        REG_GPIO_PXMASKC(1)  = 0x0fc00000;                \
        REG_GPIO_PXPAT0C(1) = 0x0fc00000;       \
        REG_GPIO_PXPAT1C(1) = 0x0fc00000;               \
} while (0)


/*
 * SSI_CE0, SSI_CE1, SSI_GPC, SSI_CLK, SSI_DT, SSI_DR
 */
#define __gpio_as_ssi()				\
do {						\
	REG_GPIO_PXFUNS(0) = 0x002c0000; /* SSI0_CE0, SSI0_CLK, SSI0_DT	*/ \
	REG_GPIO_PXTRGS(0) = 0x002c0000;	\
	REG_GPIO_PXSELC(0) = 0x002c0000;	\
	REG_GPIO_PXPENS(0)  = 0x002c0000;	\
						\
	REG_GPIO_PXFUNS(0) = 0x00100000; /* SSI0_DR */	\
	REG_GPIO_PXTRGC(0) = 0x00100000;	\
	REG_GPIO_PXSELS(0) = 0x00100000;	\
	REG_GPIO_PXPENS(0)  = 0x00100000;	\
} while (0)

/*
 * SSI_CE0, SSI_CE2, SSI_GPC, SSI_CLK, SSI_DT, SSI1_DR
 */
#define __gpio_as_ssi_1()			\
do {						\
	REG_GPIO_PXFUNS(5) = 0x0000fc00;	\
	REG_GPIO_PXTRGC(5) = 0x0000fc00;	\
	REG_GPIO_PXSELC(5) = 0x0000fc00;	\
	REG_GPIO_PXPENS(5)  = 0x0000fc00;	\
} while (0)

/* Port B
 * SSI2_CE0, SSI2_CE2, SSI2_GPC, SSI2_CLK, SSI2_DT, SSI12_DR
 */
#define __gpio_as_ssi2_1()			\
do {						\
	REG_GPIO_PXFUNS(5) = 0xf0300000;	\
	REG_GPIO_PXTRGC(5) = 0xf0300000;	\
	REG_GPIO_PXSELS(5) = 0xf0300000;	\
	REG_GPIO_PXPENS(5)  = 0xf0300000;	\
} while (0)


/*
 * I2C_SCK, I2C_SDA
 */
#define __gpio_as_i2c(n)				\
	do {						\
		REG_GPIO_PXINTC(3 + (n)) = 0xC0000000;	\
		REG_GPIO_PXMASKC(3 + (n)) = 0xC0000000;	\
		REG_GPIO_PXPAT1C(3 + (n)) = 0xC0000000;	\
		REG_GPIO_PXPAT0C(3 + (n)) = 0xC0000000;	\
	} while (0)

#define __gpio_as_i2c2()				\
	do {						\
		REG_GPIO_PXINTC(5) = 0x00030000;	\
		REG_GPIO_PXMASKC(5) = 0x00030000;	\
		REG_GPIO_PXPAT1S(5) = 0x00030000;	\
		REG_GPIO_PXPAT0C(5) = 0x00030000;	\
	} while(0)
/*
 * PWM0
 */
#define __gpio_as_pwm0()			\
do {						\
	REG32(GPIO_PXDATC(4)) = 0x1;		\
	REG32(GPIO_PXIMC(4)) = 0x1;		\
	REG32(GPIO_PXPEC(4)) = 0x1;		\
	REG32(GPIO_PXFUNC(4)) = 0x1;		\
} while (0)

/*
 * PWM1
 */
#define __gpio_as_pwm1()			\
do {						\
	REG32(GPIO_PXDATC(4)) = 0x2;		\
	REG32(GPIO_PXIMC(4)) = 0x2;		\
	REG32(GPIO_PXPEC(4)) = 0x2;		\
	REG32(GPIO_PXFUNC(4)) = 0x2;		\
} while (0)

/*
 * PWM2
 */
#define __gpio_as_pwm2()			\
do {						\
	REG32(GPIO_PXDATC(4)) = 0x4;		\
	REG32(GPIO_PXIMC(4)) = 0x4;		\
	REG32(GPIO_PXPEC(4)) = 0x4;		\
	REG32(GPIO_PXFUNC(4)) = 0x4;		\
} while (0)

/*
 * PWM3
 */
#define __gpio_as_pwm3()			\
do {						\
	REG32(GPIO_PXDATC(4)) = 0x8;		\
	REG32(GPIO_PXIMC(4)) = 0x8;		\
	REG32(GPIO_PXPEC(4)) = 0x8;		\
	REG32(GPIO_PXFUNC(4)) = 0x8;		\
} while (0)

/*
 * PWM4
 */
#define __gpio_as_pwm4()			\
do {						\
	REG32(0xB0010418) = 0x10;		\
	REG32(0xB0010428) = 0x10;		\
	REG32(0xB0010438) = 0x10;		\
	REG32(0xB0010448) = 0x10;		\
} while (0)

/*
 * PWM5
 */
#define __gpio_as_pwm5()			\
do {						\
	REG32(GPIO_PXDATC(4)) = 0x20;		\
	REG32(GPIO_PXIMC(4)) = 0x20;		\
	REG32(GPIO_PXPEC(4)) = 0x20;		\
	REG32(GPIO_PXFUNC(4)) = 0x20;		\
} while (0)

#define __gpio_as_pwm6()			\
do {						\
	REG32(GPIO_PXDATC(3)) = 0x1 << 10;	\
	REG32(GPIO_PXIMC(3)) = 0x1 << 10;	\
	REG32(GPIO_PXPEC(3)) = 0x1 << 10;	\
	REG32(GPIO_PXFUNC(3)) = 0x1 << 10;	\
} while (0)

#define __gpio_as_pwm7()			\
do {						\
	REG32(GPIO_PXDATC(3)) = 0x1 << 11;	\
	REG32(GPIO_PXIMC(3)) = 0x1 << 11;	\
	REG32(GPIO_PXPEC(3)) = 0x1 << 11;	\
	REG32(GPIO_PXFUNC(3)) = 0x1 << 11;	\
} while (0)

/*
 * n = 0 ~ 7
 */
#define __gpio_as_pwm(n)	__gpio_as_pwm##n()

/*
 * OWI - PA29 function 1
 */
#define __gpio_as_owi()				\
do {						\
	REG_GPIO_PXFUNS(0) = 0x20000000;	\
	REG_GPIO_PXTRGC(0) = 0x20000000;	\
	REG_GPIO_PXSELS(0) = 0x20000000;	\
} while (0)

/*
 * SCC - PD08 function 0
 *       PD09 function 0
 */
#define __gpio_as_scc()				\
do {						\
	REG_GPIO_PXFUNS(3) = 0xc0000300;	\
	REG_GPIO_PXTRGC(3) = 0xc0000300;	\
	REG_GPIO_PXSELC(3) = 0xc0000300;	\
} while (0)

#define __gpio_as_otg_drvvbus()	\
do {	\
	__gpio_as_func0(4*32+10);		\
} while (0)

//-------------------------------------------
// GPIO or Interrupt Mode

#define __gpio_get_port(p)	(REG_GPIO_PXPIN(p))
#define __gpio_port_as_output0(p, o)		\
do {						\
    REG_GPIO_PXINTC(p) = (1 << (o));		\
    REG_GPIO_PXMASKS(p) = (1 << (o));		\
    REG_GPIO_PXPAT1C(p) = (1 << (o));		\
    REG_GPIO_PXPAT0C(p) = (1 << (o));		\
} while (0)

#define __gpio_port_as_output1(p, o)		\
do {						\
    REG_GPIO_PXINTC(p) = (1 << (o));		\
    REG_GPIO_PXMASKS(p) = (1 << (o));		\
    REG_GPIO_PXPAT1C(p) = (1 << (o));		\
    REG_GPIO_PXPAT0S(p) = (1 << (o));		\
} while (0)

#define __gpio_port_as_input(p, o)		\
do {						\
    REG_GPIO_PXINTC(p) = (1 << (o));		\
    REG_GPIO_PXMASKS(p) = (1 << (o));		\
    REG_GPIO_PXPAT1S(p) = (1 << (o));		\
    REG_GPIO_PXPAT0C(p) = (1 << (o));		\
} while (0)


#define __gpio_as_output0(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	__gpio_port_as_output0(p, o);		\
} while (0)

#define __gpio_as_output1(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	__gpio_port_as_output1(p, o);		\
} while (0)

#define __gpio_as_output(n)	__gpio_as_output0(n)

#define __gpio_as_input(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	__gpio_port_as_input(p, o);		\
} while (0)

#define __gpio_set_pin(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXPAT0S(p) = (1 << o);		\
} while (0)

#define __gpio_clear_pin(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXPAT0C(p) = (1 << o);		\
} while (0)

#define __gpio_get_pin(n)			\
({						\
	unsigned int p, o, v;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	if (__gpio_get_port(p) & (1 << o))	\
		v = 1;				\
	else					\
		v = 0;				\
	v;					\
})

#define __gpio_as_irq_high_level(n)		\
	do {					\
		unsigned int p, o;		\
		p = (n) / 32;			\
		o = (n) % 32;			\
		REG_GPIO_PXINTS(p) = (1 << o);	\
		REG_GPIO_PXMASKC(p) = (1 << o);	\
		REG_GPIO_PXPAT1C(p) = (1 << o);	\
		REG_GPIO_PXPAT0S(p) = (1 << o);	\
	} while (0)

#define __gpio_as_irq_low_level(n)		\
	do {					\
		unsigned int p, o;		\
		p = (n) / 32;			\
		o = (n) % 32;			\
		REG_GPIO_PXINTS(p) = (1 << o);	\
		REG_GPIO_PXMASKC(p) = (1 << o);	\
		REG_GPIO_PXPAT1C(p) = (1 << o);	\
		REG_GPIO_PXPAT0C(p) = (1 << o);	\
	} while (0)

#define __gpio_as_irq_rise_edge(n)		\
	do {					\
		unsigned int p, o;		\
		p = (n) / 32;			\
		o = (n) % 32;			\
		REG_GPIO_PXINTS(p) = (1 << o);	\
		REG_GPIO_PXMASKC(p) = (1 << o);	\
		REG_GPIO_PXPAT1S(p) = (1 << o);	\
		REG_GPIO_PXPAT0S(p) = (1 << o);	\
	} while (0)

#define __gpio_as_irq_fall_edge(n)		\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXINTS(p) = (1 << o);  \
        REG_GPIO_PXMASKC(p) = (1 << o);   \
        REG_GPIO_PXPAT1S(p) = (1 << o); \
        REG_GPIO_PXPAT0C(p) = (1 << o); \
} while (0)

#define __gpio_mask_irq(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXMASKS(p) = (1 << o);		\
} while (0)

#define __gpio_unmask_irq(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXMASKC(p) = (1 << o);		\
} while (0)

#define __gpio_ack_irq(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXFLGC(p) = (1 << o);		\
} while (0)

#define __gpio_group_irq(n)			\
({						\
	register int tmp, i;			\
	tmp = REG_GPIO_PXFLG((n));		\
	for (i=31;i>=0;i--)			\
		if (tmp & (1 << i))		\
			break;			\
	i;					\
})

#define __gpio_enable_pull(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXPEC(p) = (1 << o);		\
} while (0)

#define __gpio_disable_pull(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXPENS(p) = (1 << o);		\
} while (0)


#endif /* __MIPS_ASSEMBLER */

#endif /* __JZ4770GPIO_H__ */

