SUMMARY = "Thin userspace library for the dma-proxy driver"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS = "dma-proxy"

SRC_URI = " \
	file://dma-proxy-lib.c \
	file://dma-proxy-lib.h \
	file://Makefile \
"

S = "${WORKDIR}"
TARGET_CC_ARCH += "${LDFLAGS} -fPIC"

do_install() {
    oe_runmake 'DESTDIR=${D}' install
}

FILES_${PN} = "${libdir}/libdmaproxy.so.0.1.0 ${libdir}/libdmaproxy.so.0"
FILES_${PN}-dev = "${includedir}/* ${libdir}/libdmaproxy.so"
