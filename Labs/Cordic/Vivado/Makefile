.PHONY: all clean move
PRJ_NAME ?= CORDIC

all:
	-@vivado -nolog -nojournal -mode batch -source ./srcs/generate_bitstream.tcl
	-@cp prj/$(PRJ_NAME).runs/impl_1/*.bit ./cordic.bit
	-@cp prj/cordic/hw_handoff/cordic.hwh ./

clean:
	-@rm -f ./*.bit
	-@rm -f ./*.hwh
	-@rm -rf ./prj/*


move:
	-@cp prj/$(PRJ_NAME).runs/impl_1/*.bit ./cordic.bit
	-@cp prj/cordic/hw_handoff/cordic.hwh ./

