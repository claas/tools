#!/bin/bash

TARGET=arm-none-linux-gnueabi
RAMMAP=rammap

$TARGET-gcc -Wall -Werror -O3 -nostdlib -nostartfiles -ffreestanding -c $1.c -o $1.o
$TARGET-ld -T $RAMMAP $1.o -o build.elf
$TARGET-objdump -D build.elf > build.list
$TARGET-objcopy build.elf -O srec build.srec
$TARGET-objcopy build.elf -O binary start.bin
rm $1.o build.elf build.list build.srec
