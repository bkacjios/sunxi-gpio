#include <stdio.h>
#include "sunxi_gpio.h"

int main(int argc, char *argv[]) {
	if (sunxi_gpio_init() != 0) {
		printf("Failed initializing GPIO\n");
		return -1;
	}

	printf("SUNXI_PIO_BASE = %x\n", SUNXI_PIO_BASE);
	printf("SUNXI_R_PIO_BASE = %x\n", SUNXI_R_PIO_BASE);

	printf("DPAD_LEFT = %d\n", sunxi_gpio_input(SUNXI_GPL(8)));
	printf("DPAD_RIGHT = %d\n", sunxi_gpio_input(SUNXI_GPL(9)));
	printf("DPAD_UP = %d\n", sunxi_gpio_input(SUNXI_GPG(9)));
	printf("DPAD_DOWN = %d\n", sunxi_gpio_input(SUNXI_GPG(8)));

	printf("BUTTON_B = %d\n", sunxi_gpio_input(SUNXI_GPG(10)));
	printf("BUTTON_Y = %d\n", sunxi_gpio_input(SUNXI_GPG(11)));
	printf("BUTTON_A = %d\n", sunxi_gpio_input(SUNXI_GPG(12)));
	printf("BUTTON_X = %d\n", sunxi_gpio_input(SUNXI_GPG(13)));

	printf("SELECT = %d\n", sunxi_gpio_input(SUNXI_GPL(11)));
	printf("START = %d\n", sunxi_gpio_input(SUNXI_GPG(4)));

	for (int b=SUNXI_GPIO_A; b <= SUNXI_GPIO_I; b++) {
		printf("BANK: %c | ", (char) 65+b);
		for (int p=0; p < 32; p++) {
			printf("%d ", sunxi_gpio_input((b*32)+p));
		}
		printf("\n");
	}

	for (int b=SUNXI_GPIO_L; b <= SUNXI_GPIO_N; b++) {
		printf("BANK: %c | ", (char) 65+b);
		for (int p=0; p < 32; p++) {
			printf("%d ", sunxi_gpio_input((b*32)+p));
		}
		printf("\n");
	}

	for (int b=SUNXI_GPIO_A; b <= SUNXI_GPIO_I; b++) {
		printf("CONF: %c | ", (char) 65+b);
		for (int p=0; p < 32; p++) {
			printf("%d ", sunxi_gpio_get_cfgpin((b*32)+p));
		}
		printf("\n");
	}

	for (int b=SUNXI_GPIO_L; b <= SUNXI_GPIO_N; b++) {
		printf("CONF: %c | ", (char) 65+b);
		for (int p=0; p < 32; p++) {
			printf("%d ", sunxi_gpio_get_cfgpin((b*32)+p));
		}
		printf("\n");
	}

	return 0;
} 
