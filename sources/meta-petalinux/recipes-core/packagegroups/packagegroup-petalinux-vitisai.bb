DESCRIPTION = "PetaLinux Vitis AI packages"

inherit packagegroup

# Since dnndk is SOC_FAMILY specific, this package must be also
PACKAGE_ARCH = "${SOC_FAMILY_ARCH}"

COMPATIBLE_MACHINE = "^$"
COMPATIBLE_MACHINE_zynqmp = ".*"

RDEPENDS_${PN} = "\
    glog \
    googletest \
    json-c \
    protobuf \
    python3-pip \
    opencv \
    dnndk \
    python3-pybind11 \
    vitis-ai-library \
    xir \
    target-factory \
    vart \
    unilog \
    "

RDEPENDS_${PN}-dev += "\
    protobuf-c \
    libeigen-dev \
    "
