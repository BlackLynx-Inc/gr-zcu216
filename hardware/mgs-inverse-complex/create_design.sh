#!/bin/bash
#########################################################################################
###
### Company:     Blacklynx Inc.
###
### File Name: create_design.sh
### Description: GNU Radio FPGA project maintenace utility script.
###   Options:
###     help      - Prints the command syntax.
###     clean     - Deletes the current project
###     update    - The update option copies the source files from the specified
###                 path to the project creation sources ./xilinx-zcu106-2021.1.srcs
###                 A second option can be specified with update, it is the path to the
###                 sources used for updating. If a second option is not specified with
###                 update, ${source_path} is used
###     no option - Generates a Vivado project from ./xilinx-zcu106-2021.1.srcs
###
###
#########################################################################################

BLACK=$(tput setaf 0)
RED=$(tput setaf 1)
GREEN=$(tput setaf 2)
YELLOW=$(tput setaf 3)
LIME_YELLOW=$(tput setaf 190)
POWDER_BLUE=$(tput setaf 153)
BLUE=$(tput setaf 4)
MAGENTA=$(tput setaf 5)
CYAN=$(tput setaf 6)
WHITE=$(tput setaf 7)
BRIGHT=$(tput bold)
NORMAL=$(tput sgr0)
BLINK=$(tput blink)
REVERSE=$(tput smso)
UNDERLINE=$(tput smul)

scriptdir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" > /dev/null && pwd )"

source_path=${2:-./gnuradio-zcu106-2021.1/xilinx-zcu106-2021.1.srcs}
strategy=${2:-0}

# Select which version of Xilinx Vivado to use
VIVADO_VER=$XILINX_2021_1_Vitis_HOME

# Build with Performance_ExtraTimingOpt implementation stategy.
CMD_DEFAULT="vivado -mode batch -source default.tcl"

# Build with Performance_ExtraTimingOpt implementation stategy.
CMD_EXTRATIMINGOPT="vivado -mode batch -source ExtraTimingOpt.tcl"

# Build with Performance_ExploreWithRemap implementation stategy.
CMD_EXPLOREREMAP="vivado -mode batch -source ExploreWithRemap.tcl"

case "$1" in
   help)
     printf "%b" "${GREEN}Syntax: ./create_design.sh [clean|help|update [path/to/updated/xilinx-zcu106-2021.1.srcs]|build [0(Vivado Default)|1(ExtraTimingOpt)|2(ExploreWithRemap)]]${NORMAL}\n"
     printf "\n"
     printf "%b" "${GREEN}The update option copies the source files from the specified path to the project creation sources ./xilinx-zcu106-2021.1.srcs${NORMAL}\n"
     printf "%b" "${GREEN}A second option can be specified with update, it is the path to the sources used for updating.${NORMAL}\n"
     printf "%b" "${GREEN}If a second option is not specified with update, ${source_path} is used.${NORMAL}\n"
     printf "\n"
     printf "%b" "${GREEN}The build option allows the selection of a implementation strategy. 0 for Vivado Defaults, 1 for ExtraTimingOpt(default) or 2 for ExploreWithRemap${NORMAL}\n"
     printf "%b" "${GREEN}If a second option is not specified with build, ExtraTimingOpt is used.${NORMAL}\n"
     printf "%b" "${GREEN}Example: ./create_design.sh build 1${NORMAL}\n"
     printf "\n"
     printf "%b" "${GREEN}If no option is given a project is built with the Vivado default implementation strategy.${NORMAL}\n"
     printf "%b" "${GREEN}Example: ./create_design.sh${NORMAL}\n"

   ;;
   clean)
      read -p "${YELLOW}The current project will be deleted. Contine? [yes/no]${NORMAL}"
      if [ "$REPLY" = "yes" ]; then
         rm -f *.log
         rm -f *.jou
         rm -rf .Xil
         rm -rf .cache
         rm -rf gnuradio-zcu106-2021.1
      else
         printf "${GREEN}The project has been preserved.${NORMAL}\n"
      fi
   ;;
   update)
      read -p "${YELLOW}./xilinx-zcu106-2021.1.srcs will be overwritten with ${source_path}. Continue? [yes/no]${NORMAL}"
      if [ "$REPLY" = "yes" ]; then
         cp -rf ${source_path}/sources_1/imports/hdl/project_1_wrapper.v ./xilinx-zcu106-2021.1.srcs/sources_1/imports/hdl/project_1_wrapper.v
         cp -rf ${source_path}/sources_1/bd/project_1/project_1.bd ./xilinx-zcu106-2021.1.srcs/sources_1/bd/project_1/project_1.bd
         cp -rf ${source_path}/constrs_1/imports/xdc/default.xdc ./xilinx-zcu106-2021.1.srcs/constrs_1/imports/xdc/default.xdc
         cp -rf ${source_path}/constrs_1/imports/xdc/vcu_uc2_pll.xdc ./xilinx-zcu106-2021.1.srcs/constrs_1/imports/xdc/vcu_uc2_pll.xdc
      else
         printf "${GREEN}The project generation source files ./xilinx-zcu106-2021.1.srcs preserved.${NORMAL}\n"
      fi
   ;;
   build)
      if [ ! -f "${scriptdir}/../lib/matrix_inverse_complex_stream/prj_mgs_inverse_complex/solution1/impl/ip/component.xml" ]; then
         printf "%b" "${GREEN}Building Complex Floating Point Matrix inverse IP.${NORMAL}\n"
         cd ${scriptdir}/../lib/matrix_inverse_complex_stream
         ./createProject.sh build
         cd ${scriptdir}
      fi

      if [ "$2" -eq "2" ]; then
         printf "%b" "${GREEN}Generating GNU Radio ZCU106 FPGA project using Xilinx Tools path ${VIVADO_VER}. Implemetation Strategy: ExtraTimingOpt.${NORMAL}\n"
         source $VIVADO_VER/settings64.sh
         $CMD_EXPLOREREMAP
         printf "%b" "${GREEN}Done Generating GNU Radio ZCU106 FPGA project.${NORMAL}\n"
      elif [ "$2" -eq "1" ]; then
         printf "%b" "${GREEN}Generating GNU Radio ZCU106 FPGA project using Xilinx Tools path ${VIVADO_VER}. Implemetation Strategy: ExploreWithRemap.${NORMAL}\n"
         source $VIVADO_VER/settings64.sh
         $CMD_EXTRATIMINGOPT
         printf "%b" "${GREEN}Done Generating GNU Radio ZCU106 FPGA project.${NORMAL}\n"
      else
         printf "%b" "${GREEN}Generating GNU Radio ZCU106 FPGA project using Xilinx Tools path ${VIVADO_VER}. Implemetation Strategy: Vivado Defaults.${NORMAL}\n"
         source $VIVADO_VER/settings64.sh
         $CMD_DEFAULT
         printf "%b" "${GREEN}Done Generating GNU Radio ZCU106 FPGA project.${NORMAL}\n"
      fi
   ;;
   *)
      if [ ! -f "${scriptdir}/../lib/matrix_inverse_complex_stream/prj_mgs_inverse_complex/solution1/impl/ip/component.xml" ]; then
         printf "%b" "${GREEN}Building Complex Floating Point Matrix inverse IP.${NORMAL}\n"
         cd ${scriptdir}/../lib/matrix_inverse_complex_stream
         ./createProject.sh build
         cd ${scriptdir}
      fi

      printf "%b" "${GREEN}Generating GNU Radio ZCU106 FPGA project using Xilinx Tools path ${VIVADO_VER}. Implemetation Strategy: Vivado Defaults.${NORMAL}\n"
      source $VIVADO_VER/settings64.sh
      $CMD_DEFAULT
      printf "%b" "${GREEN}Done Generating GNU Radio ZCU106 FPGA project.${NORMAL}\n"
   ;;
esac
