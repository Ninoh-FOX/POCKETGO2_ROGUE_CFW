/*
 * linux/arch/mips/jz4770/cpm.c
 *
 * jz4770 on-chip modules.
 *
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 *
 * Author: <whxu@ingenic.cn>
 */

#include <linux/module.h>

#include <asm/mach-jz4770/jz4770cpm.h>


#ifndef JZ_EXTAL
#define JZ_EXTAL        (12 * 1000000)  /* 12MHz */
#endif


/*
 * Get clock div, such as CDIV, HDIV, PDIV, MDIV, H2DIV, SDIV.
 * Convert the value read from register to the division ratio
 */
static unsigned int __get_clock_div(unsigned int val)
{
        unsigned int index;
        unsigned int div[] = {1, 2, 3, 4, 6, 8};

        if (val < (sizeof(div)/div[0]))
                index = val;
        else {
                printk("WARNING: Invalid clock value %d!\n", val);
                index = 0;
        }

        return div[index];
}

/*
 * Get the external clock
 */
static unsigned int get_external_clock(void)
{
        return JZ_EXTAL;
}

/*
 * Get the PLL clock
 */
unsigned int cpm_get_pllout(void)
{
        unsigned int exclk = get_external_clock();
        unsigned int pll_stat, pll_ctrl, pllout;
        unsigned int m, n, od, no;


        pll_stat = INREG32(CPM_CPPSR);
        if ((pll_stat & CPPSR_PLLBP) || (pll_stat & CPPSR_PLLOFF)) {
                pllout = exclk;
                return pllout;
        }

        pll_ctrl = INREG32(CPM_CPPCR0);

        m = ((pll_ctrl & CPPCR0_PLLM_MASK) >> CPPCR0_PLLM_LSB) + 1;
        n = ((pll_ctrl & CPPCR0_PLLN_MASK) >> CPPCR0_PLLN_LSB) + 1;

        od = (pll_ctrl & CPPCR0_PLLOD_MASK) >> CPPCR0_PLLOD_LSB;
        no = 1 << od;

	pllout = exclk * m / (n * no);

        return pllout;
}
EXPORT_SYMBOL(cpm_get_pllout);

/*
 * Get the PLL2 clock
 */
unsigned int cpm_get_pllout1(void)
{
        unsigned int clock_in, pll_ctrl, pll_out;
        unsigned int m, n, od, no, val, div;

        pll_ctrl = INREG32(CPM_CPPCR1);

        if ( !(pll_ctrl & CPPCR1_PLL1ON)) {					/* pll1 off */
            return 0;
        }

        if (pll_ctrl & CPPCR1_P1SCS) {
                val = get_bf_value(pll_ctrl, CPPCR1_P1SDIV_LSB, CPPCR1_P1SDIV_MASK);
                div = val + 1;
                clock_in = cpm_get_pllout() / div;
        } else
                clock_in = get_external_clock();

        if (pll_ctrl & CPPCR1_PLL1S) {
                m = ((pll_ctrl & CPPCR1_PLL1M_MASK) >> CPPCR1_PLL1M_LSB) + 1;
                n = ((pll_ctrl & CPPCR1_PLL1N_MASK) >> CPPCR1_PLL1N_LSB) + 1;

                od = (pll_ctrl & CPPCR1_PLL1OD_MASK) >> CPPCR1_PLL1OD_LSB;
                no = 1 << od;
		pll_out = clock_in * m / (n * no);
        } else {
                pll_out = clock_in;
        }

        return pll_out;
}
EXPORT_SYMBOL(cpm_get_pllout1);

/*
 * Start the module clock
 */
void cpm_start_clock(clock_gate_module module_name)
{
        unsigned int cgr_index, module_index;

	if (module_name == CGM_ALL_MODULE) {
	        OUTREG32(CPM_CLKGR0, 0x0);
	        OUTREG32(CPM_CLKGR1, 0x0);
		return;
	}

        cgr_index = module_name / 32;
        module_index = module_name % 32;
        switch (cgr_index) {
                case 0:
                        CLRREG32(CPM_CLKGR0, 1 << module_index);
			switch(module_index) {
			case 3:
				CLRREG32(CPM_MSC0CDR, MSCCDR_MCSG);
				CLRREG32(CPM_MSC0CDR, MSCCDR_MPCS);
				break;
			case 11:
				CLRREG32(CPM_MSC1CDR, MSCCDR_MCSG);
				CLRREG32(CPM_MSC1CDR, MSCCDR_MPCS);
				break;
			case 12:
				CLRREG32(CPM_MSC2CDR, MSCCDR_MCSG);
				CLRREG32(CPM_MSC2CDR, MSCCDR_MPCS);
				break;
			default:
				;
			}
                        break;
                case 1:
                        CLRREG32(CPM_CLKGR1, 1 << module_index);
                        break;
                default:
                        printk("WARNING: can NOT start the %d's clock\n",
                                        module_name);
                        break;
       }
}
EXPORT_SYMBOL(cpm_start_clock);

/*
 * Stop the module clock
 */
void cpm_stop_clock(clock_gate_module module_name)
{
        unsigned int cgr_index, module_index;

        if (module_name == CGM_ALL_MODULE) {
	        OUTREG32(CPM_CLKGR0, 0xffffffff);
	        OUTREG32(CPM_CLKGR1, 0x3ff);
		return;
        }

        cgr_index = module_name / 32;
        module_index = module_name % 32;
        switch (cgr_index) {
                case 0:
                        SETREG32(CPM_CLKGR0, 1 << module_index);
                        break;

                case 1:
                        SETREG32(CPM_CLKGR1, 1 << module_index);
                        break;

                default:
                        printk("WARNING: can NOT stop the %d's clock\n",
                                        module_name);
                        break;
        }
}
EXPORT_SYMBOL(cpm_stop_clock);

/*
 * Get the clock, assigned by the clock_name, and the return value unit is Hz
 */
unsigned int cpm_get_clock(cgu_clock clock_name)
{
        unsigned int exclk = get_external_clock();
        unsigned int pllclk1 = cpm_get_pllout1();
        unsigned int pllclk = cpm_get_pllout();
        unsigned int clock_ctrl = INREG32(CPM_CPCCR);
        unsigned int clock_hz = exclk;
        unsigned int val, tmp, div;

        switch (clock_name) {
        case CGU_CCLK:
                val = get_bf_value(clock_ctrl, CPCCR_CDIV_LSB, CPCCR_CDIV_MASK);
                div = __get_clock_div(val);
                clock_hz = pllclk / div;

                break;

        case CGU_HCLK:
                val = get_bf_value(clock_ctrl, CPCCR_H0DIV_LSB, CPCCR_H0DIV_MASK);
                div = __get_clock_div(val);
                clock_hz = pllclk / div;

                break;

        case CGU_PCLK:
                val = get_bf_value(clock_ctrl, CPCCR_PDIV_LSB, CPCCR_PDIV_MASK);
                div = __get_clock_div(val);
                clock_hz = pllclk / div;

                break;

	case CGU_C1CLK:
                val = get_bf_value(clock_ctrl, CPCCR_C1DIV_LSB, CPCCR_C1DIV_MASK);
                div = __get_clock_div(val);
                clock_hz = pllclk / div;

                break;

        case CGU_MCLK:
                val = get_bf_value(clock_ctrl, CPCCR_H0DIV_LSB, CPCCR_H0DIV_MASK);
                div = __get_clock_div(val);
                clock_hz = pllclk / div;

                break;

        case CGU_H2CLK:
                val = get_bf_value(clock_ctrl, CPCCR_H2DIV_LSB, CPCCR_H2DIV_MASK);
                div = __get_clock_div(val);
                clock_hz = pllclk / div;

                break;

	case CGU_H1CLK:
                val = get_bf_value(clock_ctrl, CPCCR_H1DIV_LSB, CPCCR_H1DIV_MASK);
                div = __get_clock_div(val);
                clock_hz = pllclk / div;

                break;

#if 0
        case CGU_SCLK:
                val = get_bf_value(clock_ctrl, CPCCR_SDIV_LSB, CPCCR_SDIV_MASK);
                div = __get_clock_div(val);
                clock_hz = pllclk / div;

                break;
#endif

        case CGU_MSC0CLK:
                tmp = INREG32(CPM_MSC0CDR);
                val = get_bf_value(tmp, MSCCDR_MSCDIV_LSB, MSCCDR_MSCDIV_MASK);
                div = val + 1;
                if (!(tmp & MSCCDR_MPCS)) {
                        if (clock_ctrl & CPCCR_PCS)
                                clock_hz = (pllclk / 2) / div;
                        else
                                clock_hz = pllclk / div;

                } else {
			clock_hz = pllclk1 / div;
                }

                break;

	case CGU_MSC1CLK:
                tmp = INREG32(CPM_MSC1CDR);
                val = get_bf_value(tmp, MSCCDR_MSCDIV_LSB, MSCCDR_MSCDIV_MASK);
                div = val + 1;
                if (!(tmp & MSCCDR_MPCS)) {
                        if (clock_ctrl & CPCCR_PCS)
                                clock_hz = (pllclk / 2) / div;
                        else
                                clock_hz = pllclk / div;

                } else {
			clock_hz = pllclk1 / div;
                }

                break;

	case CGU_MSC2CLK:
                tmp = INREG32(CPM_MSC2CDR);
                val = get_bf_value(tmp, MSCCDR_MSCDIV_LSB, MSCCDR_MSCDIV_MASK);
                div = val + 1;
                if (!(tmp & MSCCDR_MPCS)) {
                        if (clock_ctrl & CPCCR_PCS)
                                clock_hz = (pllclk / 2) / div;
                        else
				clock_hz = pllclk / div;

                } else {
			clock_hz = pllclk1 / div;
                }

                break;

        case CGU_SSICLK:
                tmp = INREG32(CPM_SSICDR);
                val = get_bf_value(tmp, SSICDR_SSIDIV_LSB, SSICDR_SSIDIV_MASK);
                div = val + 1;
                if (tmp & SSICDR_SCS) {
                        if (clock_ctrl & CPCCR_PCS)
                                clock_hz = pllclk / div;
                        else
                                clock_hz = (pllclk / 2) / div;
                } else {
                        clock_hz = exclk;
                }

                break;

        case CGU_CIMCLK:
                tmp = INREG32(CPM_CIMCDR);
                val = get_bf_value(tmp, CIMCDR_CIMDIV_LSB, CIMCDR_CIMDIV_MASK);
                div = val + 1;
                if (clock_ctrl & CPCCR_PCS)
                        clock_hz = pllclk / div;
                else
                        clock_hz = (pllclk / 2) / div;

                break;

        case CGU_LPCLK:
	case CGU_TVECLK:
                tmp = INREG32(CPM_LPCDR);
                val = get_bf_value(tmp, LPCDR_PIXDIV_LSB, LPCDR_PIXDIV_MASK);
                div = val + 1;
                if (tmp & LPCDR_LTCS) {
			if (tmp & LPCDR_LPCS)
				clock_hz = cpm_get_pllout1() / div;
			else
				clock_hz = pllclk / div;
		}
                 else {
                        if (tmp & LPCDR_LPCS)
                                clock_hz = cpm_get_pllout1() / div;
                        else
                                clock_hz = pllclk / div;
                }

                break;

        case CGU_I2SCLK:
                tmp = INREG32(CPM_I2SCDR);
                val = get_bf_value(tmp, CIMCDR_CIMDIV_LSB, CIMCDR_CIMDIV_MASK);
                div = val + 1;
                if (tmp & I2SCDR_I2CS) {
                        if (tmp & I2SCDR_I2PCS)
                                clock_hz = cpm_get_pllout1() / div;
                        else
                                clock_hz = pllclk / div;
                } else {
                        if (clock_ctrl & CPCCR_ECS)
                                clock_hz = exclk / 2;
                        else
                                clock_hz = exclk;
                }

                break;

        case CGU_PCMCLK:
                tmp = INREG32(CPM_PCMCDR);
                val = get_bf_value(tmp, PCMCDR_PCMDIV_LSB, PCMCDR_PCMDIV_MASK);
                div = val + 1;
                if (tmp & PCMCDR_PCMS) {
                        if (tmp & PCMCDR_PCMPCS)
                                clock_hz = cpm_get_pllout1() / div;
                        else
                                clock_hz = pllclk / div;
                } else {
                        if (clock_ctrl & CPCCR_ECS)
                                clock_hz = exclk / 2;
                        else
                                clock_hz = exclk;
                }

                break;

        case CGU_OTGCLK:
                tmp = INREG32(CPM_USBCDR);
                val = get_bf_value(tmp, USBCDR_OTGDIV_LSB, USBCDR_OTGDIV_MASK);
                div = val + 1;
                if (tmp & USBCDR_UCS) {
                        if (tmp & USBCDR_UPCS)
                                clock_hz = cpm_get_pllout1() / div;
                        else
                                clock_hz = pllclk / div;
                } else {
                        if (clock_ctrl & CPCCR_ECS)
                                clock_hz = exclk / 2;
                        else
                                clock_hz = exclk;
                }

                break;

        case CGU_UHCCLK:
                tmp = INREG32(CPM_UHCCDR);
                val = get_bf_value(tmp, UHCCDR_UHCDIV_LSB, UHCCDR_UHCDIV_MASK);
                div = val + 1;
                if (tmp & UHCCDR_UHPCS)
                        clock_hz = cpm_get_pllout1() / div;
                else
                        clock_hz = pllclk / div;

                break;

        case CGU_GPSCLK:
                tmp = INREG32(CPM_GPSCDR);
                val = get_bf_value(tmp, GPSCDR_GPSDIV_LSB, GSPCDR_GPSDIV_MASK);
                div = val + 1;
                if (tmp & GPSCDR_GPCS)
                        clock_hz = cpm_get_pllout1() / div;
                else
                        clock_hz = pllclk / div;

                break;

        case CGU_GPUCLK:
                tmp = INREG32(CPM_GPUCDR);
                val = get_bf_value(tmp, GPUCDR_GPUDIV_LSB, GPUCDR_GPUDIV_MASK);
                div = val + 1;
                if (tmp & GPUCDR_GPCS)
                        clock_hz = cpm_get_pllout1() / div;
                else
                        clock_hz = pllclk / div;

                break;

        case CGU_UARTCLK:
        case CGU_SADCCLK:
                if (clock_ctrl & CPCCR_ECS)
                        clock_hz = exclk / 2;
                else
                        clock_hz = exclk;

                break;

        case CGU_TCUCLK:
                clock_hz = exclk;

                break;

        default:
                printk("WARNING: can NOT get clock %d!\n", clock_name);
                clock_hz = exclk;
                break;
        }

        return clock_hz;
}
EXPORT_SYMBOL(cpm_get_clock);

/*
 * Check div value whether valid, if invalid, return the max valid value
 */
static unsigned int __check_div(unsigned int div, unsigned int lsb, unsigned int mask)
{
	if ((div << lsb) > mask) {
		printk("WARNING: Invalid div %d larger than %d\n", div, mask >> lsb);
		return mask >> lsb;
	} else
		return div;
}

/*
 * Set the clock, assigned by the clock_name, and the return value unit is Hz,
 * which means the actual clock
 */
#define ceil(v,div) ({ unsigned int val = 0; if(v % div ) val = v /div + 1; else val = v /div; val;})
#define nearbyint(v,div) ({unsigned int val = 0; if((v % div) * 2 >= div)  val = v / div + 1;else val = v / div; val;})
unsigned int cpm_set_clock(cgu_clock clock_name, unsigned int clock_hz)
{
        unsigned int actual_clock = 0;
        unsigned int exclk = get_external_clock();
        unsigned int pllclk = cpm_get_pllout();
	unsigned int pllclk1 = cpm_get_pllout1();
        unsigned int div;

        if (!clock_hz)
                return actual_clock;

        switch (clock_name) {
        case CGU_MSC0CLK:
		div = ceil(pllclk , clock_hz) - 1;
		div = __check_div(div, MSCCDR_MSCDIV_LSB, MSCCDR_MSCDIV_MASK);
		OUTREG32(CPM_MSC0CDR, div);

		break;

	case CGU_MSC1CLK:
		div = ceil(pllclk , clock_hz) - 1;
		div = __check_div(div, MSCCDR_MSCDIV_LSB, MSCCDR_MSCDIV_MASK);
		OUTREG32(CPM_MSC1CDR, div);

		break;

	case CGU_MSC2CLK:
		div = ceil(pllclk , clock_hz) - 1;
		div = __check_div(div, MSCCDR_MSCDIV_LSB, MSCCDR_MSCDIV_MASK);
		OUTREG32(CPM_MSC2CDR, div);

		break;

	case CGU_TVECLK:
		div = nearbyint(pllclk1 , clock_hz) - 1;
		div = __check_div(div, LPCDR_PIXDIV_LSB, LPCDR_PIXDIV_MASK);
		/* Select pll1 clock as input */
		OUTREG32(CPM_LPCDR, LPCDR_LTCS | LPCDR_LPCS | div);
		break;

	case CGU_LPCLK:
		div = nearbyint(pllclk , clock_hz) - 1;
		div = __check_div(div, LPCDR_PIXDIV_LSB, LPCDR_PIXDIV_MASK);
		/* Select pll0 clock as input */
		OUTREG32(CPM_LPCDR, div);
		break;

	case CGU_I2SCLK:
		if (clock_hz == exclk) {
			CLRREG32(CPM_CPCCR, CPCCR_ECS);
			OUTREG32(CPM_I2SCDR, 0);
		} else if (clock_hz == exclk/2) {
			SETREG32(CPM_CPCCR, CPCCR_ECS);
			OUTREG32(CPM_I2SCDR, 0);
		} else {
			div = nearbyint(pllclk , clock_hz) - 1;
			div = __check_div(div, I2SCDR_I2SDIV_LSB, I2SCDR_I2SDIV_MASK);
			OUTREG32(CPM_I2SCDR, I2SCDR_I2CS | div);
		}
		break;

	case CGU_PCMCLK:
                if (clock_hz == exclk) {
			/* Select external clock as input*/
                        CLRREG32(CPM_PCMCDR, PCMCDR_PCMS);
                } else {
			div = ceil(pllclk , clock_hz) - 1;
			div = __check_div(div, PCMCDR_PCMDIV_LSB, PCMCDR_PCMDIV_MASK);
                        OUTREG32(CPM_PCMCDR, PCMCDR_PCMS | div);
                }

		break;

	case CGU_OTGCLK:
		if (clock_hz == exclk) {
			CLRREG32(CPM_CPCCR, CPCCR_ECS);
			OUTREG32(CPM_USBCDR, 0);
		} else if (clock_hz == exclk/2) {
			SETREG32(CPM_CPCCR, CPCCR_ECS);
			OUTREG32(CPM_USBCDR, 0);
		} else {
			div = nearbyint(pllclk , clock_hz) - 1;
			div = __check_div(div, USBCDR_OTGDIV_LSB, USBCDR_OTGDIV_MASK);
			OUTREG32(CPM_USBCDR, USBCDR_UCS | div);
		}
		break;

	case CGU_CIMCLK:
		div = ceil(pllclk , clock_hz) - 1;
		div = __check_div(div, CIMCDR_CIMDIV_LSB, CIMCDR_CIMDIV_MASK);
		OUTREG32(CPM_CIMCDR, div);
		break;

	case CGU_UHCCLK:
		div = nearbyint(pllclk1 , clock_hz) - 1;
		div = __check_div(div, UHCCDR_UHCDIV_LSB, UHCCDR_UHCDIV_MASK);
		OUTREG32(CPM_UHCCDR, div | UHCCDR_UHPCS | UHCCDR_UHCS);

		break;

        default:
                printk("WARNING: can NOT set clock %d!\n", clock_name);
                break;
        }

        SETREG32(CPM_CPCCR, CPCCR_CE);
        /* Get the actual clock */
        actual_clock = cpm_get_clock(clock_name);

        return actual_clock;
}
EXPORT_SYMBOL(cpm_set_clock);

 /*
  * Control UHC phy, if en is NON-ZERO, enable the UHC phy, otherwise disable
  */
void cpm_uhc_phy(unsigned int en)
{
	if (en) {
		CLRREG32(CPM_OPCR, OPCR_UHCPHY_DISABLE);
		SETREG32(CPM_USBPCR1, USBPCR1_UHC_POWON);
	} else {
		CLRREG32(CPM_USBPCR1, USBPCR1_UHC_POWON);
		SETREG32(CPM_OPCR, OPCR_UHCPHY_DISABLE);
	}
}
