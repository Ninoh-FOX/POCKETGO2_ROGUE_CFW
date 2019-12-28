#ifndef __JZ4770_PLATFORM_H__
#define __JZ4770_PLATFORM_H__

#include <linux/platform_device.h>

extern struct platform_device jz4770_adc_device;
extern struct platform_device jz4770_i2c0_device;
extern struct platform_device jz4770_i2c1_device;
extern struct platform_device jz4770_i2c2_device;
extern struct platform_device jz4770_i2s_device;
extern struct platform_device jz4770_icdc_device;
extern struct platform_device jz4770_lcd_device;
extern struct platform_device jz4770_msc0_device;
extern struct platform_device jz4770_msc1_device;
extern struct platform_device jz4770_msc2_device;
extern struct platform_device jz4770_pcm_device;
extern struct platform_device jz4770_pwm_device;
extern struct platform_device jz4770_rtc_device;
extern struct platform_device jz4770_usb_ohci_device;
extern struct platform_device jz4770_usb_otg_device;
extern struct platform_device jz4770_usb_otg_xceiv_device;
extern struct platform_device jz4770_vpu_device;
extern struct platform_device jz4770_wdt_device;

#endif /* __JZ4770_PLATFORM_H__ */
