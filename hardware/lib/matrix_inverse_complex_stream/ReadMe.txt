Run the bash createProject.sh script:

1) Print script syntax help
   Example:
      $ ./createProject.sh help
      Syntax: ./createProject.sh [help] | [clean] | [build] | [csim] | [cosim] | [impl] | [open]
2) "./createProject.sh clean" cleans the HLS project. All project files are removed
   Example:
      $ ./createProject.sh clean
      Removing prj_mgs_inverse_complex
3) "./createProject.sh build" builds the HLS project. This performs the following functions:
   a.  Creates Vivado HLS project
   b.  Runs C simulation
   c.  Synthesizes C code into RTL
   d.  Sets up a co-simulation of C testbench and compiled RTL(does not run the simulation)
   e.  Packages design into IP
   Example:
      $ ./createProject.sh build
      Building HLS project (prj_mgs_inverse_complex)
4) "./createProject.sh csim" runs the HLS project C simulation.
5) "./createProject.sh cosim" runs the HLS project C/RTL co-simulation.
6) "./createProject.sh impl" runs an OOC build of the top level function.
7) "./createProject.sh open" opens the HLS project in the GUI.

Directory structure:

./ (this directory)
  script.tcl is a tcl script for executing all of the Vivado HLS commands required to build, simulate and IP package the design.
  createProject.sh is a helper bash script for building, opening and cleaning the HLS project.

./include/
  contains header files for .cpp source files.  
  The constants.h file in this folder takes precedence over any others that are located 
  in lower level modules in the hierarchy.

./src/
  contains the .cpp source code for the design.

./test/
  contains the _tb.cpp test source code for the simulation, used for both C simulation 
  and co-simulation.
