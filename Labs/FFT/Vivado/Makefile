.PHONY: all clean move
PRJ_NAME ?= FFT

all:
	-@vivado -nolog -nojournal -mode batch -source ./srcs/generate_bitstream.tcl
	-@cp prj/$(PRJ_NAME).runs/impl_1/*.bit ./system.bit
	-@cp prj/system/hw_handoff/system.hwh ./

clean:
	-@rm -f ./system.bit
	-@rm -f ./system.hwh
	-@rm -rf ./prj


move:
	-@cp prj/$(PRJ_NAME).runs/impl_1/*.bit ./system.bit
	-@cp prj/system/hw_handoff/system.hwh ./

