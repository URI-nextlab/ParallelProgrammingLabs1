---
sort: 10
---


# FFT

<script type="text/x-mathjax-config">
  MathJax.Hub.Config({
    tex2jax: {
        inlineMath: [ ['$','$'], ["\\(","\\)"] ],
        displayMath: [ ['$$','$$'], ["\\[","\\]"] ],
        processEscapes: false,
    }
  });
</script>
    
<script type="text/javascript"
        src="https://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML">
</script>

Fast Fourier Transform (FFT) is an optimized algorithm to calculate DFT. The formula for DFT is shown below:

$$
X(k) = \sum_{n = 0}^{N - 1}x(n)e^{-j\frac{2\pi}{N}nk}
$$

For an $N$ point series, the output of DFT is also an $N$ point complex series. Each output point $X(k)$ requires $N$ multiplications (ignoring adds). Therefore, to calculate DFT according to the formula, $N^2$ multiplications are required, and it is considered as an $O(N^2)$ algorithm. $O(N^2)$ algorithm is typically not acceptable as the computing time increases too fast as $N$ increases. The divide-and-conquer algorithm is a typical way to slow down the trend. The basic requirements are that the problem can be divided into two parts and can be combined afterwards without too much overhead. If the requirements are satisfied, it is possible to continue dividing the two sub-problems into four sub-sub-problems until the final problem can be solved with ease. In each dividing stage, the computation required is mostly $O(N)$, and there are $log_2^N$ stages. Hence, complexity of the original problem is reduced to the $O(N\times log_2^N)$, which is a significant improvement.

Then the problem is how to divide the DFT operations. A more intuitive way is shown below (from now, we assume $N = 2^M$ and $M$ is an integer). First, we divide the series $x$ into two parts $x_e = x(2n)$ and $x_o = x(2n+1)$, where $x_e$ is the sample on the even index and the $x_o$ is the sample on the odd index. Then, we have:

$$
\begin{aligned}
X(k) &= \sum_{n = 0}^{N - 1}x(n)e^{-j\frac{2\pi}{N}nk}\\
&= \sum_{n = 0}^{N/2 - 1}x_e(n)e^{-j\frac{2\pi}{N}2nk} + \sum_{n = 0}^{N/2 - 1}x_o(n)e^{-j\frac{2\pi}{N}(2n+1)k}\\
&= \sum_{n = 0}^{N/2 - 1}x_e(n)e^{-j\frac{2\pi}{N}2nk} + e^{-j\frac{2\pi}{N}k}\sum_{n = 0}^{N/2 - 1}x_o(n)e^{-j\frac{2\pi}{N}(2n)k}\\
&= \sum_{n = 0}^{N/2 - 1}x_e(n)e^{-j\frac{2\pi}{\frac{N}{2}}nk} + e^{-j\frac{2\pi}{N}k}\sum_{n = 0}^{N/2 - 1}x_o(n)e^{-j\frac{2\pi}{\frac{N}{2}}nk}\\
&= DFT(x_e) + e^{-j\frac{2\pi}{N}k}DFT(x_o)
\end{aligned}
$$

Now, the $N$ point DFT is divided into two $\frac{N}{2}$ DFT. The combination of the two DFTs requires $N$ multiplications. Another useful fact is that $W_N^{k+\frac{N}{2}} = -W_N^k$ ( $k = 0 \to \frac{N}{2}-1$ ). That gives the well-known radix-2 butterfly structure ([Ref](https://en.wikipedia.org/wiki/Butterfly_diagram)). Take 8 point FFT as an example, the stage is shown below:

<img src="./imgs/EightPtFFTLastStage.png" width="600"/>

The two $\frac{N}{2}$ can be divided into two $\frac{N}{4}$ FFTs, and the structure is similar. Finally, for an 8-point FFT, the system can be shown as:


<img src="./imgs/FFT_structure.png" width="600"/>

Hence, to perform FFT, we have to take the following steps:

1. Resort the data, bit reverse.
2. Calculate $W_N^0\to W_N^{\frac{N}{2}-1}$.
3. Path the data through $log_2^N$ stages.

# Algorithm Introduction

## Bit reverse

The first step is to sort the input series. It is a little complex on software, but very easy on hardware. The software implementation is shown below:

```c++
unsigned int reverse_bits(unsigned int input) {
  int i, rev = 0;
  for (i = 0; i < M; i++) {
    rev = (rev << 1) | (input & 1);
    input = input >> 1;
  }
  return rev;
}
```

This implementation is slow as there is a data dependency between loops as the input is shifted right one by one. In hardware, bit reversing is simply reordering the wires. In HLS, the fixed point number defined by ap_fixed<> (or ap_ufixed<>) allows bitwise access, which means the fixed point number can also be viewed as an array of binary numbers ([Ref](https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Other-Class-Methods-Operators-and-Data-Members)). Therefore, we have the following simpler implementation:

```c++
#define N 1024
#define S 10

typedef ap_ufixed<S,S> idx_type;

idx_type idx_reverse(idx_type idx_in){
#pragma HLS INLINE
    idx_type idx_out;
    for (int i = 0; i < S;i++){
#pragma HLS UNROLL
        idx_out[S - 1 - i] = idx_in[i];
    }
    return idx_out;
}
```

This code reverses the bit order and outputs the new index value. This implementation is better as the loop can be entirely unrolled, which imposes minimum latency. It turns out that after synthesis, the index reverse function doesn't exist at all as it is nothing for hardware (rewiring only). Then, we can use the index to reorder the input series.

```c++
void bit_reverse(cplx x_in[N],cplx x_out[N]){
    for (int i = 0; i < N;i++){
        idx_type idx = (idx_type)(i);
        idx_type idx_r = idx_reverse(idx);
#ifndef __SYNTHESIS__
        printf("%d, %d\n",(int)idx, (int)idx_r);
#endif
        x_out[idx_r] = x_in[idx];
    }
}
```

Notice that the code puts the original data into new places, which means the $x_{in}$ is accessed in order. This matches the nature of a stream interface so that the bit reverse operation can start immediately after the first input data comes and finish immediately after the the last input data is received. The ```#ifndef __SYNTHESIS__``` and ```#endif``` is used to wrap the code that it is only synthesizable in the simulation. For example, the printf here is used to show the original index and the bit reversed index for debugging. This is an especially important skill when the function is complex and hard to deduce what is happening. Notice that standard C printf doesn't support fixed-point numbers, so we have to do a format transform when using fixed point numbers with either ```%d``` or ```%f```.

The second script is the butterfly operation. Here is a simple implementation where **w_r** (**w_i**) is the real (imaginary) part of $W_N^k$:

```c++
typedef float dtype;
typedef float wtype;
typedef struct{
    dtype real;
    dtype imag;
}cplx;

// y_l = x_l + x_h * W
// y_h = x_l - x_h * W

void butterfly(cplx& y_l, cplx& y_h,cplx x_l, cplx x_h, dtype wr, dtype wi){
#pragma HLS INLINE
    dtype r_temp = wr * x_h.real - x_h.imag * wi;
#pragma HLS BIND_OP variable=r_temp op=fmul impl=meddsp
    dtype i_temp = wr * x_h.imag + x_h.real * wi;
#pragma HLS BIND_OP variable=i_temp op=fmul impl=meddsp
    y_l.real = x_l.real + r_temp;
    y_l.imag = x_l.imag + i_temp;
    y_h.real = x_l.real - r_temp;
    y_h.imag = x_l.imag - i_temp;
}
```

The INLINE pragma ([Ref](https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-inline)) is used to avoid overhead caused by function calling (function calling in hardware requires handshake logic, which requires extra logic and adds latency) and to reduce the hierarchy.

Since we are implementing 1024 point FFT on PYNQ Z2 (xz7z020) which only has 220 DSP slices, we have to carefully consider if the DSPs are enough. According to the final report, if we only use DSP slices to perform the floating point multiplication, 234 DSP slices are required, which is over the capability of the board. Therefore, BIND_OP pragma ([Ref](https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-bind_op)) is used to specify which resource to use when implementing the floating point multiplication. This pragma is applied to the result of the mathematical operation. For example, the code forces all floating point multiplication (fmul) to generate **r_temp** (medium DSP resource usage). Its uses both DSP and logic fabric for multiplication. This pragma can reduce required DSP resources and make final implementation possible.

Then, the next step is to implement each stage in FFT. Since different states have different structures, the stage number should also be one of the inputs of the stage function, and the stage function should be able to generate the butterflies according to the stage and $N$. Here is the implementation:

```c++
void fft_stage(cplx x_in[N], cplx x_out[N], int stage){
    int butters_per_block = 1 << (stage - 1);
    int points_per_block = 1 << (stage);
    int blocks = 1 << (S - stage);
#ifndef __SYNTHESIS__
    printf("Stage %d, blocks = %d, butters per block = %d, points per block = %d\n",stage,blocks,butters_per_block,points_per_block);
#endif
    for (int j = 0; j < butters_per_block;j++){
        idx_type k = j * blocks;
        dtype wr = W_real[k];
        dtype wi = W_imag[k];
        for (int i = 0; i < blocks;i++){
#pragma HLS pipeline
            idx_type idx = points_per_block * i + j;
#ifndef __SYNTHESIS__
            printf("butter %d,%d\n",(int(idx)), int(idx+butters_per_block));
#endif
            butterfly(x_out[idx],x_out[idx + butters_per_block],x_in[idx],x_in[idx + butters_per_block], wr, wi);
        }
    }
}
```

The first thing to determine is how many small FFT blocks are inside the stage. For example, the first stage of the $N=1024=2^{10}$ FFT should have 512 blocks of 2-point FFTs; and the second stage is then 256 blocks of 4-point FFT. Hence, we can decide that the number of blocks equals $2^{TotalStages - stage}$ (stage counts from 1). Then we have to decide how many points inside each block and obviously, it equals $N/(number of blocks) = 2^{stage} = 1 << stage$. Then, it is time to build the structure. Since the $W_N^k$ used in all FFT blocks are identical, the outer loop is the number of butterflies in each block so that we don't have to read from the ROM that saves $W_N^k$ for multiple times. Then, in each block, we apply the butterfly structure accordingly. Notice that we only pipelined the inner loop, which means it always needs $N/2$ trip counts to finish one stage, which is slow compared to an unrolled structure. However, considering that the FFT requires $N$ clock cycles to load that data and $N$ clock cycles to write out the result, it doesn't make sense to make the tasks inside FFT to be faster than $N$ clock cycles. For example, an $8$ point FFT has 3 stages, and we treat the bit reversal and each stage as a task. DATAFLOW pragma was introduced in DFT section to pipelining the tasks. As long as the middle tasks are no slower than the input/output stages, the FFT kernel can always receive new data and start processing. Therefore, unless we need minimum latency from the input to the output, we don't have to make the middle tasks too fast. That is why the loop is not unrolled here even when there is no data dependency exists between the loops, as you can see in the FFT structure (each source data is only read once and each output data is also only written once).


The final step is to construct the entire FFT. Here is the final top function and the test bench:

top:

```c++
void FFT(cplx_stream& x_in, cplx_stream& y_out){
#pragma HLS INTERFACE mode=ap_ctrl_none port=return
#pragma HLS INTERFACE mode=axis register_mode=both port=y_out register
#pragma HLS INTERFACE mode=axis register_mode=both port=x_in register
#pragma HLS DATAFLOW
    cplx x_local[N];
    cplx x_local_s[S+1][N];
#pragma HLS ARRAY_PARTITION dim=1 variable=x_local_s type=complete

    for (int i = 0; i < N; i++){
        cplx_dp temp;
        x_in >> temp;
        x_local[i].real = temp.data.real;
        x_local[i].imag = temp.data.imag;
    }

    bit_reverse(x_local, x_local_s[0]);
#ifndef __SYNTHESIS__
    for (int i = 0; i < N;i++){
    	printf("%f, %f\n",x_local[i].real,x_local_s[0][i].real);
    }
#endif
    for (int i = 1; i <= S; i++){
#pragma HLS UNROLL
    	fft_stage(x_local_s[i-1],x_local_s[i],i);
    }

    for (int i = 0; i < N; i++){
        cplx_dp temp;
        temp.data.real = x_local_s[S][i].real;
        temp.data.imag = x_local_s[S][i].imag;
        temp.keep = -1;
        temp.last = (i == (N - 1));
        y_out << temp;
    }
}
```

testbench:

```c++
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
```

The top function is simple. It just links the bit reverse and all 10 FFT stages. When DATAFLOW structure is used, a bunch of storage is required to connect all the stages. The intermediate results from all stages are only used once.

# Result

In the simulation, we can see the waveform shown below:

<img src="./imgs/fft_waveform.png" width="600"/>

The kernel can receive data in a batch (1024 data in a batch in this example) continuously and after $73.465\mu s$ (latency needed to process 1 batch), the result comes out continuously. Hence, the final **II** in a batch of data is 1. However, the **II** between different batches is not 1. A new batch needs to wait for about $73.465\mu s$ (latency needed to process 1 batch) to stream into the kernel.

After generating the bitstream (run ```make run``` in the Labs/FFT/Vivado/ folder). We can run it on PYNQ. Here is the code and result.

```python
from pynq import allocate
from pynq import Overlay
import numpy as np
import matplotlib.pyplot as plt

hw = Overlay("system.bit")
dma = hw.axi_dma_0

N = 1024
ibuf = allocate((N,), dtype='csingle')
obuf = allocate((N,), dtype='csingle')


idx = np.arange(0,1024)
ibuf[:] = np.cos(0.25 * np.pi * idx) + 0.5 * np.cos(0.125 * np.pi * idx) + 0.25 * np.cos(0.3765 * np.pi * idx)

dma.sendchannel.transfer(ibuf)
dma.recvchannel.transfer(obuf)

nm_freq = idx[0:512]/512
plt.figure(dpi=200)
plt.stem(nm_freq,np.abs(obuf[0:512])/N,use_line_collection=True)
plt.xlim([0,1])
plt.xlabel('Normalzied frequency (0~1)')
plt.ylabel('Magnitude')

```

<img src="./imgs/fft_exp.png" width="600"/>

In this testing code, a signal with three frequency components is generated. The first two frequencies are at the bins so that they have a single peak at that frequency and the magnitude equals the time domain magnitude. The third frequency is not at the bins and therefore it has some leakages and the peak magnitude cannot fully represent the time domain magitude.
