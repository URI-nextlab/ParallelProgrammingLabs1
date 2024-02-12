#include "FFT.hpp"
#include <math.h>

#define PI (3.141592653f)

int main(int argc, char* argv[]){
    cplx_stream in_stream, out_stream;

    float x[N];
    float y[N];
    for (int it = 0; it < 12;it++){
		for (int i = 0;i < N;i++){
			x[i] = cos(0.25 * PI * i);
			cplx_dp temp;
			temp.data.real = x[i];
			temp.data.imag = 0;
			temp.keep = -1;
			temp.last = (i == (N - 1));
			in_stream << temp;
		}
    }

    for (int it = 0; it < 12;it++){
    	FFT(in_stream,out_stream);
    }

    for (int it = 0; it < 12;it++){
		for (int i = 0;i < N;i++){
			cplx_dp temp;
			out_stream >> temp;
			y[i] = sqrt(temp.data.real * temp.data.real + temp.data.imag * temp.data.imag);
		}
    }
    float max = y[0];
    int max_idx = 0;
    for (int i = 1;i < N;i++){
    	if(y[i] > max){
    		max = y[i];
    		max_idx = i;
    	}
    }
    if (max_idx / (float)N * 2 != 0.25){
    	printf("Fail!\n");
    	return 1;
    }

	printf("Pass!\n");
    return 0;
}
