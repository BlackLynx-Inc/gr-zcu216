SUMMARY = "Firmware for VCU"
DESCRIPTION = "Firmware binaries provider for VCU"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://LICENSE;md5=63b45903a9a50120df488435f03cf498"

XILINX_VCU_VERSION = "1.0.0"
PV = "${XILINX_VCU_VERSION}-xilinx-${XILINX_RELEASE_VERSION}+git${SRCPV}"

S  = "${WORKDIR}/git"

BRANCH ?= "xlnx_rel_v2021.1"
REPO ?= "git://github.com/Xilinx/vcu-firmware.git;protocol=https"
SRCREV = "73780a2f55a4e9c10de525cdb0335fbb62364217"

BRANCHARG = "${@['nobranch=1', 'branch=${BRANCH}'][d.getVar('BRANCH', True) != '']}"
SRC_URI   = "${REPO};${BRANCHARG}"

COMPATIBLE_MACHINE = "^$"
COMPATIBLE_MACHINE_zynqmp = "zynqmp"

PACKAGE_ARCH = "${SOC_FAMILY_ARCH}"

do_install() {
    install -Dm 0644 ${S}/${XILINX_VCU_VERSION}/lib/firmware/al5d_b.fw ${D}/lib/firmware/al5d_b.fw
    install -Dm 0644 ${S}/${XILINX_VCU_VERSION}/lib/firmware/al5d.fw ${D}/lib/firmware/al5d.fw
    install -Dm 0644 ${S}/${XILINX_VCU_VERSION}/lib/firmware/al5e_b.fw ${D}/lib/firmware/al5e_b.fw
    install -Dm 0644 ${S}/${XILINX_VCU_VERSION}/lib/firmware/al5e.fw ${D}/lib/firmware/al5e.fw
}

# Inhibit warnings about files being stripped
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_PACKAGE_STRIP = "1"
FILES_${PN} = "/lib/firmware/*"

# These libraries shouldn't get installed in world builds unless something
# explicitly depends upon them.
EXCLUDE_FROM_WORLD = "1"

INSANE_SKIP_${PN} = "ldflags"
