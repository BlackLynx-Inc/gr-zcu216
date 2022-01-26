DESCRIPTION = "Device Tree generation and packaging for BSP Device Trees using DTG from Xilinx"

LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://xadcps/data/xadcps.mdd;md5=f7fa1bfdaf99c7182fc0d8e7fd28e04a"

PROVIDES = "virtual/dtb"

require recipes-bsp/device-tree/device-tree.inc
inherit xsctdt xsctyaml image-artifact-names
BASE_DTS ?= "system-top"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

S = "${WORKDIR}/git"

DT_VERSION_EXTENSION ?= "xilinx-${XILINX_RELEASE_VERSION}"
PV = "${DT_VERSION_EXTENSION}+git${SRCPV}"

XSCTH_BUILD_CONFIG = ""
YAML_COMPILER_FLAGS ?= ""
XSCTH_APP = "device-tree"
XSCTH_MISC = " -hdf_type ${HDF_EXT}"

YAML_MAIN_MEMORY_CONFIG_ultra96 ?= "psu_ddr_0"
YAML_CONSOLE_DEVICE_CONFIG_ultra96 ?= "psu_uart_1"

YAML_MAIN_MEMORY_CONFIG_kc705 ?= "mig_7series_0"
YAML_CONSOLE_DEVICE_CONFIG_kc705 ?= "axi_uartlite_0"

YAML_DT_BOARD_FLAGS_ultra96 ?= "{BOARD avnet-ultra96-rev1}"
YAML_DT_BOARD_FLAGS_zcu102 ?= "{BOARD zcu102-rev1.0}"
YAML_DT_BOARD_FLAGS_zcu106 ?= "{BOARD zcu106-reva}"
YAML_DT_BOARD_FLAGS_zc702 ?= "{BOARD zc702}"
YAML_DT_BOARD_FLAGS_zc706 ?= "{BOARD zc706}"
YAML_DT_BOARD_FLAGS_zedboard ?= "{BOARD zedboard}"
YAML_DT_BOARD_FLAGS_zc1254 ?= "{BOARD zc1254-reva}"
YAML_DT_BOARD_FLAGS_kc705 ?= "{BOARD kc705-full}"
YAML_DT_BOARD_FLAGS_zcu104 ?= "{BOARD zcu104-revc}"
YAML_DT_BOARD_FLAGS_zcu111 ?= "{BOARD zcu111-reva}"
YAML_DT_BOARD_FLAGS_zcu1275 ?= "{BOARD zcu1275-revb}"
YAML_DT_BOARD_FLAGS_zcu1285 ?= "{BOARD zcu1285-reva}"
YAML_DT_BOARD_FLAGS_zcu216 ?= "{BOARD zcu216-reva}"
YAML_DT_BOARD_FLAGS_zcu208 ?= "{BOARD zcu208-reva}"
YAML_DT_BOARD_FLAGS_virt-versal ?= "{BOARD versal-virt}"
YAML_DT_BOARD_FLAGS_vck-sc ?= "{BOARD zynqmp-e-a2197-00-reva}"
YAML_DT_BOARD_FLAGS_v350 ?= "{BOARD versal-v350-reva}"
YAML_DT_BOARD_FLAGS_vck5000 ?= "{BOARD versal-vck5000-reva}"
YAML_DT_BOARD_FLAGS_vck190 ?= "{BOARD versal-vck190-reva-x-ebm-01-reva}"
YAML_DT_BOARD_FLAGS_vmk180 ?= "{BOARD versal-vmk180-reva-x-ebm-01-reva}"
YAML_DT_BOARD_FLAGS_vc-p-a2197-00 ?= "{BOARD versal-vc-p-a2197-00-reva-x-prc-01-reva}"
YAML_DT_BOARD_FLAGS_ac701 ?= "{BOARD ac701-full}"
YAML_DT_BOARD_FLAGS_kc705 ?= "{BOARD kc705-full}"
YAML_DT_BOARD_FLAGS_kcu105 ?= "{BOARD kcu105}"
YAML_DT_BOARD_FLAGS_sp701 ?= "{BOARD sp701-rev1.0}"
YAML_DT_BOARD_FLAGS_vcu118 ?= "{BOARD vcu118-rev2.0}"
YAML_DT_BOARD_FLAGS_k26 ?= "{BOARD zynqmp-sm-k26-revb}"
YAML_DT_BOARD_FLAGS_zcu670 ?= "{BOARD zcu670-reva}"
YAML_DT_BOARD_FLAGS_vpk120 ?= "{BOARD versal-vpk120-reva}"
YAML_DT_BOARD_FLAGS_vpk-sc ?= "{BOARD zynqmp-vpk120-reva}"

YAML_OVERLAY_CUSTOM_DTS = "pl-final.dts"
CUSTOM_PL_INCLUDE_DTSI ?= ""
EXTRA_DT_FILES ?= ""

DT_FILES_PATH = "${XSCTH_WS}/${XSCTH_PROJ}"
DT_INCLUDE_append = " ${WORKDIR}"
DT_PADDING_SIZE = "0x1000"
DTC_FLAGS_append = "${@['', ' -@'][d.getVar('YAML_ENABLE_DT_OVERLAY') == '1']}"

COMPATIBLE_MACHINE_zynq = ".*"
COMPATIBLE_MACHINE_zynqmp = ".*"
COMPATIBLE_MACHINE_microblaze = ".*"
COMPATIBLE_MACHINE_versal = ".*"

SRC_URI_append_ultra96 = "${@bb.utils.contains('MACHINE_FEATURES', 'mipi', ' file://mipi-support-ultra96.dtsi file://pl.dtsi', '', d)}"

SRC_URI_append = "${@" ".join(["file://%s" % f for f in (d.getVar('EXTRA_DT_FILES') or "").split()])}"
do_configure[cleandirs] += "${DT_FILES_PATH} ${B}"
do_deploy[cleandirs] += "${DEPLOYDIR}"

do_configure_append_ultra96() {
        if [ -e ${WORKDIR}/mipi-support-ultra96.dtsi ]; then
               cp ${WORKDIR}/mipi-support-ultra96.dtsi ${DT_FILES_PATH}/mipi-support-ultra96.dtsi
               cp ${WORKDIR}/pl.dtsi ${DT_FILES_PATH}/pl.dtsi
               echo '/include/ "mipi-support-ultra96.dtsi"' >> ${DT_FILES_PATH}/${BASE_DTS}.dts
        fi
}

do_configure_append () {
    if [ -n "${CUSTOM_PL_INCLUDE_DTSI}" ]; then
        [ ! -f "${CUSTOM_PL_INCLUDE_DTSI}" ] && bbfatal "Please check that the correct filepath was provided using CUSTOM_PL_INCLUDE_DTSI"
        cp ${CUSTOM_PL_INCLUDE_DTSI} ${XSCTH_WS}/${XSCTH_PROJ}/pl-custom.dtsi
    fi

    for f in ${EXTRA_DT_FILES}; do
        cp ${WORKDIR}/${f} ${DT_FILES_PATH}/
    done
}


SRC_URI_append_zcu106-zynqmp = "\
    file://system-user.dtsi \
"

do_configure_append_zcu106-zynqmp() {
        if [ -e ${WORKDIR}/system-user.dtsi ]; then
               cp ${WORKDIR}/system-user.dtsi ${DT_FILES_PATH}/system-user.dtsi
               echo '/include/ "system-user.dtsi"' >> ${DT_FILES_PATH}/system-top.dts
        fi
}


do_compile_prepend() {
    listpath = d.getVar("DT_FILES_PATH")
    try:
        os.remove(os.path.join(listpath, "system.dts"))
    except OSError:
        pass
}

BINARY_EXT = ".dtb"
do_install_append () {
    install -Dm 0644 ${B}/${BASE_DTS}.dtb ${D}/boot/${PN}${BINARY_EXT}
}
FILES_${PN} += "/boot/${PN}${BINARY_EXT}"

DTB_BASE_NAME ?= "${MACHINE}-system${IMAGE_VERSION_SUFFIX}"

do_install_append_microblaze () {
    for DTB_FILE in `ls *.dtb`; do
        dtc -I dtb -O dts -o ${D}/boot/devicetree/mb.dts ${B}/${DTB_FILE}
    done
}

do_deploy() {
    #deploy base dtb
    install -Dm 0644 ${B}/${BASE_DTS}.dtb ${DEPLOYDIR}/${DTB_BASE_NAME}.dtb
    ln -sf ${DTB_BASE_NAME}.dtb ${DEPLOYDIR}/${MACHINE}-system.dtb
    ln -sf ${DTB_BASE_NAME}.dtb ${DEPLOYDIR}/system.dtb

    #deploy everything in case
	for DTB_FILE in `ls *.dtb *.dtbo`; do
		install -Dm 0644 ${B}/${DTB_FILE} ${DEPLOYDIR}/
	done
}

FILES_${PN}_append_microblaze = " /boot/devicetree/*.dts"

EXTERNALSRC_SYMLINKS = ""
