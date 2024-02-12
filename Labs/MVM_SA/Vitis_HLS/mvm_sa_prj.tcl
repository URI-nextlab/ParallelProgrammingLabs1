# create project
open_project -reset mvm_sa

# set top function
set_top mvm_sa

# add files
add_files ./srcs/mvm_sa.cpp
add_files ./srcs/mvm_sa.h
add_files -tb ./srcs/mvm_sa_test.cpp

# create solution
open_solution -reset -flow_target vivado solution1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

#csim_design
csynth_design
#cosim_design

export_design -rtl verilog -format ip_catalog -output ./IP/export.zip
exit
