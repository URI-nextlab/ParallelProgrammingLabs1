# create project
open_project -reset fir

# set top function
set_top fir

# add files
add_files ./srcs/fir.cpp
add_files ./srcs/fir.h
add_files -tb ./srcs/fir_test.cpp
add_files -tb ./srcs/out.gold.dat
add_files -tb ./srcs/input.dat

# create solution
open_solution -reset -flow_target vivado solution1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

#csim_design
csynth_design
#cosim_design

export_design -rtl verilog -format ip_catalog -output ./IP/export.zip
exit
