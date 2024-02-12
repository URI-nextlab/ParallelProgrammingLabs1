
.PHONY: all clean

all:
	-@vitis_hls -f mvm_sa_prj.tcl
	-@cd ./IP && unzip export.zip

clean:
	-@rm -rf mvm_sa
	-@rm -rf IP/*
	-@rm -rf *.log
