#include "mvm_sa.hpp"


void mvm_sa(data_stream& x_stream, acc_stream& y_stream){
#pragma HLS INTERFACE mode=ap_ctrl_none port=return
#pragma HLS INTERFACE mode=axis port=x_stream
#pragma HLS INTERFACE mode=axis port=y_stream
#pragma HLS DATAFLOW
	static const data_t A_local[N][N] = {
			0,1,2,3,
			4,5,6,7,
			8,9,10,11,
			12,13,14,15
	};
#pragma HLS BIND_STORAGE variable=A_local type=rom_2p impl=bram latency=1
#pragma HLS ARRAY_PARTITION variable=A_local dim=1 type=complete
	data_t x_local[N];
#pragma HLS ARRAY_PARTITION variable=x_local dim=1 type=complete
	data_t acc[N];
#pragma HLS ARRAY_PARTITION variable=acc dim=1 type=complete

load_x_loop:
	for (int i = 0; i < N;i++){
#pragma HLS PIPELINE
		data_axis_dp temp;
		x_stream >> temp;
		for (int j = 0; j < N;j++){
#pragma HLS UNROLL
			acc_t last = (i == j)? 0:acc[j];
			data_t x = (j > 0)?x_local[j - 1]:temp.data;
			data_t a = (i >= j)?A_local[j][i - j]:0;
			data_t mul = a * x;
#pragma HLS BIND_OP variable=mul op=mul impl=dsp
			acc[j] = last + mul;
		}
shift_x0_lopp:
		for (int j = N-1; j > 0;j--){
#pragma HLS UNROLL
			x_local[j] = x_local[j - 1];
		}
		x_local[0] = temp.data;
	}

continue_shift_x_loop:
	for (int i = 0; i < N; i++){
#pragma HLS PIPELINE
		acc_axis_dp temp;
		temp.data = acc[i];
		temp.keep = -1;
		temp.last = (i == (N-1));
		y_stream << temp;
		for (int j = 0; j < N; j++){
#pragma HLS UNROLL
			acc_t last = acc[j];
			data_t x = x_local[j - 1];
			data_t a = (j > i)? A_local[j][i + N - j] : 0;
			data_t mul = a * x;
#pragma HLS BIND_OP variable=mul op=mul impl=dsp
			acc[j] = last + mul;
		}
		// shift_reg
		for (int j = N-1; j > 0;j--){
#pragma HLS UNROLL
			x_local[j] = x_local[j - 1];
		}
	}
}
