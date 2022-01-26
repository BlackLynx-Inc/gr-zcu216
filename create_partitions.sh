#!/bin/bash

error() {
  printf '\E[31m'; echo "$@"; printf '\E[0m'
}

# Make sure this script is run as root
if [[ $EUID -ne 0 ]]; then
    error "ERROR: this script must be run as root."
    exit 1
fi

# Make sure the device argument is passed in
if [ "$#" -lt 1 ]; then
    error "ERROR: please specify the device (e.g. sdc) to partition"
    exit 1
fi

# Check if passed device exists as a block device
DEVICE_FILE=/dev/$1
if [[ -b "$DEVICE_FILE" ]]; then
    echo -e "Found $DEVICE_FILE block device\n"
else
    error "ERROR: specified device: $DEVICE_FILE not found!"
    exit 1
fi

# Print disk information and ask for confirmation
parted -s $DEVICE_FILE print
echo
echo
echo -n "Proceed with partitioning this device (y/n)? "
read answer
if [ "$answer" != "${answer#[Yy]}" ] ;then
    echo 
else
    error Abort
    exit 1
fi

# Partition disk
#  part 1 - 64 MB
#  part 2 - rest of space
parted -s -a optimal /dev/sdc -- mklabel msdos mkpart primary 0 64 mkpart primary 64 100%
echo
echo
sleep 1

# Format partition 1 as FAT32 labeled "BOOT"
mkfs.fat -F 32 -n "BOOT" ${DEVICE_FILE}1
echo
echo

# Format partition 2 as ext4 labeled "rootfs"
mkfs.ext4 -q -L "rootfs" ${DEVICE_FILE}2

echo "Complete"
