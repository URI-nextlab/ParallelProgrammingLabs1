#include <stdio.h>
#include "mvm_sa.hpp"

int main(int argc, char* argv[]){
	data_stream A_stream;
	data_stream x_stream;
	data_stream y_stream;
	data_t A[N][N];
	data_t x[N];
	acc_t soft_y[N] = {0};
	acc_t hard_y[N];

	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			A[i][j] = i * N + j;
		}
		x[i] = i;
	}
// push into stream first
	for (int k = 0; k < 25; k++){
		for (int i = 0; i < N; i++){
			data_axis_dp temp;
			temp.data = x[i];
			temp.keep = -1;
			temp.last = (i == (N - 1));
			x_stream << temp;
		}
	}

// run kernel
	for (int k = 0; k < 25; k++){
		mvm_sa(x_stream, y_stream);
	}
	for (int k = 0; k < 25; k++){
		for (int i = 0; i < N; i++){
			acc_axis_dp temp;
			y_stream >> temp;
			hard_y[i] = temp.data;
		}
	}
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			soft_y[j] += A[j][i] * x[i];
		}
	}
	bool correct = true;

	for (int i = 0; i < N; i++){
		printf("%5d\t%5d\n",soft_y[i],hard_y[i]);
		if (soft_y[i] != hard_y[i]){
			correct = false;
		}
	}
	if (correct){
		printf("Pass!\n");
		return 0;
	}
	else{
		printf("Fail!\n");
		return 1;
	}
}
