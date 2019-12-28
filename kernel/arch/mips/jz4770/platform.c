/*
 * Platform device support for Jz4770 SoC.
 *
 * Copyright 2010, Software Department III
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/resource.h>

#include <linux/usb/musb.h>

#include <asm/mach-jz4770/base.h>
#include <asm/mach-jz4770/dma.h>
#include <asm/mach-jz4770/irq.h>
#include <asm/mach-jz4770/platform.h>

/* Pin controller */
static struct resource jz_pinctrl_resources[] = {
	{
		.start		= JZ4770_GPIO_BASE_ADDR,
		.end		= JZ4770_GPIO_BASE_ADDR + 0x5AB,
		.flags		= IORESOURCE_MEM,
	},
};

static struct platform_device jz4770_pinctrl_device = {
	.name	= "jz4770-pinctrl",
	.id	= -1,
	.resource	= jz_pinctrl_resources,
	.num_resources	= ARRAY_SIZE(jz_pinctrl_resources),
};

/* OHCI (USB full speed host controller) */
static struct resource jz_usb_ohci_resources[] = {
	{
		.start		= JZ4770_UHC_BASE_ADDR,
		.end		= JZ4770_UHC_BASE_ADDR + 0x1000 - 1,
		.flags		= IORESOURCE_MEM,
	},
	{
		.start		= IRQ_UHC,
		.end		= IRQ_UHC,
		.flags		= IORESOURCE_IRQ,
	},
};

struct platform_device jz4770_usb_ohci_device = {
	.name		= "jz4770-ohci",
	.id		= -1,
	.dev = {
		.dma_mask = &jz4770_usb_ohci_device.dev.coherent_dma_mask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
	},
	.num_resources	= ARRAY_SIZE(jz_usb_ohci_resources),
	.resource	= jz_usb_ohci_resources,
};

/*** LCD controller ***/
static struct resource jz_lcd_resources[] = {
	[0] = {
		.start          = JZ4770_LCD_BASE_ADDR,
		.end            = JZ4770_LCD_BASE_ADDR + 0x13F,
		.flags          = IORESOURCE_MEM,
	},
	[1] = {
		.start          = JZ4770_IPU_BASE_ADDR,
		.end            = JZ4770_IPU_BASE_ADDR + 0x9B,
		.flags          = IORESOURCE_MEM,
	},
	{
		.name           = "tve",
		.start          = JZ4770_LCD_BASE_ADDR + 0x140,
		.end            = JZ4770_LCD_BASE_ADDR + 0x1BF,
		.flags          = IORESOURCE_MEM,
	},
	{
		.name           = "part2",
		.start          = JZ4770_LCD_BASE_ADDR + 0x1C0,
		.end            = JZ4770_LCD_BASE_ADDR + 0x2FF,
		.flags          = IORESOURCE_MEM,
	},
	{
		.start          = IRQ_LCD,
		.end            = IRQ_LCD,
		.flags          = IORESOURCE_IRQ,
	}
};

static u64 jz_lcd_dmamask = ~(u32)0;

struct platform_device jz4770_lcd_device = {
	.name           = "jz-lcd",
	.id             = 0,
	.dev = {
		.dma_mask               = &jz_lcd_dmamask,
		.coherent_dma_mask      = 0xffffffff,
	},
	.num_resources  = ARRAY_SIZE(jz_lcd_resources),
	.resource       = jz_lcd_resources,
};

/* USB OTG Controller */
struct platform_device jz4770_usb_otg_xceiv_device = {
	.name	= "usb_phy_gen_xceiv",
	.id	= 0,
};

static struct musb_hdrc_config jz_usb_otg_config = {
	.multipoint	= 1,
/* Max EPs scanned. Driver will decide which EP can be used automatically. */
	.num_eps	= 6,
};

static struct musb_hdrc_platform_data jz_usb_otg_platform_data = {
	.mode           = MUSB_OTG,
	.config		= &jz_usb_otg_config,
};

static struct resource jz_usb_otg_resources[] = {
	[0] = {
		.start		= JZ4770_UDC_BASE_ADDR,
		.end		= JZ4770_UDC_BASE_ADDR + 0x10000 - 1,
		.flags		= IORESOURCE_MEM,
	},
	[1] = {
		.start		= IRQ_OTG,
		.end		= IRQ_OTG,
		.flags		= IORESOURCE_IRQ,
		.name		= "mc",
	},
};

static u64  usb_otg_dmamask = ~(u32)0;

struct platform_device jz4770_usb_otg_device = {
	.name	= "musb-jz",
	.id	= 0,
	.dev = {
		.dma_mask		= &usb_otg_dmamask,
		.coherent_dma_mask	= 0xffffffff,
		.platform_data		= &jz_usb_otg_platform_data,
	},
	.num_resources	= ARRAY_SIZE(jz_usb_otg_resources),
	.resource	= jz_usb_otg_resources,
};

/** MMC/SD/SDIO controllers**/

#define JZ_MSC_PLATFORM_DEV(msc_id)					\
	static struct resource jz4770_msc##msc_id##_resources[] = {		\
		{							\
			.start	= JZ4770_MSC##msc_id##_BASE_ADDR,	\
			.end	= JZ4770_MSC##msc_id##_BASE_ADDR + 0x1000 - 1, \
			.flags	= IORESOURCE_MEM,			\
		},							\
		{							\
			.start	= IRQ_MSC##msc_id,			\
			.end	= IRQ_MSC##msc_id,			\
			.flags	= IORESOURCE_IRQ,			\
		},							\
		{							\
			.start	= DMA_ID_MSC##msc_id,			\
			.end	= DMA_ID_MSC##msc_id,			\
			.flags	= IORESOURCE_DMA,			\
		},							\
	};								\
									\
	static u64 jz4770_msc##msc_id##_dmamask =  ~(u32)0;			\
									\
	struct platform_device jz4770_msc##msc_id##_device = {		\
		.name = "jz-msc",					\
		.id = msc_id,						\
		.dev = {						\
			.dma_mask               = &jz4770_msc##msc_id##_dmamask, \
			.coherent_dma_mask      = 0xffffffff,		\
		},							\
		.num_resources  = ARRAY_SIZE(jz4770_msc##msc_id##_resources), \
		.resource       = jz4770_msc##msc_id##_resources,		\
	};

JZ_MSC_PLATFORM_DEV(0)
JZ_MSC_PLATFORM_DEV(1)
JZ_MSC_PLATFORM_DEV(2)

/* Sound devices */

/* I2S */
static struct resource jz_i2s_resources[] = {
	{
		.start	= JZ4770_AIC_BASE_ADDR,
		.end	= JZ4770_AIC_BASE_ADDR + 0x38 - 1,
		.flags	= IORESOURCE_MEM,
	},
};

struct platform_device jz4770_i2s_device = {
	.name		= "jz4770-i2s",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(jz_i2s_resources),
	.resource	= jz_i2s_resources,
};

/* PCM */
struct platform_device jz4770_pcm_device = {
	.name		= "jz4770-pcm-audio",
	.id		= -1,
};

/* Codec */
static struct resource jz_icdc_resources[] = {
	{
		.start	= JZ4770_AIC_BASE_ADDR + 0xA0,
		.end	= JZ4770_AIC_BASE_ADDR + 0xB0 - 1,
		.flags	= IORESOURCE_MEM,
	},
};

struct platform_device jz4770_icdc_device = {
	.name		= "jz4770-icdc",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(jz_icdc_resources),
	.resource	= jz_icdc_resources,
};

/* I2C devices */

static struct resource jz_i2c0_resources[] = {
	[0] = {
		.start          = JZ4770_I2C0_BASE_ADDR,
		.end            = JZ4770_I2C0_BASE_ADDR + 0x1000 - 1,
		.flags          = IORESOURCE_MEM,
	},
	[1] = {
		.start          = IRQ_I2C0,
		.end            = IRQ_I2C0,
		.flags          = IORESOURCE_IRQ,
	},
};

static struct resource jz_i2c1_resources[] = {
	[0] = {
		.start          = JZ4770_I2C1_BASE_ADDR,
		.end            = JZ4770_I2C1_BASE_ADDR + 0x1000 - 1,
		.flags          = IORESOURCE_MEM,
	},
	[1] = {
		.start          = IRQ_I2C1,
		.end            = IRQ_I2C1,
		.flags          = IORESOURCE_IRQ,
	},
};

static struct resource jz_i2c2_resources[] = {
	[0] = {
		.start          = JZ4770_I2C2_BASE_ADDR,
		.end            = JZ4770_I2C2_BASE_ADDR + 0x1000 - 1,
		.flags          = IORESOURCE_MEM,
	},
	[1] = {
		.start          = IRQ_I2C2,
		.end            = IRQ_I2C2,
		.flags          = IORESOURCE_IRQ,
	},
};

static u64 jz_i2c_dmamask =  ~(u32)0;

struct platform_device jz4770_i2c0_device = {
	.name = "i2c-jz4770",
	.id = 0,
	.dev = {
		.dma_mask               = &jz_i2c_dmamask,
		.coherent_dma_mask      = 0xffffffff,
	},
	.num_resources  = ARRAY_SIZE(jz_i2c0_resources),
	.resource       = jz_i2c0_resources,
};

struct platform_device jz4770_i2c1_device = {
	.name = "i2c-jz4770",
	.id = 1,
	.dev = {
		.dma_mask               = &jz_i2c_dmamask,
		.coherent_dma_mask      = 0xffffffff,
	},
	.num_resources  = ARRAY_SIZE(jz_i2c1_resources),
	.resource       = jz_i2c1_resources,
};

struct platform_device jz4770_i2c2_device = {
	.name = "i2c-jz4770",
	.id = 2,
	.dev = {
		.dma_mask               = &jz_i2c_dmamask,
		.coherent_dma_mask      = 0xffffffff,
	},
	.num_resources  = ARRAY_SIZE(jz_i2c2_resources),
	.resource       = jz_i2c2_resources,
};

/* PWM */

static struct resource jz_pwm_resources[] = {
	{
		.start	= JZ4770_TCU_BASE_ADDR,
		.end	= JZ4770_TCU_BASE_ADDR + 0x4B,
		.flags	= IORESOURCE_MEM,
	},
};

struct platform_device jz4770_pwm_device = {
	.name = "jz4770-pwm",
	.id   = -1,
	.resource	= jz_pwm_resources,
	.num_resources	= ARRAY_SIZE(jz_pwm_resources),
};

/* RTC */

static struct resource jz_rtc_resources[] = {
	{
		.start	= JZ4770_RTC_BASE_ADDR,
		.end	= JZ4770_RTC_BASE_ADDR + 0x40 - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= IRQ_RTC,
		.end	= IRQ_RTC,
		.flags	= IORESOURCE_IRQ,
	},
};

struct platform_device jz4770_rtc_device = {
	.name		= "jz4770-rtc",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(jz_rtc_resources),
	.resource	= jz_rtc_resources,
};

/* ADC controller */

static struct resource jz_adc_resources[] = {
	{
		/* Assign only the shared registers to the MFD driver. */
		.start	= JZ4770_SADC_BASE_ADDR,
		.end	= JZ4770_SADC_BASE_ADDR + 0x2F,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= IRQ_SADC,
		.end	= IRQ_SADC,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= IRQ_SADC_BASE,
		.end	= IRQ_SADC_BASE + SADC_IRQ_NUM - 1,
		.flags	= IORESOURCE_IRQ,
	},
};

struct platform_device jz4770_adc_device = {
	.name		= "jz4770-adc",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(jz_adc_resources),
	.resource	= jz_adc_resources,
};

/* VPU */

static struct resource jz_vpu_resources[] = {
	{
		.start	= JZ4770_AUX_BASE_ADDR,
		.end	= JZ4770_AUX_BASE_ADDR + 0xFFFF,
		.flags	= IORESOURCE_MEM,
	},
	{
		/* TCSM0 is 16K in size, other 48K is reserved. */
		.start	= JZ4770_TCSM0_BASE_ADDR,
		.end	= JZ4770_TCSM0_BASE_ADDR + 0xFFFF,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= IRQ_VPU,
		.end	= IRQ_VPU,
		.flags	= IORESOURCE_IRQ,
	},
};

struct platform_device jz4770_vpu_device = {
	.name		= "jz-vpu",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(jz_vpu_resources),
	.resource	= jz_vpu_resources,
};

/* Watchdog */
static struct resource jz4770_wdt_resources[] = {
	{
		.start = JZ4770_WDT_BASE_ADDR,
		.end   = JZ4770_WDT_BASE_ADDR + 0xC - 1,
		.flags = IORESOURCE_MEM,
	},
};

struct platform_device jz4770_wdt_device = {
	.name		= "jz4740-wdt",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(jz4770_wdt_resources),
	.resource	= jz4770_wdt_resources,
};

static struct platform_device *jz_platform_devices[] __initdata = {
	&jz4770_pinctrl_device,
};

static int __init jz4770_init_platform_devices(void)
{
	return platform_add_devices(jz_platform_devices,
				    ARRAY_SIZE(jz_platform_devices));
}
postcore_initcall(jz4770_init_platform_devices);
