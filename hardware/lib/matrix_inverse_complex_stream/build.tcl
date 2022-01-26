open_project  $::env(PROJECT_NAME)
set_top $::env(TOP_NAME)
add_files src/mgs_inverse_complex.cpp -cflags "-Iinclude"
add_files -tb test/mgs_inverse_complex_tb.cpp -cflags "-Iinclude -Wno-unknown-pragmas"
open_solution "solution1"
set_part {xczu7ev-ffvc1156-2-e}
create_clock -period 4.0 -name default
config_export -description "MGS vectorized matrix inversion" -display_name complex_MGS_inverse -format ip_catalog -library dogfish -rtl verilog -vendor BlackLynx
#config_compile -unsafe_math_optimizations=true

# Can be set to low standard and high. Reference: https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitis_hls_coding_styles.html
#config_op facc -impl auto -precision low
csim_design
csynth_design
cosim_design -setup -trace_level all
export_design -rtl verilog -format ip_catalog -description "MGS vectorized matrix inversion" -vendor "BlackLynx" -library "dogfish" -display_name "complex_MGS_inverse"
exit
