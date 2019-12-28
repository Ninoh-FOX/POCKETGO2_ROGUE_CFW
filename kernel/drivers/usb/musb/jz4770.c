/*
 * Author: River <zwang@ingenic.cn>
 * Restructured by Maarten ter Huurne <maarten@treewalker.org>, using the
 * tusb6010 module as a template.
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/io.h>
#include <linux/jiffies.h>
#include <linux/gpio.h>
#include <linux/usb/otg.h>
#include <linux/usb/usb_phy_gen_xceiv.h>
#include <linux/platform_data/usb-musb-jz4770.h>
#include <linux/act8600_power.h>

#include <asm/mach-jz4770/jz4770cpm.h>

#include "musb_core.h"


struct jz_musb_glue {
	struct device *dev;
	struct platform_device *musb;
	struct clk *clk;
	struct timer_list gpio_id_debounce_timer;
	unsigned long gpio_id_debounce_jiffies;
};

static inline void jz_musb_phy_enable(void)
{
	printk(KERN_INFO "jz4760: Enable USB PHY.\n");

	__cpm_enable_otg_phy();

	/* Wait PHY Clock Stable. */
	udelay(300);
}

static inline void jz_musb_phy_disable(void)
{
	printk(KERN_INFO "jz4760: Disable USB PHY.\n");

	__cpm_suspend_otg_phy();
}

static inline void jz_musb_phy_reset(void)
{
	REG_CPM_USBPCR |= USBPCR_POR;
	udelay(30);
	REG_CPM_USBPCR &= ~USBPCR_POR;

	udelay(300);
}

static inline void jz_musb_set_device_only_mode(void)
{
	printk(KERN_INFO "jz4760: Device only mode.\n");

	/* Device Mode. */
	REG_CPM_USBPCR &= ~USBPCR_USB_MODE;

	REG_CPM_USBPCR |= USBPCR_VBUSVLDEXT;
}

static inline void jz_musb_set_normal_mode(void)
{
	printk(KERN_INFO "jz4760: Normal mode.\n");

	/* OTG Mode. */
	REG_CPM_USBPCR |= USBPCR_USB_MODE;

	REG_CPM_USBPCR &= ~(USBPCR_VBUSVLDEXT |
			    USBPCR_VBUSVLDEXTSEL |
			    USBPCR_OTG_DISABLE);

	REG_CPM_USBPCR = (REG_CPM_USBPCR & ~USBPCR_IDPULLUP_MASK)
		       | USBPCR_IDPULLUP_ALWAYS;
}

static inline void jz_musb_init_regs(struct musb *musb)
{
	/* fil */
	REG_CPM_USBVBFIL = 0x80;

	/* rdt */
	REG_CPM_USBRDT = 0x96;

	/* rdt - filload_en */
	REG_CPM_USBRDT |= (1 << 25);

	/* TXRISETUNE & TXVREFTUNE. */
	REG_CPM_USBPCR &= ~(USBPCR_TXRISETUNE_MASK | USBPCR_TXVREFTUNE_MASK);
	REG_CPM_USBPCR |= (3 << USBPCR_TXRISETUNE_LSB)
			| (5 << USBPCR_TXVREFTUNE_LSB);

	jz_musb_set_normal_mode();

	jz_musb_phy_reset();
}

static void jz_musb_set_vbus(struct musb *musb, int is_on)
{
	u8 devctl = musb_readb(musb->mregs, MUSB_DEVCTL);

	/* HDRC controls CPEN, but beware current surges during device
	 * connect.  They can trigger transient overcurrent conditions
	 * that must be ignored.
	 */

	if (is_on) {
		musb->is_active = 1;
		musb->xceiv->otg->default_a = 1;
		musb->xceiv->state = OTG_STATE_A_WAIT_VRISE;
		devctl |= MUSB_DEVCTL_SESSION;

		MUSB_HST_MODE(musb);

		//act8600_set_power_mode(VBUS_POWERED_BY_5VIN);
	} else {
		musb->is_active = 0;

		//act8600_set_power_mode(VBUS_POWERED_EXTERNALLY);

		/* NOTE:  we're skipping A_WAIT_VFALL -> A_IDLE and
		 * jumping right to B_IDLE...
		 */

		musb->xceiv->otg->default_a = 0;
		musb->xceiv->state = OTG_STATE_B_IDLE;
		devctl &= ~MUSB_DEVCTL_SESSION;

		MUSB_DEV_MODE(musb);
	}
	musb_writeb(musb->mregs, MUSB_DEVCTL, devctl);

	dev_dbg(musb->xceiv->dev, "VBUS %s, devctl %02x "
		/* otg %3x conf %08x prcm %08x */ "\n",
		usb_otg_state_string(musb->xceiv->state),
		musb_readb(musb->mregs, MUSB_DEVCTL));
}

/* ---------------------- OTG ID PIN Routines ---------------------------- */

static void do_otg_id_pin_state(struct musb *musb)
{
	struct device *dev = musb->controller;
	struct musb_hdrc_platform_data *pdata = dev->platform_data;
	struct jz_otg_board_data *board_data = pdata->board_data;

	unsigned int default_a = !gpio_get_value(board_data->gpio_id_pin);

	dev_info(dev, "USB OTG default mode: %s\n", default_a ? "A" : "B");

	musb->xceiv->otg->default_a = default_a;

	jz_musb_set_vbus(musb, default_a);
}

static void otg_id_pin_stable_func(unsigned long data)
{
	struct musb *musb = (struct musb *)data;

	do_otg_id_pin_state(musb);
}

static irqreturn_t jz_musb_otg_id_irq(int irq, void *data)
{
	struct jz_musb_glue *glue = data;

	mod_timer(&glue->gpio_id_debounce_timer,
		  jiffies + glue->gpio_id_debounce_jiffies);

	return IRQ_HANDLED;
}

static int otg_id_pin_setup(struct musb *musb)
{
	struct device *dev = musb->controller;
	struct jz_musb_glue *glue = dev_get_drvdata(dev->parent);
	struct musb_hdrc_platform_data *pdata = dev->platform_data;
	struct jz_otg_board_data *board_data = pdata->board_data;
	int id_pin = board_data->gpio_id_pin;
	int ret;

	ret = devm_gpio_request(dev, id_pin, "USB OTG ID");
	if (ret) {
		dev_err(dev, "Failed to request USB OTG ID pin %d: %d\n",
			id_pin, ret);
		return ret;
	}

	gpio_direction_input(id_pin);
	jz_gpio_disable_pullup(id_pin);

	glue->gpio_id_debounce_jiffies =
			msecs_to_jiffies(board_data->gpio_id_debounce_ms);

	/* Update OTG ID PIN state. */
	do_otg_id_pin_state(musb);
	setup_timer(&glue->gpio_id_debounce_timer, otg_id_pin_stable_func,
		    (unsigned long)musb);

	ret = devm_request_irq(dev, gpio_to_irq(id_pin), jz_musb_otg_id_irq,
			       IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
			       "otg-id-irq", glue);
	if (ret) {
		dev_err(dev, "Failed to request USB OTG ID IRQ: %d\n", ret);
		return ret;
	}

	return ret;
}

static void otg_id_pin_cleanup(struct musb *musb)
{
	struct device *dev = musb->controller;
	struct jz_musb_glue *glue = dev_get_drvdata(dev->parent);
	struct musb_hdrc_platform_data *pdata = dev->platform_data;
	struct jz_otg_board_data *board_data = pdata->board_data;

	devm_free_irq(dev, gpio_to_irq(board_data->gpio_id_pin), glue);
	del_timer(&glue->gpio_id_debounce_timer);
}

/* ---------------------------------------------------------------- */

static irqreturn_t jz_musb_interrupt(int irq, void *__hci)
{
	unsigned long	flags;
	struct musb	*musb = __hci;

	irqreturn_t rv, rv_dma, rv_usb;
	rv = rv_dma = rv_usb = IRQ_NONE;

	spin_lock_irqsave(&musb->lock, flags);

#if defined(CONFIG_USB_INVENTRA_DMA)
	if (musb->b_dma_share_usb_irq)
		rv_dma = musb_call_dma_controller_irq(irq, musb);
#endif

	musb->int_usb = musb_readb(musb->mregs, MUSB_INTRUSB);
	musb->int_tx = musb_readw(musb->mregs, MUSB_INTRTX);
	musb->int_rx = musb_readw(musb->mregs, MUSB_INTRRX);

	if (musb->int_usb || musb->int_tx || musb->int_rx)
		rv_usb = musb_interrupt(musb);

	spin_unlock_irqrestore(&musb->lock, flags);

	rv = (rv_dma == IRQ_HANDLED || rv_usb == IRQ_HANDLED) ?
		IRQ_HANDLED : IRQ_NONE;

	return rv;
}

static int jz_musb_platform_init(struct musb *musb)
{
	struct device *dev = musb->controller;
	struct jz_musb_glue *glue = dev_get_drvdata(dev->parent);
	struct clk *clk;

	musb->xceiv = usb_get_phy(USB_PHY_TYPE_USB2);
	if (!musb->xceiv) {
		pr_err("HS USB OTG: no transceiver configured\n");
		return -ENODEV;
	}

	musb->b_dma_share_usb_irq = 1;
	musb->isr = jz_musb_interrupt;

	clk = devm_clk_get(dev, "usb");
	if (IS_ERR(clk)) {
		int ret = PTR_ERR(clk);
		dev_err(dev, "Failed to get clock: %d\n", ret);
		return ret;
	}
	glue->clk = clk;
	clk_enable(clk);

	jz_musb_init_regs(musb);

	/* host mode and otg(host) depend on the id pin */
	return otg_id_pin_setup(musb);
}

static int jz_musb_platform_exit(struct musb *musb)
{
	struct jz_musb_glue *glue = dev_get_drvdata(musb->controller->parent);

	jz_musb_phy_disable();

	clk_disable(glue->clk);

	otg_id_pin_cleanup(musb);

	usb_nop_xceiv_unregister();

	return 0;
}

static void jz_musb_platform_enable(struct musb *musb)
{
	jz_musb_phy_enable();
}

static void jz_musb_platform_disable(struct musb *musb)
{
	jz_musb_phy_disable();
}

static const struct musb_platform_ops jz_musb_ops = {
	.init		= jz_musb_platform_init,
	.exit		= jz_musb_platform_exit,

	.enable		= jz_musb_platform_enable,
	.disable	= jz_musb_platform_disable,

	.set_vbus	= jz_musb_set_vbus,
};

static int jz_musb_probe(struct platform_device *pdev)
{
	struct musb_hdrc_platform_data	*pdata = pdev->dev.platform_data;
	struct platform_device		*musb;
	struct jz_musb_glue		*glue;

	int				ret = -ENOMEM;

	glue = kzalloc(sizeof(*glue), GFP_KERNEL);
	if (!glue) {
		dev_err(&pdev->dev, "failed to allocate glue context\n");
		goto err0;
	}

	musb = platform_device_alloc("musb-hdrc", PLATFORM_DEVID_AUTO);
	if (!musb) {
		dev_err(&pdev->dev, "failed to allocate musb device\n");
		goto err1;
	}

	musb->dev.parent		= &pdev->dev;
	musb->dev.dma_mask		= &musb->dev.coherent_dma_mask;
	musb->dev.coherent_dma_mask	= DMA_BIT_MASK(32);

	glue->dev			= &pdev->dev;
	glue->musb			= musb;

	pdata->platform_ops		= &jz_musb_ops;

	platform_set_drvdata(pdev, glue);

	ret = platform_device_add_resources(musb, pdev->resource,
			pdev->num_resources);
	if (ret) {
		dev_err(&pdev->dev, "failed to add resources\n");
		goto err2;
	}

	ret = platform_device_add_data(musb, pdata, sizeof(*pdata));
	if (ret) {
		dev_err(&pdev->dev, "failed to add platform_data\n");
		goto err2;
	}

	ret = platform_device_add(musb);
	if (ret) {
		dev_err(&pdev->dev, "failed to register musb device\n");
		goto err2;
	}

	return 0;

err2:
	platform_device_put(musb);

err1:
	kfree(glue);

err0:
	return ret;
}

static int jz_musb_remove(struct platform_device *pdev)
{
	struct jz_musb_glue *glue = platform_get_drvdata(pdev);

	platform_device_unregister(glue->musb);
	kfree(glue);

	return 0;
}

static struct platform_driver jz_musb_driver = {
	.probe		= jz_musb_probe,
	.remove		= jz_musb_remove,
	.driver		= {
		.name	= "musb-jz",
		.owner	= THIS_MODULE,
	},
};

module_platform_driver(jz_musb_driver);

MODULE_DESCRIPTION("JZ4770 MUSB Glue Layer");
MODULE_AUTHOR("River <zwang@ingenic.cn>");
MODULE_LICENSE("GPL v2");
