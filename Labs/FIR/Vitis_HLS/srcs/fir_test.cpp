/*
	Filename: fir_test.h
		FIR lab wirtten for WES/CSE237C class at UCSD.
		Testbench file
		Calls fir() function from fir.cpp
		Compares the output from fir() with out.gold.dat
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "fir.h"

int main () {
  const int    SAMPLES=600;
  FILE         *fp, *fin;

  data_t signal;
  acc_t output;
  d_in_stream signal_stream;
  d_out_stream result_stream;
  int i;
  signal = 0;

  fin=fopen("input.dat","r");
  fp=fopen("out.dat","w");
  printf("%10s%10s%10s%10s\n", "Index", "Input", "Output", "TLAST");

  for (i=0;i<SAMPLES;i++) {
	int temp;
	fscanf(fin,"%d",&temp);
	data_t_pack signal_pack;
	signal_pack.data = (data_t)temp;
	signal_pack.keep = -1;
	signal_pack.last = (i == (SAMPLES - 1));

	signal_stream << signal_pack;

	//Call the HLS block
    fir(result_stream,signal_stream);

    acc_t_pack result_pack;
    result_stream >> result_pack;
    // Save the results.
    fprintf(fp,"%d\n",(int)result_pack.data);
    printf("%10d%10d%10d%10d\n",i,signal,int(result_pack.data), (int)result_pack.last);
    if (result_pack.last != signal_pack.last){
    	printf("Tlast signal error!\n");
    	return 2;
    }
  }

  fclose(fp);
  fclose(fin);

  //Comparing results with the golden output.
  printf ("Comparing against output data \n");
    if (system("diff -w out.dat out.gold.dat")) {
  	fprintf(stdout, "*******************************************\n");
  	fprintf(stdout, "FAIL: Output DOES NOT match the golden output\n");
  	fprintf(stdout, "*******************************************\n");
       return 1;
    } else {
  	fprintf(stdout, "*******************************************\n");
  	fprintf(stdout, "PASS: The output matches the golden output!\n");
  	fprintf(stdout, "*******************************************\n");
       return 0;
    }

}

