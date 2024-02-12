//--------------------------------------------------------------------------
// Test bench for your convolution codes.
//
// You should not need to modify this, except for debugging.
//
// Remove any print statements in your submission codes!
//--------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <cmath>

#include "gradient.h"
#include "io.h"
#include "cmodel/fp.h"
#include "cmodel/bp_single_class.h"

using namespace std;

//--------------------------------------------------------------------------
// Set up the global variables for all the layers
//--------------------------------------------------------------------------

// float versions for csim

float layer1_ifmap[3][IM_SIZE][IM_SIZE];        // input image

fm_t fixp_layer1_ifmap[3][IM_SIZE][IM_SIZE];        // input image
fm_t fixp_layer2_ifmap[32][IM_SIZE][IM_SIZE];       // output of conv1
                       // output of fc2
fm_t fixp_layer3_ifmap[32][IM_SIZE][IM_SIZE];
// fixed point versions of above variables

float conv1_weights[32][3][3][3];
float conv1_bias[32];


wt_t fixp_conv1_weights[32][3][3][3];
wt_t fixp_conv1_bias[32];


//--------------------------------------------------------------------------
// Read the reference files into test bench arrays
//--------------------------------------------------------------------------

void read_bin_files()
{
    read_input_feature <3,IM_SIZE,IM_SIZE> (layer1_ifmap);

    read_conv_weight <32,3,3,3> ("conv_layer1_weights.bin", conv1_weights);
    read_conv_bias <32> ("conv_layer1_bias.bin", conv1_bias);

}

//--------------------------------------------------------------------------
// Convert the data types of every array element for specified 
// configuration.
//--------------------------------------------------------------------------

void convert_type()
{
    convert_input_3d <3,IM_SIZE,IM_SIZE> (layer1_ifmap, fixp_layer1_ifmap);

    convert_conv_layer_params <3,32> (
        conv1_weights,
        conv1_bias,
        fixp_conv1_weights,
        fixp_conv1_bias
    );


}   

void cmodel_conv_fp(){
    model_conv <32,3,IM_SIZE,IM_SIZE> (
        fixp_layer1_ifmap,
        fixp_conv1_weights,
        fixp_conv1_bias,
        fixp_layer3_ifmap
    );


}
//--------------------------------------------------------------------------
// This is where fun begins.
//--------------------------------------------------------------------------
int main ()
{
    read_bin_files();
    
    convert_type();
//    cmodel_conv_fp();
        model_conv(
            fixp_layer1_ifmap,
            fixp_conv1_weights,
            fixp_conv1_bias,
            fixp_layer3_ifmap);
    cout << "Beginning HLS tiled-convolution simulation..." << std::endl;
        tiled_conv(
            fixp_layer1_ifmap,
            fixp_layer2_ifmap,
            fixp_conv1_weights,
            fixp_conv1_bias

        );
    cout << "Tiled-convolution simulation complete!\n" << std::endl;
    long double mse = 0.0;
       for(int f = 0; f < 32; f++)
        {
            for(int i = 0; i < 32; i++)
            {
                for(int j = 0; j < 32; j++)
                {
                    mse += std::pow((float(fixp_layer3_ifmap[f][i][j])
                                     -float(fixp_layer2_ifmap[f][i][j])), 2);
                }
            }
        }
       mse = mse / (32 * 32 * 32);

         std::cout << "\nOutput MSE:  " << mse << std::endl;
    return 0;
}





