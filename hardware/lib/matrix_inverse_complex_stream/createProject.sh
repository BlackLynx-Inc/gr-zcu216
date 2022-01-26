#!/bin/bash
################################################################################
###                   PROPRIETARY NOTICE
################################################################################
### (c) Copyright 2019 Blacklynx Inc. All rights reserved.
###
### This file contains Blacklynx Inc. proprietary information. It is the
### property of Blacklynx Inc. and shall not be used, disclosed to others or
### reproduced without the express written consent of Blacklynx Inc.,
### including, but without limitation, it is not to be used in the creation,
### manufacture, development, or derivation of any designs, or configuration.
###
################################################################################
###
### Company:     Blacklynx Inc.
###
### File Name: createProject.sh
### Description: Generate/Clean HLS prj_mgs_inverse_complex Project.
###
###
################################################################################

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

# Specify and export a name for the project
export TOP_NAME=mgs_inverse_complex
export PROJECT_NAME=prj_mgs_inverse_complex

# Select which version of Xilinx Vivado to use
#VIVADO_VER=$XILINX_2020_2_Vitis_HOME
#VIVADO_VER=$XILINX_2020_1_Vitis_HOME
VIVADO_VER=$XILINX_2021_1_Vitis_HOME
#VIVADO_VER=$XILINX_2018_3_SDx_HOME

#if [[ "$VIVADO_VER" = "$XILINX_2020_2_Vitis_HOME" ]]; then
if [[ "$VIVADO_VER" = "$XILINX_2021_1_Vitis_HOME" ]]; then
   CMD="vitis_hls -f"
   OPEN_CMD="vitis_hls -p $PROJECT_NAME"
else
   CMD="vivado_hls -f"
   OPEN_CMD="vivado_hls -p $PROJECT_NAME"
fi

case "$1" in
   help)
      printf "%b" "${GREEN}Syntax: ./createProject.sh [help] | [clean] | [build] | [csim] | [cosim] | [impl] | [open]${NORMAL}\n"
   ;;
   clean)
      printf "%b" "${YELLOW}Removing $PROJECT_NAME${NORMAL}\n"
      rm -rf $PROJECT_NAME
      rm -rf *.log
   ;;
   build)
      if [ ! -d $PROJECT_NAME ]; then
      printf "%b" "${GREEN}Creating new HLS project ($PROJECT_NAME)${NORMAL}\n"
         source $VIVADO_VER/settings64.sh
         $CMD build.tcl
         # Change the IP from Pre-Production to Production
         find . -type f -name "component.xml" -print0 | xargs -0 sed -i 's/lifeCycle="Pre-Production"/lifeCycle="Production"/g'
      else
         printf "%b" "${YELLOW}HLS project ($PROJECT_NAME) already exits run clean then build${NORMAL}\n"
      fi
   ;;
   csim)
      if [ -d $PROJECT_NAME ]; then
         printf "%b" "${GREEN}Running Csim for HLS project ($PROJECT_NAME)${NORMAL}\n"
         source $VIVADO_VER/settings64.sh
         $CMD csim.tcl
      else
         printf "%b" "${YELLOW}HLS project ($PROJECT_NAME) does not exist run ./createProject.sh build to create a new project${NORMAL}\n"
      fi
   ;;
   cosim)
      if [ -d $PROJECT_NAME ]; then
         printf "%b" "${GREEN}Running RTL/Cosim for HLS project ($PROJECT_NAME)${NORMAL}\n"
         source $VIVADO_VER/settings64.sh
         $CMD cosim.tcl
      else
         printf "%b" "${YELLOW}HLS project ($PROJECT_NAME) does not exist run ./createProject.sh build to create a new project${NORMAL}\n"
      fi
   ;;
   impl)
      if [ -d $PROJECT_NAME ]; then
         printf "%b" "${GREEN}Implementing HLS project ($PROJECT_NAME)${NORMAL}\n"
         source $VIVADO_VER/settings64.sh
         $CMD impl.tcl
         # Change the IP from Pre-Production to Production
         find . -type f -name "component.xml" -print0 | xargs -0 sed -i 's/lifeCycle="Pre-Production"/lifeCycle="Production"/g'
      else
         printf "%b" "${YELLOW}HLS project ($PROJECT_NAME) does not exist run ./createProject.sh build to create a new project${NORMAL}\n"
      fi
   ;;
   open)
      printf "%b" "${GREEN}Opening HLS project ($PROJECT_NAME)${NORMAL}\n"
      source $VIVADO_VER/settings64.sh
      $OPEN_CMD </dev/null &>/dev/null &
   ;;
   *)
      printf "%b" "${RED}SYNTAX ERROR ./createProject.sh $1${NORMAL}\n"
      printf "%b" "${RED}Syntax: ./createProject.sh [help] | [clean] | [build] | [csim] | [cosim] | [impl] | [open]${NORMAL}\n"
   ;;
esac
