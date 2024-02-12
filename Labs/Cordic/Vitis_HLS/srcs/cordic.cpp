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
        0.78539816339745,   0.46364760900081,   0.24497866312686,   0.12435499454676,
        0.06241880999596,   0.03123983343027,   0.01562372862048,   0.00781234106010,
        0.00390623013197,   0.00195312251648,   0.00097656218956,   0.00048828121119,
        0.00024414062015,   0.00012207031189,   0.00006103515617,   0.00003051757812,
        0.00001525878906,   0.00000762939453,   0.00000381469727,   0.00000190734863,
        0.00000095367432,   0.00000047683716,   0.00000023841858,   0.00000011920929,
        0.00000005960464,   0.00000002980232,   0.00000001490116,   0.00000000745058
    };

    theta_t_pack theta_t_pack_temp;
    theta_stream >> theta_t_pack_temp;
    THETA_TYPE theta = theta_t_pack_temp.data;

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
    cos_temp.data = current_cos;
    sin_temp.data = current_sin;
    cos_temp.keep = -1;
    sin_temp.keep = -1;
    cos_temp.last = theta_t_pack_temp.last;
    sin_temp.last = theta_t_pack_temp.last;

    sin_stream << sin_temp;
    cos_stream << cos_temp;
   
}

