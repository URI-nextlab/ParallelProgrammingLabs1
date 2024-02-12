---
sort: 9
---

# DFT

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


## Introduction
Discrete Fourier Transform (DFT) is widely used in digital signal processing (Fast-Fourier Transform, FFT, is an optimized algorithm to calculate DFT). The formula is simple:

$$
X(k) = \sum_{n = 0}^{N - 1}x(n)e^{-j\frac{2\pi}{N}nk}
$$

Given a certain $k$, DFT is actully a inner product between series $x(n)$ and a complex rotating vector $e^{-j\frac{2k\pi}{N}n}$. Therefore, let $x(n)$ written as a column vector $\vec{x}$, and $e^{-j\frac{2k\pi}{N}n}$ written as a row vector $\vec T_k$, we have $X(k) = \vec T_k \cdot \vec x$. Hence, the entire transform becomes a Matrix-Vector Multiplication (MVM).

$$
\begin{equation}
\left[\begin{matrix}
X(0)\\
X(1)\\
...\\
X(N-1)
\end{matrix}\right]
=\left[\begin{matrix}
\vec T_0\\
\vec T_1\\
...\\
\vec T_{N-1}
\end{matrix}\right]
\vec x
\end{equation}
$$

$$
\vec T_k = e^{-j\frac{2k\pi}{N}n}, n = 0,1,2,...,N-1
$$

Since DFT has a better implementation called FFT, in this experiment we focus on Matrix-Vector Multiplication for real numbers for simplicity.

## Inner product implementation

The MVM can be treated as $N$ inner products between the row vectors in Matrix and the column vector. The inner product can be implemented with the following code:

```c++
int acc = 0;
for (int i = 0; i < N; i++){
    acc += t[i] * x[i];
}
```

The loop can either be pipelined or unrolled, depending on the data accessibility. For example, mostly the input $\vec x$ comes from a stream interface, which infers that in each clock cycle, only one $x[i]$ is available. In this case, even when the loop is unrolled, it still requires at least $N$ clocks to finish, which is the same with the pipelined structure. This structure is shown in (a) below. However, if both $x$ and $t$ have higher accessibility, it is possible to carry out more multiplications in each clock cycle and then reduce the trip count. For example, in (b), $x$ and $t$ are saved in two blocks. Therefore, in each cycle, two multiplications can be done. In another word, the 'for' loop can be unrolled by a factor of 2 in this case. Furthermore, if the $x$ and $t$ are completely partitioned, it is possible to finish all multiplication and the 'for' loop can be fully unrolled. In summary, data accessibility determines the parallelism of the implementation. The unroll factor should be picked carefully depending on how much data is available in one cycle. Also, if unroll is required to increase the performance, the memory used to save the data should also be changed accordingly. Xilinx provides 'array_partition' pragma to specify the data accessibility ([Ref](https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-array_partition)). In general, if the memory is implemented with BRAM on FPGA, two data is available in each cycle. If the memory is implemented with FFs (completely partitioned), all data are available in one clock cycle.  

<img src="./imgs/DotProduct.png" alt="drawing" width="600"/>

## MVM Implementation

The MVM is $N$ dot products. It can be realized with the following code:

```c++
int T[N][N];
int X[N] = {0};
COL_LOOP:
for (int i = 0; i < N; i++){
ROW_LOOP:
    for(int j = 0; j < N; j++){
        X[j] += T[j][i] * x[i];
    }
}
```

The first thing to do is to determine which loop should be first. In this example, we assume that $x$ comes in series, which means $x[i]$ comes in order and only one $x[i]$ is available in each clock cycle. In this case, once a new $x[i]$ is received, we should finish all computations that require $x[i]$, otherwise, extra memory is required to save the $x$ for future use. Hence, the $i$ loop (COL_LOOP) should be the first. If not, then $x$ has to be iterated in each outer loop. The second step is to determine the data accessibility in the ROW_LOOP. In the ROW_LOOP (j), the $i^{th}$ values in all rows of $T$ are required. Then, if we don't partition array $T$, only one $T[j][i]$ is available in each cycle, which means it takes $N^2$ trips to finish the operation. Partitioning the $T$ array in the second dimension doesn't help as only one $x[i]$ is available at a clock cycle. Partitioning the $T$ array in the first dimension makes the data in different rows available at the same time, which helps reduce the trip count. For example, if we completely partition the $T$ in the first dimension, the ROW_LOOP can also be fully unrolled, generating $N$ independent dot product instances like the figure (a) in the last section (see Figure 4.12 in the textbook as well).  

Since multiplication operation mostly requires more than 1 clock cycle to finish, we still need to pipeline the outer loop for better performance. The pipeline is shown below:

<img src="./imgs/DotProductPipeline.png" alt="drawing" width="600"/>

If we pipeline the outer loop and unroll the inner loop, $N$ instances of this pipeline are generated. Here is the implementation of the MVM ($y = Ax$, all real number cases):

mvm.h

```c++
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
```

```c++
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
#pragma HLS DATAFLOW

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
	for (int j = 0; j < N; j++){
#pragma HLS PIPELINE
		data_axis_dp temp;
		x_stream >> temp;
ROW_LOOP:
		for(int i = 0; i < N; i++){
#pragma HLS UNROLL
			local_y[i] += local_A[i][j] * temp.data;
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
```

Testbench

```c++
/*
	Filename: mvm_test.h
		Testbench file
		Calls mvm() function from mvm.cpp
		Compares the output from mvm() with out.gold.dat
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "mvm.h"

int main () {
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
	for (int k = 0; k < 5; k++){

		for (int i = 0; i < N; i++){
			for (int j = 0; j < N; j++){
				data_axis_dp temp;
				temp.data = A[i][j];
				temp.keep = -1;
				temp.last = (i == (N - 1)) && (j == (N - 1));
				A_stream << temp;
			}
			data_axis_dp temp;
			temp.data = x[i];
			temp.keep = -1;
			temp.last = (i == (N - 1));
			x_stream << temp;
		}
	}

// run kernel
	for (int k = 0; k < 5; k++){
		mvm(A_stream, x_stream, y_stream);
	}
	for (int k = 0; k < 5; k++){
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
```

The waveform is shown below:

<img src="./imgs/mvm_vhls.png" alt="drawing" width="850"/>

The waveform shows that the module has a low efficiency. Firstly, matrix A is reloaded every time. For an AXI stream bus, it requires $N^2$ clock cycles to reload the matrix, while the computation only requires $N$ cycles. This stops vector $x$ from being received continuously. Since in most cases, the matrix remains the same while the input vector varies (for example, DFT has a constant transform matrix). Secondly, this structure makes every new input $x[i]$ accessed by all rows simultaneously, which means one FF in the final implementation is fanned out to $N$ receivers. When $N$ is small, it is not a huge concern, however, as $N$ goes to hundreds or more, it leads to high load capacitance that will slow down the circuit. Xilinx Vivado may use redundant resources to avoid large fanout, but it consequently increases the difficulty of routing.

Systolic array is a typical way to solve the problem. The systolic array uses interconnected independent data processing elements to achieve the final algorithm. The input of a PE comes from the outside or other PEs; the input from the outside and the output of PE are only fanned out to one trasmitter/receiver. Thus, a chain or a network of PEs is formed. In MVM, each PE can simply perfrom the inner product (one row vector times one column vector). However, the input $x$ is only sent to the first PE (the first row) rather than all PEs as previously shown in the unrolled implementation example. The first PE then registers the input $x$ and sends it to the next PE (the second row). Therefore, PE can be simply described with the following figure and formula, where acc has to be cleared every time the calculation starts:

<center><img src="./imgs/PE.png" alt="drawing" width="150"/></center>

$$
\begin{aligned}
x_{out} &= x_{in}\\
A_{out} &= A_{in}\\
acc &= acc + A_{in} \times x_{in}
\end{aligned}
$$

Depending on how the PEs are connected, different operations can be realized. For example, if we connect the new PEs vertically ($x_{out}$ connect to new $x_{in}$, $A_{out}$ is neglected) and form an $N$ row $1$ column array, MVM is realized. If we form an $N$ by $N$ grid, matrix-matrix multiplication can be realized.  

Avoiding reloading $A$ every time is tricky. All HLS IPs have a block-level interface (Interface pragma, port = return)([Ref](https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-interface)). Except for ap_ctrl_none interface, the IP kernel (block, module) must be started manually. Unless the latency from the first input to the last output is much smaller than the length of the total data stream, or the whole module is implemented as an FRP pipeline (Like fir and Cordic example), starting signal can significantly slow down the process. For example, even with the use of systolic array, the first output of MVM comes after the final $x$ is received, which requires $N$ clock cycles, and additional $N$ clock cycles are still required to push out the result. In this case, even under the perfect circumstance, the latency between the first input and last output is at least $2\times N$. If the module requires a starting signal, the corresponding 'done' comes after $2\times N$ clock cycles. During this time, the software can only wait, and no input can be received between $N\to 2N$ cycles. Hence, even with all pipelines inside the loop achieving II=1, the total average II for the module is at least 2. Reloading $A$ requires at least $N^2$ clock cycles and no operation can be done during this time, which means it is impossible to make the module an FRP pipeline via HLS (reloading itself is a pipeline, then the whole module cannot be a pipeline because if the module is specified as pipelined the reload loop must be unrolled). To solve the problem, we have to create independent reloading $A$ module and let the module forwards the columns of $A$ to the MVM module. Only in this case, the MVM kernel is simple enough to be designed as an FRP pipeline. Vitis HLS cannot simulate with multiple kernels connected (Vitis allows). Therefore, here we assume the matrix $A$ is a constant matrix, and we just write it inside the bitstream and not need to be reloaded. The ultimate purpose is to have a module with II=1 and new data can still be received during the $N\to 2N$ cycles. In addition, we hope the kernel allows random stall, which means $x$ may not come continuously.

Here is one possible implementation with systolic array architecture:

mvm_sa.hpp

```c++
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
```

mvm.cpp

```c++
#include "mvm_sa.hpp"


void mvm_sa(data_stream& x_stream, acc_stream& y_stream){
#pragma HLS INTERFACE mode=ap_ctrl_none port=return
#pragma HLS INTERFACE mode=axis port=x_stream
#pragma HLS INTERFACE mode=axis port=y_stream
#pragma HLS DATAFLOW
	static const data_t A_local[N][N] = {
			0,    1,   2,   3,
			4,    5,   6,   7,
			8,    9,  10,  11,
			12,  13,  14,  15
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
			acc[j] = last + a * x;
		}
shift_x0_loop:
		for (int j = N-1; j > 0; j--){
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
			acc[j] = last + a * x;
		}
		// shift_reg
		for (int j = N-1; j > 0;j--){
#pragma HLS UNROLL
			x_local[j] = x_local[j - 1];
		}
	}
}
```

There are two parts in this implementation. First, the input $x$ shifts in (load_x_loop) and the PEs start running one by one. At the end of the first part, the entire $x$ is saved in a local shift register. Then the $x$ still needs to be shifted until the last PE gets the last value of $x$, which is taken care of by the continue_shift_x_loop. In this loop, the final result also comes out one by one. Hence, the $y$ is pushed into the output stream as well. The kernel requires $2N$ plus some latency to process one MVM. The DATAFLOW pragma can be used to pipelining to the two loops (treat each loop as a task, DATAFLOW is essentially a task-level pipeline) ([Ref](https://docs.xilinx.com/r/2021.2-English/ug1399-vitis-hls/pragma-HLS-dataflow)). Here, when the continue_shift_x_loop is processing the current input $x$, the load_x_loop is free to receive the next $x$. Notice that matrix $A$ can be accessed concurrently by the two loops, which means at least two read ports are required for the RAM that is saving the matrix. Therefore, bind_storage pragma is used to force the tool using two ports RAM to implement the ***A_local[N][N]*** array ([Ref](https://docs.xilinx.com/r/2021.2-English/ug1399-vitis-hls/pragma-HLS-bind_storage)). This implementation ensures high performance. The waveform is shown below.

<img src="./imgs/mvm_sa_vhls.png" alt="drawing" width="850"/>

With this implementation, the kernel is only stalled for one clock cycle and the new input $x$ and output $y$ come almost continuously, giving an average II equals $(N+1)/N$, which is almost 1 especially when $N$ is large enough.

## Special: Four-point DFT implementation
Since DFT has a better implementation called FFT, it doesn't make sense to create a large MVM kernel to do DFT in specific. However, Four-point DFT is important due to the simple $T$ matrix:

$$
\begin{equation}
T = \left[\begin{matrix}
1&1&1&1\\
1&-j&-1&j\\
1&-1&1&-1\\
1&j&-1&-j
\end{matrix}\right]
\end{equation}
$$

Obviously, only add and subtrication is required in Four-point DFT. Here is an implementation of the Four-point DFT:


dft.h
```c++
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
```

dft.cpp
```c++
#include "dft.h"

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
```

This module can be easily pipelined. According to Vitis HLS synthesis report, with 100MHz clock, the II for the module is 1 and the latency is also 1, which means it is a combinational logic module (Under higher frequency, it may become a real pipeline).

## Special: Almost hardware systolic array

Though the DATAFLOW can create the systolic array correctly, the DATAFLOW typically requires much more resources. For example, since the resources in the two loops are not shared, $2N$ multipliers are required. This problem is hard to solve with current HLS tools. To have a perfect systolic array, the code has to be rewritten completely in an almost Verilog/VHDL style. Here is one implementation:

```c++

typedef ap_fixed<40,14> d_htype;
typedef ap_fixed<80,14> d_htype_wide;
typedef ap_fixed<80,28> d_htype_acc;
typedef float d_stype;
typedef ap_uint<32> u32;
typedef ap_uint<8> u8;
typedef ap_uint<16> u16_bitwise;
typedef ap_uint<M> shift_bits;
typedef ap_uint<1> bit;
typedef ap_int<16> s16;

typedef hls::stream<d_htype> d_stream;
//typedef struct {
//  d_htype data;
//  bit last;
//  bit user;
//}dp;
typedef hls::axis<d_htype,1,0,0> dp;
typedef hls::axis<d_htype_wide,1,0,0> wdp;
typedef hls::stream<dp> dp_stream;
typedef hls::stream<wdp> wdp_stream;
typedef struct {
	d_htype data[M];
}Col;
 typedef hls::stream<Col> col_stream;
 
 typedef struct {
     d_htype data[M + 1];
 }SA_array;
 typedef hls::axis<SA_array,1,0,0> SA_dp;
 typedef hls::stream<SA_dp> SA_stream;
 typedef hls::stream<u16_bitwise> Ctrl_Stream;

void systolic_array(col_stream &A, dp_stream& x, wdp_stream& y){
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS aggregate variable=A_up compact=bit
#pragma HLS aggregate variable=A_down compact=bit
#pragma HLS pipeline ii=1

//	*	*	*	*	Define variables for systolic array	*	*	*	*	//
	// local fifo of A
	static hls::stream<d_htype,M*2> A_local[M - 1];
#pragma HLS array_partition variable=A_local dim=1 type=complete
	static dp x_local[M];
#pragma HLS array_partition variable=x_local dim=1 type=complete
	static d_htype_wide accumulator[M];
#pragma HLS array_partition variable=accumulator dim=1 type=complete
	static bit data_valid[M];
#pragma HLS array_partition variable=data_valid dim=1 type=complete
	static bit valid_local[M];
#pragma HLS array_partition variable=valid_local dim=1 type=complete
	static u8 out_pointer;

	static bool ready;


//	*	*	*	*	*	*	*	Input logic		*	*	*	*	*	*	//
	bool x_valid;
	dp x_temp;
	Col A_temp;
	//SA_dp SA_temp;
	if (((!A.empty()) && (!x.empty())){
		A >> A_temp;
		x >> x_temp;
		x_valid = 1;
push_A_loop:
		for (u32 i = 1; i < M; i++){
#pragma HLS unroll
			A_local[i - 1].write(A_temp.data[i]);
		}
	}
	else{
		x_valid = 0;
	}


//	*	*	*	*	*	*	*	Output logic	*	*	*	*	*	*	//
	if (data_valid[out_pointer] == 1){
		wdp y_temp;
		y_temp.data = accumulator[out_pointer];
		y_temp.last = (out_pointer == (M - 1));
		y_temp.user = (out_pointer == (0));
		y_temp.keep = -1;
		y << y_temp;
		if (out_pointer < M - 1){
			out_pointer++;
		}
		else{
			out_pointer = 0;
		}
	}
	//ready = !A_local[0].empty();
//	*	*	*	*	*	*	*	Compute logic	*	*	*	*	*	*	//
systolic_array_gen_loop:
	for (int i = M - 1; i >= 0; i--){
#pragma HLS unroll
		if (i == 0){
			if(x_valid){
				d_htype a = A_temp.data[0];
				if (x_temp.user == 1){
					accumulator[i] = a * x_temp.data;
					data_valid[i] = 0;
				}
				else if (x_temp.last == 1){
					accumulator[i] += a * x_temp.data;
					data_valid[i] = 1;
				}
				else{
					accumulator[i] += a * x_temp.data;
					data_valid[i] = 0;
				}
			}
			else{
				data_valid[i] = 0;
			}
			valid_local[i] = x_valid;
			x_local[i] = x_temp;
		}
		else{
			if (valid_local[i - 1]){
				d_htype a = A_local[i - 1].read();
				if (x_local[i - 1].user == 1){
					accumulator[i] = a * x_local[i - 1].data;
					data_valid[i] = 0;
				}
				else if (x_local[i - 1].last == 1){
					accumulator[i] += a * x_local[i - 1].data;
					data_valid[i] = 1;
				}
				else{
					accumulator[i] += a * x_local[i - 1].data;
					data_valid[i] = 0;
				}
			}
			else{
				data_valid[i] = 0;
			}
			valid_local[i] = valid_local[i - 1];
			x_local[i] = x_local[i - 1];
		} // end if i
	}

}
```

In this implementation, the whole module is pipelined and the II is 1, which means it has the same performance as the DATAFLOW implementation. The PE and entire structure generated with this code are shown below:

<img src="./imgs/PE_p.png" alt="drawing" width="350"/>
<img src="./imgs/SA.png" alt="drawing" width="350"/>

With the same specification ($N = 4$), the utilization is shown below:

|  Name   |  DATAFLOW   | PIPELINE  |
|  ----  | ----  | ----  |
| LUT  | 2113 | 1227 |
| FF  | 3182 | 859 |
| DSP  | 7 | 4 |
| BRAM  | 7 | 4 |

Clearly, the pipelined structure uses fewer resoucres. Notice that the reason that the DSP and BRAM required in DATAFLOW is 7 (4 + 3) rather than 8 (4 + 4) is the first result ($y[0]$) can be calculated out directly in the first loop (load_x_loop), so the second loop only requires 3 PEs.
