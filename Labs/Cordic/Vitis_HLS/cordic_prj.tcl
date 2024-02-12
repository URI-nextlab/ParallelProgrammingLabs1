# create project
open_project -reset cordic

# set top function
set_top cordic

# add files
add_files ./srcs/cordic.cpp
add_files ./srcs/cordic.h
add_files -tb ./srcs/cordic_test.cpp
# create solution
open_solution -reset -flow_target vivado solution1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

#csim_design
csynth_design
#cosim_design

export_design -rtl verilog -format ip_catalog -output ./IP/export.zip
exit
