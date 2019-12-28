#ifndef __LINUX_JZ4770_ADC
#define __LINUX_JZ4770_ADC

#include <linux/bitops.h>

struct device;

/*
 * jz4770_adc_set_config - Configure a JZ4740 adc device
 * @dev: Pointer to a jz4770-adc device
 * @mask: Mask for the config value to be set
 * @val: Value to be set
 *
 * This function can be used by the JZ4770 ADC mfd cells to configure their
 * options in the shared config register.
*/
int jz4770_adc_set_config(struct device *dev, uint32_t mask, uint32_t val);
int jz4770_adc_set_adcmd(struct device *dev);
#define JZ_ADC_CONFIG_SPZZ		BIT(31)
#define JZ_ADC_CONFIG_WIRE_SEL		BIT(23)
#define JZ_ADC_CONFIG_CMD_SEL		BIT(22)
#define JZ_ADC_CONFIG_RPU(x)		((x) << 16)
#define JZ_ADC_CONFIG_RPU_MASK		JZ_ADC_CONFIG_RPU(0x3F)
#define JZ_ADC_CONFIG_DMA_EN		BIT(15)
#define JZ_ADC_CONFIG_XYZ(x)		((x) << 13)
#define JZ_ADC_CONFIG_XYZ_MASK		JZ_ADC_CONFIG_XYZ(0x3)
#define JZ_ADC_CONFIG_SAMPLE_NUM(x)	((x) << 10)
#define JZ_ADC_CONFIG_SAMPLE_NUM_MASK	JZ_ADC_CONFIG_SAMPLE_NUM(0x7)
#define JZ_ADC_CONFIG_CMD(x)		(x)
#define JZ_ADC_CONFIG_CMD_MASK		JZ_ADC_CONFIG_CMD(0x3)

#endif
