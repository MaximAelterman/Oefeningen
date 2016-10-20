// blink.c
//
// Example program for bcm2835 library
// Blinks a pin on an off every 0.5 secs
//
// After installing bcm2835, you can build this 
// with something like:
// gcc -o blink blink.c -l bcm2835
// sudo ./blink
//
// Or you can test it before installing with:
// gcc -o blink -I ../../src ../../src/bcm2835.c blink.c
// sudo ./blink
//
// Author: Mike McCauley
// Copyright (C) 2011 Mike McCauley
// $Id: RF22.h,v 1.21 2012/05/30 01:51:25 mikem Exp $
// Blinks on RPi Plug P1 pin 11 (which is GPIO pin 17)

//#include <bcm2835.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

#define PERI_BASE 0x20000000
#define GPIO_BASE 0x200000
#define BCM2835_GPSET0 0x001c
#define BCM2835_GPCLR0 0x0028

#define PIN1 17

#define AANTAL_PINS 8

volatile uint32_t *gpio;
uint32_t *peripherals;
uint32_t *peripherals_base = (uint32_t *) PERI_BASE;

void initpins(uint8_t);
void init();
static void *mapmem(const char*, size_t, int, off_t);
//void write_gpio(volatile uint32_t*, uint32_t);
void set_gpio(uint8_t pin);
void clear_gpio(uint8_t pin);

int main(int argc, char **argv)
{
    if (!bcm2835_init())
      return 1;
	init();
    // Set the pin to be an output
    uint8_t pin = PIN1;
    initpins(pin);
    // Blink
    while (1)
    {
		set_gpio(pin);					//pin hoog maken
		bcm2835_delay(500);
		clear_gpio(pin);			//pin laag maken
		bcm2835_delay(500);
    }
    bcm2835_close();
    return 0;
}

void initpins(uint8_t pin)
{
	bcm2835_gpio_fsel(pin, 0x01);
}

void init()
{
	int memfd;
	if(geteuid() == 0)
	{
		if ((memfd = open("/dev/mem",  O_RDWR | O_SYNC)) < 0)
		{
			fprintf(stderr, "bcm init: geen admin. error %s", strerror(errno));
			goto exit;
		}
		peripherals = mapmem("gpio",  0x01000000,  memfd, (uint32_t)peripherals_base);
		gpio = peripherals + GPIO_BASE/4;
	}
exit:
	return;
}

static void *mapmem(const char *msg, size_t size, int fd, off_t off)
{
	void *map = mmap(NULL, size, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, off);
}

/*
void write_gpio(volatile uint32_t* paddr, uint32_t value)
{
	__sync_synchronize();
	*paddr = value;
	__sync_synchronize();
}
*/

void set_gpio(uint8_t pin)
{
	volatile uint32_t* paddr = gpio + BCM2835_GPSET0 / 4 + pin / 32;
	uint8_t shift = pin % 32;
	bcm2835_peri_write(paddr, 1 << shift);
}

void clear_gpio(uint8_t pin)
{
	volatile uint32_t* paddr = gpio + BCM2835_GPCLR0 / 4 + pin / 32;
	uint8_t shift = pin % 32;
	bcm2835_peri_write(paddr, 1 << shift);
}