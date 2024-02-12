#include "cordic.h"

// Not optimzied code in Figure 3.3

void cordic (
  cos_sin_t_stream& sin_stream,
  cos_sin_t_stream& cos_stream,
  theta_t_stream& theta_stream
){
#pragma HLS INTERFACE mode=ap_ctrl_none port=return
#pragma HLS INTERFACE mode=axis register_mode=both port=sin_stream
#pragma HLS INTERFACE mode=axis register_mode=both port=cos_stream
#pragma HLS INTERFACE mode=axis register_mode=both port=theta_stream
#pragma HLS PIPELINE style=frp
    static THETA_TYPE cordic_phase[NUM_ITERATIONS] = {
    		0.125000000000000000,
    		0.073791808825216645,
    		0.038989565188684662,
    		0.019791712080282773,
    		0.009934262152770421,
    		0.004971973911794637,
    		0.002486593639475207,
    		0.001243372696834870,
    		0.000621695834357050,
    		0.000310849102961686,
    		0.000155424699705010,
    		0.000077712368380566,
    		0.000038856186506292,
    		0.000019428093542647,
    		0.000009714046807511,
    		0.000004857023408279,
    		0.000002428511704705,
    		0.000001214255852423,
    		0.000000607127926220,
    		0.000000303563963111,
    		0.000000151781981556,
    		0.000000075890990778,
    		0.000000037945495389,
    		0.000000018972747694,
    		0.000000009486373847,
    		0.000000004743186924,
    		0.000000002371593462,
    		0.000000001185796731
    };

    theta_t_pack theta_t_pack_temp;
    theta_stream >> theta_t_pack_temp;
    THETA_TYPE theta = theta_t_pack_temp.data;c
    theta = theta * MAP_K;
    // handle negative phase
    if (theta < 0){
    	theta = (THETA_TYPE)-theta;
    	theta(30,16) = 0;
    	theta = 1 - theta;
    }else{
    	theta(30,16) = 0;
    }

    if (theta < 0.25){
    	theta = theta;
    }else if(theta < 0.5){
    	theta = (THETA_TYPE)0.5 - theta;
    	inv_cos = true;
    }else if (theta < 0.75){
    	theta = theta - (THETA_TYPE)0.5;
    	inv_cos = true;
    	inv_sin = true;
    }else if (theta < 1){
    	theta = (THETA_TYPE)1 - theta;
    	inv_sin = true;
    }

    COS_SIN_TYPE current_cos = INIT_X;
    COS_SIN_TYPE current_sin = 0.0;


ROTATION_LOOP:
    for (int j = 0; j < NUM_ITERATIONS; j++){
        COS_SIN_TYPE cos_shift = current_cos >> j;
        COS_SIN_TYPE sin_shift = current_sin >> j;

        if (theta >= 0){
            current_cos = current_cos - sin_shift;
            current_sin = current_sin + cos_shift;
            theta -= cordic_phase[j];
        }
        else{
            current_cos = current_cos + sin_shift;
            current_sin = current_sin - cos_shift;
            theta += cordic_phase[j];
        }
    }

    cos_sin_t_pack cos_temp, sin_temp;
    cos_temp.data = inv_cos?((COS_SIN_TYPE)-current_cos):current_cos;
    sin_temp.data = inv_sin?((COS_SIN_TYPE)-current_sin):current_sin;
    cos_temp.keep = -1;
    sin_temp.keep = -1;
    cos_temp.last = theta_t_pack_temp.last;
    sin_temp.last = theta_t_pack_temp.last;

    sin_stream << sin_temp;
    cos_stream << cos_temp;
   
}

