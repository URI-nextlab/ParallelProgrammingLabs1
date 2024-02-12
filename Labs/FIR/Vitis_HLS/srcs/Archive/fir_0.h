/*
Filename: fir.h
	Header file
	FIR lab
*/
#ifndef FIR_H_
#define FIR_H_

#include "hls_stream.h"
#include "ap_axi_sdata.h"
#include "ap_fixed.h"

const int N=11;

typedef ap_fixed<10,10> coef_t;
typedef ap_fixed<8,8> data_t;
typedef ap_fixed<19,19> acc_t;

typedef hls::axis<data_t,0,0,0> data_t_pack;
typedef hls::axis<acc_t,0,0,0> acc_t_pack;
typedef hls::stream<data_t_pack> d_in_stream;
typedef hls::stream<acc_t_pack> d_out_stream;

void fir (
  d_out_stream& y,
  d_in_stream& x
);

#endif
