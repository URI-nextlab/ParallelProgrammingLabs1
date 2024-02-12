#!/bin/bash

PRJ_NAME=$1

echo "Create a project named as '${PRJ_NAME}'"

mkdir ${PRJ_NAME}

mkdir ${PRJ_NAME}/Vitis_HLS
mkdir ${PRJ_NAME}/Vitis_HLS/srcs
mkdir ${PRJ_NAME}/Vitis_HLS/IP

cat > ${PRJ_NAME}/Vitis_HLS/Makefile << EOL
.PHONY: all clean

all:
    -@vitis_hls -f ${PRJ_NAME}_prj.tcl

clean:
    -@rm -rf ${PRJ_NAME}
    -@rm -rf IP/*
    -@rm -f *.log

EOL

cat > ${PRJ_NAME}/Vitis_HLS/${PRJ_NAME}_prj.tcl << EOL
# create project
open_project -reset ${PRJ_NAME}

# set top function
set_top ${PRJ_NAME}

# add files
add_files ./srcs/${PRJ_NAME}.cpp
add_files ./srcs/${PRJ_NAME}.hpp
add_files -tb ./srcs/${PRJ_NAME}_test.cpp

# create solution
open_solution -reset -flow_target vivado solution1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csynth_design

export_design -rtl verilog -format ip_catalog -output ./IP

exit
EOL

cat > ${PRJ_NAME}/Vitis_HLS/srcs/${PRJ_NAME}.hpp << EOL
#ifndef __${PRJ_NAME}_HPP__
#define __${PRJ_NAME}_HPP__

#endif
EOL

cat > ${PRJ_NAME}/Vitis_HLS/srcs/${PRJ_NAME}.cpp << EOL
#include "${PRJ_NAME}.hpp"
EOL

cat > ${PRJ_NAME}/Vitis_HLS/srcs/${PRJ_NAME}_test.cpp << EOL
#include "${PRJ_NAME}.hpp"
EOL

mkdir ${PRJ_NAME}/Vivado
