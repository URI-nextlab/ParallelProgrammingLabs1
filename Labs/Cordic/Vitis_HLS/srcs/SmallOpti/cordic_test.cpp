/*
  Filename: cordic_test.h
  CORDIC lab wirtten for WES/CSE237C class at UCSD.
  Testbench file
  Calls cordic() function from cordic.cpp
  Compares the output from cordic() with math library
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "cordic.h"



int main (int argc, char** argv) {
  printf("Cordic rotation mode test:\n");
  cos_sin_t_stream sin_stream;
  cos_sin_t_stream cos_stream;
  theta_t_stream theta_stream;
  THETA_TYPE theta;

  float acc_error = 0;
  printf("Angle\t\t\tsin\t\t\tsin_gold\tcos\t\t\tcos_gold\terror\t\t\tacc_error\n");
  for (int i = 0; i < 256; i++){
    theta = (i - 128) * PI / 16 / 2; // 16 steps from 0 to pi/2
    theta_t_pack theta_t_pack_temp;
    theta_t_pack_temp.data = theta;
    theta_t_pack_temp.keep = -1;
    theta_t_pack_temp.last = (i == 15);
    theta_stream << theta_t_pack_temp;

    cordic(sin_stream, cos_stream, theta_stream);

    cos_sin_t_pack sin_pack, cos_pack;
    sin_stream >> sin_pack;
    cos_stream >> cos_pack;
    float sin_golden = sin((float)theta);
    float cos_golden = cos((float)theta);

    float new_error = pow((float)cos_pack.data-(float)cos_golden,2) + pow((float)sin_pack.data-(float)sin_golden,2);
    acc_error += new_error;

    printf("%3.3f    :\t\t%1.4f\t\t%1.4f\t\t%1.4f\t\t%1.4f\t\t%1.8f\t\t%1.8f\n",(float)theta,(float)sin_pack.data,(float)sin_golden,(float)cos_pack.data,(float)cos_golden,new_error, acc_error);
  }
  if(acc_error / 256 < 0.0001){
	  printf(" +---------------------+\n");
	  printf(" |        PASS!        |\n");
	  printf(" +---------------------+\n");
	  printf("Mean error = %.8f\n",acc_error / 16);
	  return 0;
  }
  else{
	  printf(" +---------------------+\n");
	  printf(" |        FAIL!        |\n");
	  printf(" +---------------------+\n");
	  printf("Mean error = %.8f\n",acc_error / 16);
	  return -1;
  }
}

