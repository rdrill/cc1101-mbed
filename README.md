# cc1101-mbed
STM32 MBED port for the Space_Teddy's Arduino\Raspberry library

### How to setup

- open CC1101.h and fit your module settings by editing the flags
```
#define MY_ADDR 0x01
#define MY_CHAN 0x02
#define TG_ADDR 0x03 //TX target address
#define MY_FREQ 0x02
#define MY_MODE 0x01
```
- open CC1101.cpp and setup the SPI and pins
```
Serial dbs(PA_2, PA_3); // debug serial
DigitalOut csn(PB_12);
DigitalIn gdo2(PA_8);
DigitalIn RDmiso(PA_9);
SPI spi(PB_15, PB_14, PB_13);
```

### TODO list

- adequate documentation should be created
- interface init constructor should be added
- debug serial constructor should be added

