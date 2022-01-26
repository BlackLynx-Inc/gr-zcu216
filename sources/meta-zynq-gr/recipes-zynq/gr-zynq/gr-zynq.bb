SUMMARY = "gr-zynq GNU Radio OOT module"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
	file://gr-zynq/ \
"

DEPENDS += "gnuradio dma-proxy-lib python3-pybind11-native "

inherit setuptools3 cmake

export BUILD_SYS
export HOST_SYS="${MULTIMACH_TARGET_SYS}"

FILES_${PN} += "${datadir}/gnuradio/gr-zynq \
		${datadir}/gnuradio/grc/blocks/ \
                ${PYTHON_SITEPACKAGES_DIR}/gnuradio/gr-zynq"


S = "${WORKDIR}/gr-zynq"

EXTRA_OECMAKE += "-DGnuradio_DIR=/lib/cmake/gnuradio"

