IMPL ?= 2.1

.PHONY: all clean

all:
	-@cp srcs/Archive/fir_$(IMPL).cpp srcs/fir.cpp
	-@cp srcs/Archive/fir_$(IMPL).h srcs/fir.h
	-@vitis_hls -f fir_prj.tcl
	-@cd ./IP && unzip export.zip

clean:
	-@rm -rf fir
	-@rm -rf IP/*
	-@rm -rf *.log
