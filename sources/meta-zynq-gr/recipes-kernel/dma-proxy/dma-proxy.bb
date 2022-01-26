SUMMARY = "Recipe for  build an external dma-proxy Linux kernel module"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.
inherit module

SRC_URI = " \
	file://Makefile \
        file://dma-proxy.c \
        file://dma-proxy.h \
	file://COPYING \
"

S = "${WORKDIR}"

KERNEL_MODULE_AUTOLOAD += "dma-proxy"
