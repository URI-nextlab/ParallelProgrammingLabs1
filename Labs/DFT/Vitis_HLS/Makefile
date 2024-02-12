
.PHONY: all clean

all:
	-@vitis_hls -f dft_prj.tcl
	-@cd ./IP && unzip export.zip

clean:
	-@rm -rf dft
	-@rm -rf IP/*
	-@rm -rf *.log
