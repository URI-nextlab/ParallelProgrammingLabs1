/*
Filename: dft.h
	Header file
	DFT lab
*/
#ifndef DFT_H_
#define DFT_H_

#include "hls_stream.h"
#include "ap_axi_sdata.h"
#include "ap_fixed.h"

const int N = 4;

typedef ap_fixed<32,8> data_t;
typedef ap_fixed<32,8> acc_t;

typedef struct {
	data_t x[N];
}dft_tdp;

typedef struct {
	acc_t real[N];
	acc_t imag[N];
}dft_fdp;

typedef hls::axis<dft_tdp,0,0,0> dft_tdp_axis_dp;
typedef hls::axis<dft_fdp,0,0,0> dft_fdp_axis_dp;
typedef hls::stream<dft_tdp_axis_dp> dft_time_stream;
typedef hls::stream<dft_fdp_axis_dp> dft_freq_stream;

void dft (
		dft_freq_stream& y,
		dft_time_stream& x
);

#endif
