Create a Vivado 2020.1 Embedded GNU Radio FPGA project for the Xilinx ZCU106 card.

1) Export the XILINX_2020_1_SDx_HOME environment variable to point to the Vivado install

   Examples:
   $ export XILINX_2020_1_SDx_HOME=/opt/Xilinx/SDx/2020.1
   or
   $ export XILINX_2020_1_SDx_HOME=/opt/Xilinx/Vivado/2020.1
  
2) Run the create_design.sh script
   $ ./create_design.sh

   NOTE: IP warnings like the following are normal since the IP will be generated:
         WARNING: [BD 41-2576] File 'xyz.xci' referenced by design 'xxxx' could not be found.

3) cd gnuradio-zcu106-2020.1 and open the design
   $ source $XILINX_2020_2_SDx_HOME/settings64.sh
   $ cd gnuradio-zcu106-2020.1
   $ vivado xilinx-zcu106-2020.1.xpr &

4) Run Generate Block Diagram then Run Implementation.

5) To clean design (deletes project)
   $ ./create_design.sh clean