SUMMARY = "rfdc Library"
SECTION = "rfdc"
LICENSE = "BSD"

inherit pkgconfig xlnx-embeddedsw

COMPATIBLE_MACHINE = "^$"
COMPATIBLE_MACHINE_zynqmp-dr = "zynqmp-dr"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

S = "${WORKDIR}/git"

PACKAGE_ARCH = "${SOC_VARIANT_ARCH}"

DEPENDS = "libmetal"

PROVIDES = "rfdc"

RFDC_SUBDIR = "XilinxProcessorIPLib/drivers/rfdc/src"

do_compile_prepend() {
    cd ${S}/${RFDC_SUBDIR}
    cp Makefile.Linux Makefile
}

do_install() {
    install -d ${D}${libdir}
    install -d ${D}${includedir}

    cd ${S}/${RFDC_SUBDIR}
    oe_libinstall -so librfdc ${D}${libdir}
    install -m 0644 xrfdc_hw.h ${D}${includedir}/xrfdc_hw.h
    install -m 0644 xrfdc.h ${D}${includedir}/xrfdc.h
}

FILES_${PN} = "${libdir}/*.so.*"
FILES_${PN}-dev = "${libdir}/*.so  ${includedir}/*"
