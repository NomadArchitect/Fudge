#!/bin/sh

rm -f fudge.img
rm -f fudge.iso
rm -f root/boot/kernel
rm -f root/boot/initrd
rm -f ../kernel/*.o
rm -f ../arch/arm/kernel/*.o
rm -f ../arch/arm/lib/*.o
rm -f ../arch/x86/kernel/*.o
rm -f ../arch/x86/lib/*.o
rm -f ../lib/*.o
rm -f ../ramdisk/*.o
rm -f ../ramdisk/cat
rm -f ../ramdisk/clear
rm -f ../ramdisk/cpu
rm -f ../ramdisk/date
rm -f ../ramdisk/echo
rm -f ../ramdisk/elf
rm -f ../ramdisk/hello
rm -f ../ramdisk/ls
rm -f ../ramdisk/reboot
rm -f ../ramdisk/shell
rm -f ../ramdisk/timer
rm -f ../tools/mkinitrd

