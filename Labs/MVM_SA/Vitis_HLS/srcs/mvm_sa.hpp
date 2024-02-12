#ifndef __MVM_SA_HPP__
#define __MVM_SA_HPP__

#include "hls_stream.h"
#include "ap_axi_sdata.h"

const int N = 4;

typedef int data_t;
typedef int acc_t;

typedef struct{
	data_t a[N];
}matrix_col;

typedef hls::axis<data_t,0,0,0> data_axis_dp;
typedef hls::axis<acc_t,0,0,0> acc_axis_dp;
typedef hls::axis<matrix_col,0,0,0> col_axis_dp;
typedef hls::stream<data_axis_dp> data_stream;
typedef hls::stream<acc_axis_dp> acc_stream;
typedef hls::stream<col_axis_dp> col_stream;

void mvm_sa(data_stream& x_stream, acc_stream& y_stream);

#endif
