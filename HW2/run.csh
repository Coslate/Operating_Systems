#!/bin/csh -f

#clean kernel log buffer
sudo dmesg -C

#clean compiled hw1.ko
make clean

#compile hw1.ko
make program=$1

#load the kernel modules: hw1
sudo insmod $1.ko

#remove the kernel modules: hw1
sudo rmmod $1.ko

#check the kernel log buffer
dmesg > kernel.log
