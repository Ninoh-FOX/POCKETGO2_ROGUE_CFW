/*
 * Copyright (C) Ingenic Semiconductor Inc.
 * Original driver by Lutts Wolf <slcao@ingenic.cn>.
 *
 * Copyright (C) 2012, Maarten ter Huurne <maarten@treewalker.org>
 * Updated to match ALSA changes and restructured to better fit driver model.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/delay.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>

#include <sound/core.h>
#include <sound/initval.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>

#include <asm/mach-jz4770/dma.h>
#include <asm/mach-jz4770/jz4770aic.h>
#include <asm/mach-jz4770/jz4770cpm.h>

#include "jz4770-pcm.h"


#define I2S_RFIFO_DEPTH 32
#define I2S_TFIFO_DEPTH 64


struct jz4770_i2s {
	//struct resource *mem;
	void __iomem *base;
	//dma_addr_t phys_base;

	//struct clk *clk_aic;
	//struct clk *clk_i2s;

	//struct jz4740_pcm_config pcm_config_playback;
	//struct jz4740_pcm_config pcm_config_capture;
};

static int jz_i2s_debug = 1;
module_param(jz_i2s_debug, int, 0644);
#define I2S_DEBUG_MSG(msg...)			\
	do {					\
		if (jz_i2s_debug)		\
			printk("I2S: " msg);	\
	} while(0)

static struct jz4770_pcm_dma_params jz4770_i2s_pcm_stereo_out = {
	.channel	= DMA_ID_AIC_TX,
	.dma_addr	= AIC_DR,
	.dma_size	= 2,
};

static struct jz4770_pcm_dma_params jz4770_i2s_pcm_stereo_in = {
	.channel	= DMA_ID_AIC_RX,
	.dma_addr	= AIC_DR,
	.dma_size	= 2,
};

static int jz4770_i2s_startup(struct snd_pcm_substream *substream, struct snd_soc_dai *dai)
{
	return 0;
}

static void jz4770_i2s_shutdown(struct snd_pcm_substream *substream, struct snd_soc_dai *dai)
{
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
	} else {
	}

	return;
}

static int jz4770_i2s_set_dai_fmt(struct snd_soc_dai *cpu_dai,
		unsigned int fmt)
{
	/* interface format */
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		/* 1 : ac97 , 0 : i2s */
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		break;
	default:
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBS_CFS:
	        /* 0 : slave */
		break;
	case SND_SOC_DAIFMT_CBM_CFS:
		/* 1 : master */
		break;
	default:
		break;
	}

	return 0;
}

/*
* Set Jz4770 Clock source
*/
static int jz4770_i2s_set_dai_sysclk(struct snd_soc_dai *cpu_dai,
		int clk_id, unsigned int freq, int dir)
{
	return 0;
}

static int is_recording = 0;
static int is_playing = 0;

static void jz4770_snd_tx_ctrl(int on)
{
	I2S_DEBUG_MSG("enter %s, on = %d\n", __func__, on);
	if (on) {
		is_playing = 1;

                /* enable replay */
	        __i2s_enable_transmit_dma();
		__i2s_enable_replay();
		__i2s_enable();

	} else {
		is_playing = 0;

		/* disable replay & capture */
		__i2s_disable_replay();
		__i2s_disable_transmit_dma();

		if (!is_recording)
			__i2s_disable();
	}
}

static void jz4770_snd_rx_ctrl(int on)
{
	I2S_DEBUG_MSG("enter %s, on = %d\n", __func__, on);
	if (on) {
		is_recording = 1;

                /* enable capture */
		__i2s_enable_receive_dma();
		__i2s_enable_record();
		__i2s_enable();

	} else {
		is_recording = 0;

                /* disable replay & capture */
		__i2s_disable_record();
		__i2s_disable_receive_dma();

		if (!is_playing)
			__i2s_disable();
	}
}

static int jz4770_i2s_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params,
				struct snd_soc_dai *dai)
{
	int channels = params_channels(params);

	I2S_DEBUG_MSG("enter %s, substream = %s\n",
		      __func__,
		      (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) ? "playback" : "capture");

	/* NOTE: when use internal codec, nothing to do with sample rate here.
	 *	 if use external codec and bit clock is provided by I2S
	 *	 controller, set clock rate here!!!
	 */

	/* set channel params */
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		snd_soc_dai_set_dma_data(dai, substream,
					 &jz4770_i2s_pcm_stereo_out);
		if (channels == 1) {
			__aic_enable_mono2stereo();
			__aic_out_channel_select(0);
		} else {
			__aic_disable_mono2stereo();
			__aic_out_channel_select(1);
		}
	} else
		snd_soc_dai_set_dma_data(dai, substream,
					 &jz4770_i2s_pcm_stereo_in);


	/* set format */
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		switch (params_format(params)) {
		case SNDRV_PCM_FORMAT_U8:
		case SNDRV_PCM_FORMAT_S8:
			__i2s_set_oss_sample_size(8);
			break;
		case SNDRV_PCM_FORMAT_S16_LE:
			__i2s_set_oss_sample_size(16);
			break;
		case SNDRV_PCM_FORMAT_S24_3LE:
			__i2s_set_oss_sample_size(24);
			break;
		}
	} else {
		int sound_data_width = 0;
		switch (params_format(params)) {
		case SNDRV_PCM_FORMAT_S8:
			__i2s_set_iss_sample_size(8);
			sound_data_width = 8;
			break;
		case SNDRV_PCM_FORMAT_S16_LE:
			__i2s_set_iss_sample_size(16);
			sound_data_width = 16;
			break;
		case SNDRV_PCM_FORMAT_S24_3LE:
		default:
			__i2s_set_iss_sample_size(24);
			sound_data_width = 24;
			break;
		}
		//__i2s_set_receive_trigger(((16 * 8) / sound_data_width) / 2);
		/* use 2 sample as trigger */
		__i2s_set_receive_trigger((sound_data_width / 8 * channels) * 2 / 2 - 1);
	}

	return 0;
}

static int jz4770_i2s_trigger(struct snd_pcm_substream *substream, int cmd, struct snd_soc_dai *dai)
{
	int ret = 0;

	I2S_DEBUG_MSG("enter %s, substream = %s cmd = %d\n",
		      __func__,
		      (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
				? "playback" : "capture",
		      cmd);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		if (substream->stream == SNDRV_PCM_STREAM_CAPTURE)
			jz4770_snd_rx_ctrl(1);
		else
			jz4770_snd_tx_ctrl(1);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		if (substream->stream == SNDRV_PCM_STREAM_CAPTURE)
			jz4770_snd_rx_ctrl(0);
		else
			jz4770_snd_tx_ctrl(0);
		break;
	default:
		ret = -EINVAL;
	}

	return ret;
}

static int jz4770_i2s_dai_probe(struct snd_soc_dai *dai)
{
	cpm_start_clock(CGM_AIC);
	/* Select exclk as i2s clock */
	cpm_set_clock(CGU_I2SCLK, JZ_EXTAL);
	REG_AIC_I2SCR |= AIC_I2SCR_ESCLK;

	__i2s_disable();
	__aic_disable_transmit_dma();
	__aic_disable_receive_dma();
	__i2s_disable_record();
	__i2s_disable_replay();
	__i2s_disable_loopback();

	__i2s_internal_codec();
	__i2s_as_slave();
	__i2s_select_i2s();
	__aic_select_i2s();
	__aic_play_lastsample();
	__i2s_set_transmit_trigger(I2S_TFIFO_DEPTH / 4);
	__i2s_set_receive_trigger(I2S_RFIFO_DEPTH / 4);
	__i2s_send_rfirst();

	__aic_write_tfifo(0x0);
	__aic_write_tfifo(0x0);
	__i2s_enable_replay();
	__i2s_enable();
	mdelay(1);

	jz4770_snd_tx_ctrl(0);
	jz4770_snd_rx_ctrl(0);

	return 0;
}

static int jz4770_i2s_dai_remove(struct snd_soc_dai *dai)
{
	//struct jz4770_i2s *i2s = snd_soc_dai_get_drvdata(dai);

	//clk_disable(i2s->clk_aic);
	return 0;
}

#ifdef CONFIG_PM
static int jz4770_i2s_suspend(struct snd_soc_dai *dai)
{
	return 0;
}

static int jz4770_i2s_resume(struct snd_soc_dai *dai)
{
	return 0;
}

#else
#define jz4770_i2s_suspend	NULL
#define jz4770_i2s_resume	NULL
#endif

static struct snd_soc_dai_ops jz4770_i2s_dai_ops = {
	.startup		= jz4770_i2s_startup,
	.shutdown		= jz4770_i2s_shutdown,
	.trigger		= jz4770_i2s_trigger,
	.hw_params		= jz4770_i2s_hw_params,
	.set_fmt		= jz4770_i2s_set_dai_fmt,
	.set_sysclk		= jz4770_i2s_set_dai_sysclk,
};

#define JZ4770_I2S_RATES (SNDRV_PCM_RATE_8000  | SNDRV_PCM_RATE_11025 | \
			  SNDRV_PCM_RATE_16000 | SNDRV_PCM_RATE_22050 | \
			  SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_44100 | \
			  SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_96000)

/*#define JZ_I2S_FORMATS (SNDRV_PCM_FMTBIT_S8  | SNDRV_PCM_FMTBIT_U8 | \
			SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S24_3LE)*/
#define JZ_I2S_FORMATS (SNDRV_PCM_FMTBIT_S8 | SNDRV_PCM_FMTBIT_U8 | \
			SNDRV_PCM_FMTBIT_S16_LE)

static struct snd_soc_dai_driver jz4770_i2s_dai = {
	.probe			= jz4770_i2s_dai_probe,
	.remove			= jz4770_i2s_dai_remove,
	.suspend		= jz4770_i2s_suspend,
	.resume			= jz4770_i2s_resume,

	.ops			= &jz4770_i2s_dai_ops,

	.playback = {
		.channels_min	= 1,
		.channels_max	= 2,
		.rates		= JZ4770_I2S_RATES,
		.formats	= JZ_I2S_FORMATS,
	},
	.capture = {
		.channels_min	= 1,
		.channels_max	= 2,
		.rates		= JZ4770_I2S_RATES,
		.formats	= JZ_I2S_FORMATS,
	},
	.symmetric_rates	= 1,
};

static const struct snd_soc_component_driver jz4770_i2s_component = {
	.name			= "jz4770-i2s",
};

static int jz4770_i2s_dev_probe(struct platform_device *pdev)
{
	struct jz4770_i2s *i2s;
	struct resource *mem;
	int ret;

	i2s = devm_kzalloc(&pdev->dev, sizeof(*i2s), GFP_KERNEL);
	if (!i2s)
		return -ENOMEM;

	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!mem) {
		dev_err(&pdev->dev, "Failed to get DAI registers resource\n");
		return -ENOENT;
	}

	i2s->base = devm_request_and_ioremap(&pdev->dev, mem);
	if (!i2s->base) {
		dev_err(&pdev->dev, "Failed to request and map DAI registers\n");
		return -EBUSY;
	}

	//i2s->phys_base = i2s->mem->start;

	/*i2s->clk_aic = devm_clk_get(&pdev->dev, "aic");
	if (IS_ERR(i2s->clk_aic))
		return PTR_ERR(i2s->clk_aic);
	*/

	/*i2s->clk_i2s = devm_clk_get(&pdev->dev, "i2s");
	if (IS_ERR(i2s->clk_i2s))
		return PTR_ERR(i2s->clk_i2s);
	*/

	platform_set_drvdata(pdev, i2s);
	ret = snd_soc_register_component(&pdev->dev, &jz4770_i2s_component,
					 &jz4770_i2s_dai, 1);
	if (ret) {
		dev_err(&pdev->dev, "Failed to register DAI\n");
		return ret;
	}

	return 0;
}

static int jz4770_i2s_dev_remove(struct platform_device *pdev)
{
	snd_soc_unregister_component(&pdev->dev);

	platform_set_drvdata(pdev, NULL);

	return 0;
}

static struct platform_driver jz4770_i2s_driver = {
	.probe = jz4770_i2s_dev_probe,
	.remove = jz4770_i2s_dev_remove,
	.driver = {
		.name = "jz4770-i2s",
		.owner = THIS_MODULE,
	},
};

module_platform_driver(jz4770_i2s_driver);

MODULE_AUTHOR("Lutts Wolf <slcao@ingenic.cn>");
MODULE_DESCRIPTION("jz4770 I2S SoC Interface");
MODULE_LICENSE("GPL");
