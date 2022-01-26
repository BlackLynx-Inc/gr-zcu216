PARALLEL_MAKE = "-j 1"

FILESEXTRAPATHS_prepend := "${THISDIR}/qtbase:"

SRC_URI_append = " \
    file://0002-egl_kms-Modify-the-default-color-format-to-RGB565.patch \
    file://0003-qkmsdevice.cpp-Disable-hw-cursor-as-a-default-option.patch \
"

PACKAGECONFIG_append = " \
  examples accessibility tools libinput fontconfig \
  ${@bb.utils.contains('DISTRO_FEATURES', 'fbdev', 'linuxfb gles2 eglfs', '', d)} \
  ${@bb.utils.contains('DISTRO_FEATURES', 'x11', 'gles2 eglfs', '', d)} \
  ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'gles2 eglfs kms gbm', '', d)} \
  "

PACKAGECONFIG_remove = "tests"

# OpenGL comes from libmali on ev/eg, when egl is enabled
DEPENDS_append_mali400 = " libmali-xlnx"

PACKAGE_ARCH_mali400 = "${SOC_VARIANT_ARCH}"

