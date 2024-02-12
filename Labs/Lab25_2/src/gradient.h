#ifndef __GRADIENT_H__
#define __GRADIENT_H__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ap_fixed.h>

//--------------------------------------------------------------------------
// Compiler Defines
//--------------------------------------------------------------------------
#ifdef HLS_SIM
    #include "config.h"
#endif

//--------------------------------------------------------------------------
// Type Conversions
//--------------------------------------------------------------------------
#ifdef  CSIM_DEBUG
    typedef float fm_t;
    typedef float wt_t;
    typedef float fm_t;
    typedef float mk_t;
    
#else
    typedef ap_fixed<16,2> fm_t;
    typedef ap_fixed<16,2> wt_t;
    typedef ap_fixed<16,2> fm_t;
    typedef ap_int<2>  mk_t; // mask bits
#endif
//--------------------------------------------------------------------------
// Function Declarations
//--------------------------------------------------------------------------

#define IM_SIZE 32

#define IN_BUF_DEPTH 8
#define OUT_BUF_DEPTH 8

#define OUT_BUF_HEIGHT 8
#define OUT_BUF_WIDTH 8
#define IN_BUF_HEIGHT OUT_BUF_HEIGHT + 2
#define IN_BUF_WIDTH OUT_BUF_WIDTH + 2

#define BLOCK_SIZE_M 16
#define BLOCK_SIZE_N 16


void tiled_conv(
    fm_t fixp_layer1_ifmap[3][IM_SIZE][IM_SIZE],        // input image
    fm_t fixp_layer2_ifmap[32][IM_SIZE][IM_SIZE],       // output of conv2
    wt_t fixp_conv1_weights[32][3][3][3],
    wt_t fixp_conv1_bias[32],
    wt_t fixp_conv2_weights[32][32][3][3],
	wt_t fixp_conv2_bias[32]


);




#endif
