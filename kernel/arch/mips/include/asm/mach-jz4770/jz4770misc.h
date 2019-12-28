/*
 * linux/include/asm-mips/mach-jz4770/jz4770misc.h
 *
 * JZ4770 misc definition.
 *
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ4770MISC_H__
#define __JZ4770MISC_H__


#if defined(__ASSEMBLY__) || defined(__LANGUAGE_ASSEMBLY)
        #ifndef __MIPS_ASSEMBLER
                #define __MIPS_ASSEMBLER
        #endif
        #define REG8(addr)	(addr)
        #define REG16(addr)	(addr)
        #define REG32(addr)	(addr)
#else
        #define REG8(addr)	*((volatile unsigned char *)(addr))
        #define REG16(addr)	*((volatile unsigned short *)(addr))
        #define REG32(addr)	*((volatile unsigned int *)(addr))

        #define INREG8(x)               ((unsigned char)(*(volatile unsigned char *)(x)))
        #define OUTREG8(x, y)           *(volatile unsigned char *)(x) = (y)
        #define SETREG8(x, y)           OUTREG8(x, INREG8(x)|(y))
        #define CLRREG8(x, y)           OUTREG8(x, INREG8(x)&~(y))
        #define CMSREG8(x, y, m)        OUTREG8(x, (INREG8(x)&~(m))|(y))

        #define INREG16(x)              ((unsigned short)(*(volatile unsigned short *)(x)))
        #define OUTREG16(x, y)          *(volatile unsigned short *)(x) = (y)
        #define SETREG16(x, y)          OUTREG16(x, INREG16(x)|(y))
        #define CLRREG16(x, y)          OUTREG16(x, INREG16(x)&~(y))
        #define CMSREG16(x, y, m)       OUTREG16(x, (INREG16(x)&~(m))|(y))

        #define INREG32(x)              ((unsigned int)(*(volatile unsigned int *)(x)))
        #define OUTREG32(x, y)          *(volatile unsigned int *)(x) = (y)
        #define SETREG32(x, y)          OUTREG32(x, INREG32(x)|(y))
        #define CLRREG32(x, y)          OUTREG32(x, INREG32(x)&~(y))
        #define CMSREG32(x, y, m)       OUTREG32(x, (INREG32(x)&~(m))|(y))

#endif


/*
 * Define the bit field macro to avoid the bit mistake
 */
#define BIT0            (1 << 0)
#define BIT1            (1 << 1)
#define BIT2            (1 << 2)
#define BIT3            (1 << 3)
#define BIT4            (1 << 4)
#define BIT5            (1 << 5)
#define BIT6            (1 << 6)
#define BIT7            (1 << 7)
#define BIT8            (1 << 8)
#define BIT9            (1 << 9)
#define BIT10           (1 << 10)
#define BIT11           (1 << 11)
#define BIT12 	        (1 << 12)
#define BIT13 	        (1 << 13)
#define BIT14 	        (1 << 14)
#define BIT15 	        (1 << 15)
#define BIT16 	        (1 << 16)
#define BIT17 	        (1 << 17)
#define BIT18 	        (1 << 18)
#define BIT19 	        (1 << 19)
#define BIT20 	        (1 << 20)
#define BIT21 	        (1 << 21)
#define BIT22 	        (1 << 22)
#define BIT23 	        (1 << 23)
#define BIT24 	        (1 << 24)
#define BIT25 	        (1 << 25)
#define BIT26 	        (1 << 26)
#define BIT27 	        (1 << 27)
#define BIT28 	        (1 << 28)
#define BIT29 	        (1 << 29)
#define BIT30 	        (1 << 30)
#define BIT31 	        (1 << 31)


/* Generate the bit field mask from msb to lsb */
#define BITS_H2L(msb, lsb)  ((0xFFFFFFFF >> (32-((msb)-(lsb)+1))) << (lsb))


/* Get the bit field value from the data which is read from the register */
#define get_bf_value(data, lsb, mask)  (((data) & (mask)) >> (lsb))


#endif /* __JZ4770MISC_H__ */
