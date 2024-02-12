#include "mvm.h"

void mvm (
		data_stream& A_stream,
		data_stream& x_stream,
		data_stream& y_stream
){
#pragma HLS INTERFACE mode=ap_ctrl_none port=return
#pragma HLS INTERFACE mode=axis register_mode=both port=A_stream
#pragma HLS INTERFACE mode=axis register_mode=both port=x_stream
#pragma HLS INTERFACE mode=axis register_mode=both port=y_stream
//#pragma HLS DATAFLOW

	data_t local_A[N][N];
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=local_A
	acc_t local_y[N] = {0};
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=local_y

load_A_loop:
	for (int loc = 0, i = 0, j = 0; loc < N * N; loc++, j++) {
#pragma HLS PIPELINE
        if (j == N) {
            i++;
            j = 0;
        }
        data_axis_dp temp;
        A_stream >> temp;
        local_A[i][j] = temp.data;
    }

COL_LOOP:
	for (int i = 0; i < N; i++){
#pragma HLS PIPELINE
		data_axis_dp temp;
		x_stream >> temp;
ROW_LOOP:
		for(int j = 0; j < N;j++){
#pragma HLS UNROLL
			local_y[j] += local_A[j][i] * temp.data;
		}
	}

write_y_loop:
	for (int i = 0; i < N;i++){
#pragma HLS PIPELINE
		acc_axis_dp temp;
		temp.data = local_y[i];
		temp.keep = -1;
		temp.last = (i == (N-1));
		y_stream << temp;
	}

}

