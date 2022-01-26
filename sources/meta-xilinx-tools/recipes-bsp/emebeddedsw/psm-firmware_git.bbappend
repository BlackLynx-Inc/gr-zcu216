# Should not need any external patches
SRC_URI = "${EMBEDDEDSW_SRCURI}"

# We WANT to default to this version when available
DEFAULT_PREFERENCE = "100"

inherit xsctapp xsctyaml

B = "${S}/${XSCTH_PROJ}"

XSCTH_PROC_IP = "psv_psm"
XSCTH_APP  = "versal PSM Firmware"

# XSCT version provides it's own toolchain, so can build in any environment
COMPATIBLE_HOST_versal = "${HOST_SYS}"

# Clear this for a Linux build, using the XSCT toolchain
EXTRA_OEMAKE_linux = ""

# Workaround for hardcoded toolchain items
XSCT_PATH_ADD_append_elf = "\
${WORKDIR}/bin:"

MB_OBJCOPY = "mb-objcopy"

do_compile_prepend_elf() {
  mkdir -p ${WORKDIR}/bin
  echo "#! /bin/bash\n${CC} \$@" > ${WORKDIR}/bin/mb-gcc
  echo "#! /bin/bash\n${AS} \$@" > ${WORKDIR}/bin/mb-as
  echo "#! /bin/bash\n${AR} \$@" > ${WORKDIR}/bin/mb-ar
  echo "#! /bin/bash\n${AR} \$@" > ${WORKDIR}/bin/mb-gcc-ar
  echo "#! /bin/bash\n${OBJCOPY} \$@" > ${WORKDIR}/bin/mb-objcopy
  chmod 0755 ${WORKDIR}/bin/mb-gcc
  chmod 0755 ${WORKDIR}/bin/mb-as
  chmod 0755 ${WORKDIR}/bin/mb-ar
  chmod 0755 ${WORKDIR}/bin/mb-gcc-ar
  chmod 0755 ${WORKDIR}/bin/mb-objcopy
}

do_compile_append() {
  ${MB_OBJCOPY} -O binary ${B}/${XSCTH_PROJ}/executable.elf ${B}/${XSCTH_PROJ}/executable.bin
}

# xsctapp sets it's own do_install, replace it with the real one
do_install() {
    :
}

do_deploy() {
    install -Dm 0644 ${B}/${XSCTH_PROJ}/executable.elf ${DEPLOYDIR}/${PSM_FIRMWARE_BASE_NAME}.elf
    ln -sf ${PSM_FIRMWARE_BASE_NAME}.elf ${DEPLOYDIR}/${PSM_FIRMWARE_IMAGE_NAME}.elf
    install -m 0644 ${B}/${XSCTH_PROJ}/executable.bin ${DEPLOYDIR}/${PSM_FIRMWARE_BASE_NAME}.bin
    ln -sf ${PSM_FIRMWARE_BASE_NAME}.bin ${DEPLOYDIR}/${PSM_FIRMWARE_IMAGE_NAME}.bin
}
