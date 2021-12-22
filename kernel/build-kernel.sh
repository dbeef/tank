#!/bin/sh -e

# TODO: Cleanup
# https://www.raspberrypi.com/documentation/computers/linux_kernel.html

git clone --depth=1 https://github.com/raspberrypi/linux
cp raspbian-kernel-config linux/.config
cd linux

# For generating default .config:
# cd linux
# KERNEL=kernel
# make bcmrpi_defconfig

# Deps:
# sudo apt install git bc bison flex libssl-dev make libc6-dev libncurses5-dev
# sudo apt install crossbuild-essential-armhf
KERNEL=kernel
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage modules dtbs

mkdir mnt
mkdir mnt/fat32
mkdir mnt/ext4
# lsblk to check
sudo mount /dev/sdd1 mnt/fat32
sudo mount /dev/sdd2 mnt/ext4

sudo env PATH=$PATH make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MOD_PATH=mnt/ext4 modules_install

sudo cp mnt/fat32/$KERNEL.img mnt/fat32/$KERNEL-backup.img
sudo cp arch/arm/boot/zImage mnt/fat32/$KERNEL.img
sudo cp arch/arm/boot/dts/*.dtb mnt/fat32/
sudo cp arch/arm/boot/dts/overlays/*.dtb* mnt/fat32/overlays/
sudo cp arch/arm/boot/dts/overlays/README mnt/fat32/overlays/
sudo umount mnt/fat32
sudo umount mnt/ext4

