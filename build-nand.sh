#!/bin/bash

NAME=$1

NANDNAME=$2
NANDSIZE=0xe80000

XLOADER=~/archive/x-load.bin.ift
#UBOOT=~/workspace/u-boot/u-boot.bin
UBOOT=~/archive/u-boot.bin

OBJCOPY=arm-rtemseabi4.11-objcopy
MKIMAGE=~/workspace/u-boot/tools/mkimage
NANDFLASH=~/buildspace/qemu/arm-softmmu/bb_nandflash.sh
NANDFLASH_ECC=~/buildspace/qemu/arm-softmmu/bb_nandflash_ecc

ADDRESS=0x80000000
ENTRY=0x80000000

$OBJCOPY -R -S -O binary $NAME.exe $NAME.bin
cat $NAME.bin | gzip -9 >$NAME.gz
$MKIMAGE -A arm -O rtems -T kernel -a $ADDRESS -e $ENTRY -n "RTEMS" -d $NAME.gz $NAME.img
rm $NAME.bin $NAME.gz

$NANDFLASH $XLOADER $NANDNAME x-loader
$NANDFLASH $UBOOT $NANDNAME u-boot
$NANDFLASH $NAME.img $NANDNAME kernel

$NANDFLASH_ECC $NANDNAME 0x0 $NANDSIZE
