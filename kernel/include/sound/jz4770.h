#ifndef __SOUND_JZ4770_H
#define __SOUND_JZ4770_H

enum jz4770_icdc_mic_mode {
	JZ4770_MIC_NONE,
	JZ4770_MIC_1,
	JZ4770_MIC_2,
	JZ4770_MIC_DUAL_MONO,
	JZ4770_MIC_STEREO_1L2R,
	JZ4770_MIC_STEREO_1R2L,
};

struct jz4770_icdc_platform_data {
	enum jz4770_icdc_mic_mode mic_mode;
};

#endif /* __SOUND_JZ4770_H */
