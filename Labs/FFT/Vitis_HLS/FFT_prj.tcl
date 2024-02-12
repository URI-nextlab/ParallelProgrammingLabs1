# create project
open_project -reset FFT

# set top function
set_top FFT

# add files
add_files ./srcs/FFT.cpp
add_files ./srcs/FFT.hpp
add_files -tb ./srcs/FFT_test.cpp

# create solution
open_solution -reset -flow_target vivado solution1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csynth_design

export_design -rtl verilog -format ip_catalog -output ./IP/export.zip

exit
