# For a Xen-enabled distro on the Raspberry Pi, override the contents of cmdline.txt
# with Xen-on-ARM-specific command line options

DEFAULT_CMDLINE := "${CMDLINE}"
XEN_LINUX_CMDLINE ?= "console=hvc0 clk_ignore_unused earlycon=xenboot debug root=/dev/mmcblk0p2 rootwait"
CMDLINE = "${@bb.utils.contains('DISTRO_FEATURES', 'xen', '${XEN_LINUX_CMDLINE}', '${DEFAULT_CMDLINE}', d)}"

KBRANCH_raspberrypi4-64 = "standard/bcm-2xxx-rpi"
KMACHINE_raspberrypi4-64 = "bcm-2xxx-rpi4"
COMPATIBLE_MACHINE_raspberrypi4-64 = "(raspberrypi4-64)"
