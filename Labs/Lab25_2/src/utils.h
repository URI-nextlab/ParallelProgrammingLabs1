#include "gradient.h"

//--------------------------------------------------------------------------
// Function to load input feature map tile block from DRAM to on-chip BRAM.
//--------------------------------------------------------------------------
template <int inp_channel, int inp_height, int inp_width>
void load_input_tile_block_from_DRAM (
    fm_t in_fm_buf[IN_BUF_DEPTH][IN_BUF_HEIGHT][IN_BUF_WIDTH], 
    fm_t in_fm[inp_channel][inp_height][inp_width], 
    int  ti, 
    int  tj, 
    int  d
)
{
    #pragma HLS inline off
    
    int IN_BUF_DEPTH_MUX;
    if (inp_channel < IN_BUF_DEPTH)
        IN_BUF_DEPTH_MUX = inp_channel;
    else
        IN_BUF_DEPTH_MUX = IN_BUF_DEPTH;

    const int depth_offset  =  d * IN_BUF_DEPTH;
    const int height_offset = ti * OUT_BUF_HEIGHT; // OUT_BUF is intended, not a typo. 
    const int width_offset  = tj * OUT_BUF_WIDTH;
        
    INPUT_BUFFER_DEPTH:
    for(int c = 0; c < IN_BUF_DEPTH_MUX; c++)
    {
        INPUT_BUFFER_HEIGHT:
        for(int i = 0; i < IN_BUF_HEIGHT; i++)
        {
            INPUT_BUFFER_WIDTH:
            for(int j = 0; j < IN_BUF_WIDTH; j++)
            {
                // TODO: Handle border features here
		        if ( height_offset+i-1 >= 0 && height_offset+i-1 < inp_height    &&    width_offset + j -1 >=0 && width_offset + j -1 < inp_width )
                {
                    in_fm_buf[c][i][j] = in_fm[depth_offset + c][height_offset + i - 1][width_offset + j - 1];
                }
                else
                    in_fm_buf[c][i][j]  = 0;
            }
        }
    }
}

//--------------------------------------------------------------------------
// Function to load layer parameters (weights and bias) for convolution.
//--------------------------------------------------------------------------
template <int out_channel, int inp_channel>
void load_conv_layer_params_from_DRAM(
    wt_t weight_buf[OUT_BUF_DEPTH][IN_BUF_DEPTH][3][3],
    wt_t bias_buf[OUT_BUF_DEPTH],
    wt_t weights[out_channel][inp_channel][3][3],
    wt_t bias[out_channel],
    int b,
    int d
)
{
    #pragma HLS inline off

    int IN_BUF_DEPTH_MUX;
    if (inp_channel < IN_BUF_DEPTH)
        IN_BUF_DEPTH_MUX = inp_channel;
    else
        IN_BUF_DEPTH_MUX = IN_BUF_DEPTH;

    const int kernel_offset  = b * OUT_BUF_DEPTH;
    const int channel_offset = d * IN_BUF_DEPTH;

    WEIGHT_KERNEL_NUM:
    for(int f = 0; f < OUT_BUF_DEPTH; f++)
    {
        WEIGHT_KERNEL_DEPTH:
        for(int c = 0; c < IN_BUF_DEPTH_MUX; c++)
        {
            WEIGHT_KERNEL_HEIGHT:
            for(int kh = 0; kh < 3; kh++)
	        {
                WEIGHT_KERNEL_WIDTH:
	            for(int kw = 0; kw < 3; kw++)
	            {
	                weight_buf[f][c][kh][kw] = weights[kernel_offset + f][channel_offset + c][kh][kw];
                }
            }
        }
    }
    
    BIAS:
    for(int f = 0; f < OUT_BUF_DEPTH; f++)
    {
        bias_buf[f] = bias[kernel_offset + f];
    }
}


//---------------------------------------------------------------------------
// Perform synthesizable tiling-based convolution for a single tile.
//---------------------------------------------------------------------------

void conv_3x3 (
    fm_t Y_buf[OUT_BUF_DEPTH][OUT_BUF_HEIGHT][OUT_BUF_WIDTH], 
    fm_t X_buf[IN_BUF_DEPTH][IN_BUF_HEIGHT][IN_BUF_WIDTH],
    wt_t W_buf[OUT_BUF_DEPTH][IN_BUF_DEPTH][3][3],
    wt_t bias_buf[OUT_BUF_DEPTH],
    int  d,
    int inp_channel,
    int out_channel
)
{
    if(d==0){ 
        init:
        for(int oh=0; oh<OUT_BUF_HEIGHT; oh++){
            // #pragma HLS pipeline
                
            for(int ow=0; ow<OUT_BUF_WIDTH; ow++){
                // #pragma HLS unroll
        
                for(int oc=0; oc<OUT_BUF_DEPTH; oc++){
                    // #pragma HLS unroll
                    Y_buf[oc][oh][ow]   = bias_buf[oc];
                }
            }
        }
    }

    int IN_BUF_DEPTH_MUX;
    if (inp_channel < IN_BUF_DEPTH)
        IN_BUF_DEPTH_MUX = inp_channel;
    else
        IN_BUF_DEPTH_MUX = IN_BUF_DEPTH;

    int OUT_BUF_DEPTH_MUX;
    if (out_channel < OUT_BUF_DEPTH)
        OUT_BUF_DEPTH_MUX = out_channel;
    else
        OUT_BUF_DEPTH_MUX = OUT_BUF_DEPTH;

    ip_chan:
    for(int ic=0; ic<IN_BUF_DEPTH_MUX; ic++){
        fh:
        for(int fh=0; fh<3; fh++){
            fw:
            for(int fw=0; fw<3; fw++){
                
                op_chan:
                for(int oc=0; oc<OUT_BUF_DEPTH_MUX; oc++){
                    // #pragma HLS pipeline
                    
                    op_h:
                    for(int oh=0; oh<OUT_BUF_HEIGHT; oh++){
                        // #pragma HLS unroll  
                        #pragma HLS pipeline

                        op_w:
                        for(int ow=0; ow<OUT_BUF_WIDTH; ow++){
                            // #pragma HLS unroll
                            Y_buf[oc][oh][ow]   += X_buf[ic][oh+1+fh-1][ow+1+fw-1] * W_buf[oc][ic][fh][fw];
                        }
                    }
                }
            }
        }
    }

}

//--------------------------------------------------------------------------
// Function to store complete output tile block from BRAM to DRAM.
//--------------------------------------------------------------------------
template <int out_channel, int inp_height, int inp_width>
void store_output_tile_to_DRAM (
    fm_t out_fm[out_channel][inp_height][inp_width], 
    fm_t out_fm_buf[OUT_BUF_DEPTH][OUT_BUF_HEIGHT][OUT_BUF_WIDTH], 
    int  ti,
    int  tj,
    int  b,
    bool relu_enable
)
{
    #pragma HLS inline
    const int depth_offset  =  b * OUT_BUF_DEPTH;
    const int height_offset = ti * OUT_BUF_HEIGHT;
    const int width_offset  = tj * OUT_BUF_WIDTH;

    int OUT_BUF_DEPTH_MUX = OUT_BUF_DEPTH;
    
    if(out_channel < OUT_BUF_DEPTH)
        OUT_BUF_DEPTH_MUX = out_channel;

    OUTPUT_BUFFER_DEPTH:
    for(int f = 0; f < OUT_BUF_DEPTH_MUX; f++)
    {
        OUTPUT_BUFFER_HEIGHT:
        for(int i = 0; i < OUT_BUF_HEIGHT; i++)
        {
            OUTPUT_BUFFER_WIDTH:
            for(int j = 0; j < OUT_BUF_WIDTH; j++)
            {
                // ReLU in-place
                if(out_fm_buf[f][i][j] < (fm_t) 0 && relu_enable)
                {
                    out_fm[depth_offset + f][height_offset + i][width_offset + j] = (fm_t) 0;
                }
                else
                {
                    out_fm[depth_offset + f][height_offset + i][width_offset + j] = out_fm_buf[f][i][j];
                }
            }
        }
    }
}

