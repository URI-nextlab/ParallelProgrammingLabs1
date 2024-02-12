
.PHONY: all clean

all:
	-@vitis_hls -f mvm_prj.tcl
	-@cd ./IP && unzip export.zip

clean:
	-@rm -rf mvm
	-@rm -rf IP/*
	-@rm -rf *.log
