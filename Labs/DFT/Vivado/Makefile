.PHONY: all clean move
PRJ_NAME ?= FIR

all:
	-@vivado -nolog -nojournal -mode batch -source ./srcs/generate_bitstream.tcl
	-@cp prj/$(PRJ_NAME).runs/impl_1/*.bit ./fir.bit
	-@cp prj/fir/hw_handoff/fir.hwh ./

clean:
	-@rm -f ./system.bit
	-@rm -f ./system.hwh
	-@rm -rf ./prj/*


move:
	-@cp prj/$(PRJ_NAME).runs/impl_1/*.bit ./system.bit
	-@cp prj/fir/hw_handoff/fir.hwh ./system.hwh

