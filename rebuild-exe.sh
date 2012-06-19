#!/bin/bash

NAME=$1

export PATH=$PATH:/opt/rtems-4.11/bin
export RTEMS_MAKEFILE_PATH=~/buildspace/install-rtems/arm-rtemseabi4.11/beagle_mzx

rm $NAME.*
rm beagle-$NAME-nand.img

cd ~/buildspace/install-rtems
rm -rf *
cd ~/buildspace/build-rtems
rm -rf *
../rtems/configure --target=arm-rtemseabi4.11 --disable-posix --disable-networking --disable-cxx --enable-rtems-debug --enable-rtemsbsp=beagle_mzx --prefix=/home/claas/buildspace/install-rtems
make all
make install

cd ~/workspace/examples-v2/hello/hello_world_c
make clean
make
cp ./o-optimize/$NAME.exe ~/workspace

