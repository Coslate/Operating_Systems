#!/bin/csh -f

#clean kernel log buffer
sudo dmesg -C

#clean compiled hw1.ko
make clean

#compile hw1.ko
make

#load the kernel modules: hw1
sudo insmod hw1.ko

#remove the kernel modules: hw1
sudo rmmod hw1.ko

#check the kernel log buffer
dmesg > kernel.log
