.PHONY: all clean move
PRJ_NAME ?= mvm

all:
	-@vivado -nolog -nojournal -mode batch -source ./srcs/generate_bitstream.tcl
	-@cp prj/$(PRJ_NAME).runs/impl_1/*.bit ./mvm.bit
	-@cp prj/mvm/hw_handoff/mvm.hwh ./

clean:
	-@rm -f ./system.bit
	-@rm -f ./system.hwh
	-@rm -rf ./prj/*


move:
	-@cp prj/$(PRJ_NAME).runs/impl_1/*.bit ./system.bit
	-@cp prj/mvm/hw_handoff/mvm.hwh ./system.hwh

