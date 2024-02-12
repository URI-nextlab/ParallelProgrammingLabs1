/*
Filename: MVM.h
	Header file
	MVM lab
*/
#ifndef MVM_H_
#define MVM_H_

#include "hls_stream.h"
#include "ap_axi_sdata.h"
#include "ap_fixed.h"

const int N = 4;

typedef int data_t;
typedef int acc_t;



typedef hls::axis<data_t,0,0,0> data_axis_dp;
typedef hls::axis<acc_t,0,0,0> acc_axis_dp;
typedef hls::stream<data_axis_dp> data_stream;
typedef hls::stream<acc_axis_dp> acc_stream;

void mvm (
		data_stream& A_stream,
		data_stream& x_stream,
		data_stream& y_stream
);

#endif
