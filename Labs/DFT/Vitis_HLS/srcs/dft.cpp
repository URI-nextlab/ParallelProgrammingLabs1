#include "dft.h"

// Not optimzied code in Figure 2.1

void dft(
		dft_freq_stream& y,
		dft_time_stream& x)
{
#pragma HLS INTERFACE mode=ap_ctrl_none port=return
#pragma HLS INTERFACE mode=axis register_mode=both port=y
#pragma HLS INTERFACE mode=axis register_mode=both port=x

#pragma HLS pipeline style=frp
	dft_tdp_axis_dp x_temp;
	x >> x_temp;

	dft_fdp_axis_dp y_temp;
	y_temp.data.real[0] = (x_temp.data.x[0] + x_temp.data.x[1]) + (x_temp.data.x[2] + x_temp.data.x[3]);
	y_temp.data.real[1] = (x_temp.data.x[0] - x_temp.data.x[2]);
	y_temp.data.real[2] = (x_temp.data.x[0] - x_temp.data.x[1]) + (x_temp.data.x[2] - x_temp.data.x[3]);
	y_temp.data.real[3] = (x_temp.data.x[0] - x_temp.data.x[2]);


	y_temp.data.imag[0] = 0;
	y_temp.data.imag[1] = (-x_temp.data.x[1] + x_temp.data.x[3]);
	y_temp.data.imag[2] = 0;
	y_temp.data.imag[3] = (x_temp.data.x[1] - x_temp.data.x[3]);

	y_temp.last = x_temp.last;
	y_temp.keep = -1;
	y << y_temp;

}

