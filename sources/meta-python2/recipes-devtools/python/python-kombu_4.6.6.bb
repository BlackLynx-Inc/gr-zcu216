SUMMARY = "The aim of Kombu is to make messaging in Python as easy as possible by providing \
an idiomatic high-level interface for the AMQ protocol, and also provide proven and tested \
solutions to common messaging problems."

DESCRIPTION = "A messaging framework for Python"
BUGTRACKER = "http://github.com/celery/kombu/issues/"
HOMEPAGE = "http://kombu.readthedocs.org"
SECTION = "devel/python"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://LICENSE;md5=daea7c168428449fbee05e644df929f4"

SRC_URI[md5sum] = "5f751ea817b1f6a4293d4e3a01040d4f"
SRC_URI[sha256sum] = "1760b54b1d15a547c9a26d3598a1c8cdaf2436386ac1f5561934bc8a3cbbbd86"

inherit setuptools pypi

PYPI_PACKAGE = "kombu"

FILES_${PN}-doc += "${datadir}/${SRCNAME}"

RDEPENDS_${PN} = " \
        python-amqp \
        "
