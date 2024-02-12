
.PHONY: all clean

all:
	-@vitis_hls -f cordic_prj.tcl
	-@cd ./IP && unzip export.zip

clean:
	-@rm -rf cordic
	-@rm -rf IP/*
	-@rm -rf *.log
