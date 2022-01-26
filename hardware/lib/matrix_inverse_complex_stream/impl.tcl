open_project $::env(PROJECT_NAME)
open_solution "solution1"
export_design -rtl verilog -format ip_catalog -flow impl -description "MGS vectorized matrix inversion" -vendor "BlackLynx" -library "dogfish" -display_name "MGS vectorized matrix inversion"
exit
