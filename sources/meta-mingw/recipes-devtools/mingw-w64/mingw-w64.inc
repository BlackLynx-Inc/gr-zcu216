LICENSE = "ZPL-2.1"
LIC_FILES_CHKSUM = "file://${WORKDIR}/mingw-w64-v${PV}/COPYING;md5=bb936f0e04d8f1e19ad545100cee9654"

COMPATIBLE_HOST = ".*-mingw.*"

SRC_URI = "${SOURCEFORGE_MIRROR}/project/mingw-w64/mingw-w64/mingw-w64-release/mingw-w64-v${PV}.tar.bz2"

SRC_URI[sha256sum] = "aa20dfff3596f08a7f427aab74315a6cb80c2b086b4a107ed35af02f9496b628"

UPSTREAM_CHECK_URI = "http://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/"
UPSTREAM_CHECK_REGEX = "mingw-w64-v(?P<pver>(\d+[\.\-_]*)+)\.tar"


