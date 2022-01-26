#!/bin/bash

# Default base mount point path that will be created and used if needed
MOUNT_POINT_BASE=/mnt/sdcard
FILE_BASE=build/tmp/deploy/images/zcu106-zynqmp/

BOOT_FILES="boot.scr boot.bin system.dtb Image"
ROOTFS_FILE="petalinux-image-gr-zcu106-zynqmp.tar.gz"

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

# Check that the file base path exists
if [[ -d "$FILE_BASE" ]]; then
    :
else
    error "ERROR: file base path: $FILE_BASE does not exist"
    exit 1
fi

# Use the handy "findmnt" to find the mount point for partition 1
PART1_MPOINT=$(findmnt -nr -o target -S ${DEVICE_FILE}1)
if [[ -z "$PART1_MPOINT" ]]; then
    # Partition 1 was not mounted, so mount it manually
    PART1_MPOINT=${MOUNT_POINT_BASE}/BOOT
    echo "Partition 1 (${DEVICE_FILE}1) not mounted, creating mount point: ${PART1_MPOINT}"
    mkdir -p ${PART1_MPOINT}
    mount ${DEVICE_FILE}1 ${PART1_MPOINT}
    if [[ $? -ne 0 ]]; then
        error "ERROR: unable to mount ${DEVICE_FILE}1"
        exit 1
    fi
fi

echo "Partition 1 mount point: $PART1_MPOINT"

# Do the same for partition 2
PART2_MPOINT=$(findmnt -nr -o target -S ${DEVICE_FILE}2)
if [[ -z "$PART2_MPOINT" ]]; then
    # Partition 2 was not mounted, so mount it manually
    PART2_MPOINT=${MOUNT_POINT_BASE}/rootfs
    echo "Partition 2 (${DEVICE_FILE}2) not mounted, creating mount point: ${PART2_MPOINT}"
    mkdir -p ${PART2_MPOINT}
    mount ${DEVICE_FILE}2 ${PART2_MPOINT}
    if [[ $? -ne 0 ]]; then
        error "ERROR: unable to mount ${DEVICE_FILE}2"
        exit 1
    fi
fi

echo "Partition 2 mount point: $PART2_MPOINT"
echo
echo


# Remove existing files from partition 1
echo "Cleaning partition 1"
rm -rf ${PART1_MPOINT}/*

# Copy files onto partition 1
echo "Copying new files to partition 1"
for file in $BOOT_FILES; do
    cp ${FILE_BASE}/${file} ${PART1_MPOINT}/
done

echo
echo

# Remove existing files from partition 2
echo "Cleaning partition 2"
rm -rf ${PART2_MPOINT}/*

# Untar files to partition 2
echo "Extracting rootfs to partition 2"
tar xf ${FILE_BASE}/${ROOTFS_FILE} -C ${PART2_MPOINT}/ 
echo
echo

# Finally, unmount the partitions
echo "Unmounting partitions"
umount ${PART1_MPOINT} 
umount ${PART2_MPOINT} 

echo
echo "Complete"
