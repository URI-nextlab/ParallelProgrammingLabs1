.PHONY: all clean

all:
	-@vitis_hls -f FFT_prj.tcl
	-@cd ./IP && unzip export.zip


clean:
	-@rm -rf FFT
	-@rm -rf IP/*
	-@rm -f *.log
