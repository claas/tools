#!/bin/bash

QEMU=~/buildspace/qemu/arm-softmmu/qemu-system-arm
NANDNAME=$1

$QEMU -M beagle -mtdblock $NANDNAME -serial stdio -s -S
