// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 *
 * Based on earlier arch/arm/cpu/armv7/sunxi/gpio.c:
 *
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 */

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>

#include "sunxi_gpio.h"

unsigned int SUNXI_PIO_BASE = 0;
unsigned int SUNXI_R_PIO_BASE = 0;

int sunxi_gpio_init(void) {
    int fd;
    unsigned int addr_start, addr_offset;
    unsigned int PageSize, PageMask;
    void *pc;

    fd = open("/dev/mem", O_RDWR);
    if (fd < 0) {
        return (-1);
    }

    PageSize = sysconf(_SC_PAGESIZE);
    PageMask = (~(PageSize - 1));

    addr_start = SW_PORTC_IO_BASE & PageMask;
    addr_offset = SW_PORTC_IO_BASE & ~PageMask;

    pc = (void *) mmap(0, PageSize * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr_start);
    if (pc == MAP_FAILED) {
        return (-1);
    }

    SUNXI_PIO_BASE = (unsigned int) pc;
    SUNXI_PIO_BASE += addr_offset;

    addr_start = SW_R_PIO_BASE & PageMask;
    addr_offset = SW_R_PIO_BASE & ~PageMask;

    pc = (void *) mmap(0, PageSize * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr_start);
    if (pc == MAP_FAILED) {
        return (-1);
    }

    SUNXI_R_PIO_BASE = (unsigned int) pc;
    SUNXI_R_PIO_BASE += addr_offset;

    close(fd);
    return 0;
}

unsigned int sunxi_gpio_get_base(unsigned int pin) {
    if (pin < SUNXI_GPIO_L_START)
        return SUNXI_PIO_BASE;
    else
        return SUNXI_R_PIO_BASE;
}

unsigned int sunxi_gpio_get_pin(unsigned int pin) {
     if (pin < SUNXI_GPIO_L_START)
        return pin;
    else
        return pin-SUNXI_GPIO_L_START;
}

int sunxi_gpio_set_cfgpin(unsigned int pin, unsigned int val) {
    unsigned int cfg;
    unsigned int base = sunxi_gpio_get_base(pin);
    unsigned int pin_n = sunxi_gpio_get_pin(pin);
    unsigned int bank = GPIO_BANK(pin_n);
    unsigned int index = GPIO_CFG_INDEX(pin_n);
    unsigned int offset = GPIO_CFG_OFFSET(pin_n);

    if (base == 0) {
        return -1;
    }

    struct sunxi_gpio *pio =
            &((struct sunxi_gpio_reg *) base)->gpio_bank[bank];

    cfg = *(&pio->cfg[0] + index);
    cfg &= ~(0xf << offset);
    cfg |= val << offset;

    *(&pio->cfg[0] + index) = cfg;

    return 0;
}

int sunxi_gpio_get_cfgpin(unsigned int pin) {
    unsigned int cfg;
    unsigned int base = sunxi_gpio_get_base(pin);
    unsigned int pin_n = sunxi_gpio_get_pin(pin);
    unsigned int bank = GPIO_BANK(pin_n);
    unsigned int index = GPIO_CFG_INDEX(pin_n);
    unsigned int offset = GPIO_CFG_OFFSET(pin_n);

    if (base == 0) {
        return -1;
    }

    struct sunxi_gpio *pio = &((struct sunxi_gpio_reg *) base)->gpio_bank[bank];
    cfg = *(&pio->cfg[0] + index);
    cfg >>= offset;

    return (cfg & 0xf);
}

int sunxi_gpio_output(unsigned int pin, unsigned int val) {
    unsigned int base = sunxi_gpio_get_base(pin);
    unsigned int pin_n = sunxi_gpio_get_pin(pin);
    unsigned int bank = GPIO_BANK(pin_n);
    unsigned int num = GPIO_NUM(pin_n);

    if (base == 0) {
        return -1;
    }

    struct sunxi_gpio *pio = &((struct sunxi_gpio_reg *) base)->gpio_bank[bank];

    if (val)
        *(&pio->dat) |= 1 << num;
    else
        *(&pio->dat) &= ~(1 << num);

    return 0;
}

int sunxi_gpio_pullup(unsigned int pin, unsigned int pull) {
    unsigned int cfg;
    unsigned int base = sunxi_gpio_get_base(pin);
    unsigned int pin_n = sunxi_gpio_get_pin(pin);
    unsigned int bank = GPIO_BANK(pin_n);
    unsigned int index = GPIO_PULL_INDEX(pin_n);
    unsigned int offset = GPIO_PULL_OFFSET(pin_n);

    if (base == 0) {
        return -1;
    }

    struct sunxi_gpio *pio =
            &((struct sunxi_gpio_reg *) base)->gpio_bank[bank];

    cfg = *(&pio->pull[0] + index);
    cfg &= ~(0x3 << offset);
    cfg |= pull << offset;

    *(&pio->pull[0] + index) = cfg;

    return 0;
}

int sunxi_gpio_input(unsigned int pin) {
    unsigned int dat;
    unsigned int base = sunxi_gpio_get_base(pin);
    unsigned int pin_n = sunxi_gpio_get_pin(pin);
    unsigned int bank = GPIO_BANK(pin_n);
    unsigned int num = GPIO_NUM(pin_n);

    if (base == 0) {
        return -1;
    }

    struct sunxi_gpio *pio = &((struct sunxi_gpio_reg *) base)->gpio_bank[bank];

    dat = *(&pio->dat);
    dat >>= num;

    return (dat & 0x1);
}
