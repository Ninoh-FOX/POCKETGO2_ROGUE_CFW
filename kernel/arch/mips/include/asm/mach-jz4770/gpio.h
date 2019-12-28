#ifndef _JZ_GPIO_H
#define _JZ_GPIO_H

#include <asm/mach-generic/gpio.h>

#define JZ_GPIO_PORTA(x) ((x) + 32 * 0)
#define JZ_GPIO_PORTB(x) ((x) + 32 * 1)
#define JZ_GPIO_PORTC(x) ((x) + 32 * 2)
#define JZ_GPIO_PORTD(x) ((x) + 32 * 3)
#define JZ_GPIO_PORTE(x) ((x) + 32 * 4)
#define JZ_GPIO_PORTF(x) ((x) + 32 * 5)

void jz_gpio_enable_pullup(unsigned gpio);
void jz_gpio_disable_pullup(unsigned gpio);

#endif /* _JZ_GPIO_H */
