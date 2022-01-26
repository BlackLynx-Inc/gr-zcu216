SUMMARY = "Simple dma-proxy-test application"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
	file://dma-proxy-reg.c \
	file://dma-proxy-util.c \
	file://dma-proxy-util-nb.c \
	file://dma-proxy-util-file.c \
	file://Makefile \
"

S = "${WORKDIR}"

DEPENDS = "dma-proxy dma-proxy-lib"

CFLAGS += "-pthread"
LDFLAGS += "-lpthread -ldmaproxy"

EXTRA_OEMAKE = "'CFLAGS=${CFLAGS}' 'LDFLAGS=${LDFLAGS}'"

do_compile() {
	     oe_runmake
}

do_install() {
	     install -d ${D}${bindir}
	     install -m 0755 dma-proxy-reg ${D}${bindir}
	     install -m 0755 dma-proxy-util ${D}${bindir}
	     install -m 0755 dma-proxy-util-nb ${D}${bindir}
	     install -m 0755 dma-proxy-util-file ${D}${bindir}
}
