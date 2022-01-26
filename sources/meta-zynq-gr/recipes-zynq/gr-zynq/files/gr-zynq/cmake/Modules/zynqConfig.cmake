if(NOT PKG_CONFIG_FOUND)
    INCLUDE(FindPkgConfig)
endif()
PKG_CHECK_MODULES(PC_ZYNQ zynq)

FIND_PATH(
    ZYNQ_INCLUDE_DIRS
    NAMES zynq/api.h
    HINTS $ENV{ZYNQ_DIR}/include
        ${PC_ZYNQ_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    ZYNQ_LIBRARIES
    NAMES gnuradio-zynq
    HINTS $ENV{ZYNQ_DIR}/lib
        ${PC_ZYNQ_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/zynqTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ZYNQ DEFAULT_MSG ZYNQ_LIBRARIES ZYNQ_INCLUDE_DIRS)
MARK_AS_ADVANCED(ZYNQ_LIBRARIES ZYNQ_INCLUDE_DIRS)
