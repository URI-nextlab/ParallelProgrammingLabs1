/*
Filename: cordic.h
	Header file
	CORDIC lab
*/
#ifndef CORDIC_H_
#define CORDIC_H_

#include "hls_stream.h"
#include "ap_axi_sdata.h"
#include "ap_fixed.h"

#define PI (3.14159265f)

const int NUM_ITERATIONS = 28;
const float INIT_X = 0.60735;

typedef ap_fixed<32,8> THETA_TYPE;
typedef ap_fixed<32,2>	COS_SIN_TYPE;

typedef hls::axis<THETA_TYPE,0,0,0> theta_t_pack;
typedef hls::stream<theta_t_pack> theta_t_stream;
typedef hls::axis<COS_SIN_TYPE,0,0,0> cos_sin_t_pack;
typedef hls::stream<cos_sin_t_pack> cos_sin_t_stream;

void cordic (
  cos_sin_t_stream& sin_stream,
  cos_sin_t_stream& cos_stream,
  theta_t_stream& theta_stream
);

#endif
