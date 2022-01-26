# gr-zcu106 Read Me

Follow the steps below to create a Yocto Linux image for Xilinx ZCU106 platform with GNU radio built in the filesystem using OE meta-sdr layer

## Build Instructions

1. Clone repo:
```bash
    $ git clone https://github.com/BlackLynx-Inc/gr-zcu106.git
```
2. Go to top-level directory:
```bash
    $ cd gr-zcu106
```
3. Source setup script:
```bash
    $ source setupsdk
```
4. Build image:
```bash
    $ bitbake petalinux-image-gr
```
This will kick off the build which should complete with 0 errors.

## Deploy Instructions

1. Attach SD card to host system. Determine SD card device name (e.g. `sdc` for `/dev/sdc`) using `lsblk` or similar method.
2. **NOTE: this step only needs to be performed once.** Go to top-level directory and run the `create_partitions.sh` script to reformat the repartition the SD card. **WARNING: this is a destructive operation, all data on the SD card will be lost.**
```bash
    $ cd gr-zcu106
    $ sudo ./create_partitions.sh sdc
```
3. Run the `copy_files.sh` script to copy the necessary files to the SD card:
```bash    
    $ sudo ./copy_files.sh sdc
```
4. Insert SD card into ZCU106 board's SD card slot and power on the board.
5. The board should boot to a root shell prompt. Attach serial teminal to `/dev/ttyUSB0` to view.

## Legacy Deploy Instructions 
**NOTE: these are legacy instructions are for reference**

* Go to zcu-106-build/images/zcu106mp/deploy
* Copy the following files to the SD card
* Boot.bin (Rename from TBD)
* Image (Rename from TBD)
* rootfs.cpio.gz.u-boot (Rename from TBD)
* system.dtb (TBD)
* Insert SD card in ZC106 card
* Power up ZCU106 card.
* Once board is up and running, Halt at u-boot and run below command. 
```
ZynqMP> mmc dev 0 && mmcinfo && load mmc 0:1 0x80000 Image && load mmc 0:1 0x4000000 system.dtb && load mmc 0:1 0x6000000 rootfs.cpio.gz.u-boot && booti 0x80000 0x6000000 0x4000000
```
* Board should boot correctly after this.
