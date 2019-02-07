#include <stdio.h>
#include "platform.h"
#include "xparameters.h"
#include "xtime_l.h"
#include <stdlib.h>
#include "xil_printf.h"
#include "xarray_mult.h"
#include "xarray_mult_p.h"


/* uncomment to see each stage of multiplication */
//#define DEBUG

#define SIZE 5
#define MAX_VAL 10
#define MIN_VAL 0

void generate(int *array)
{
  int i, j;

  for (i = 0; i < SIZE; i++)
  {
    for (j = 0; j < SIZE; j++)
    {
      /* populate rows and columns with rand() values */
      array[i*SIZE+j] = rand() % (MAX_VAL + 1 - MIN_VAL) + MIN_VAL;
      xil_printf("%d\t", array[i*SIZE+j]);
    }
    xil_printf("\n\r");
  }
  xil_printf("\n\r");
};

void multiply(int *array_x, int *array_y, int *array_z)
{
  int i, j, k;

  for (i = 0; i < SIZE; i++)
  {
    for (j = 0; j < SIZE; j++)
    {
      array_z[i*SIZE+j] = 0;
      for (k = 0; k < SIZE; k++)
      {
        array_z[i*SIZE+j] += (array_x[i*SIZE+k] * array_y[k*SIZE+j]);

#if defined(DEBUG)
        xil_printf("z: %d = x: %d * y %d\n\r", array_x[i][j], array_x[i][k], array_y[k][j]);
#endif
      }
    }
  }
};

int main()
{
  XTime tSeed, tStart_sw, tEnd_sw;
  XTime tStart_hw, tEnd_hw;
  XTime tStart_hw2, tEnd_hw2;
  init_platform();

  xil_printf("Seeding rand()...\n\r");

  /* use start time as a seed for rand() */
  XTime_GetTime(&tSeed);
  srand(tSeed);

  int rows = SIZE, cols = SIZE, i, j;
  int *x, *y, *z, *hw_res;
  int *hw_res2;

  /* allocate the array in memory */
  x = malloc(rows*cols * sizeof *x);
  y = malloc(rows*cols * sizeof *y);
  z = malloc(rows*cols * sizeof *z);

  hw_res= malloc(rows*cols * sizeof *hw_res);
  hw_res2= malloc(rows*cols * sizeof *hw_res2);


  xil_printf("Generating Matrices X, Y & Z...\n\n\r");

  xil_printf("Matrix X\n\r");
  generate(x);

  xil_printf("Matrix Y\n\r");
  generate(y);

  xil_printf("Multiplying X & Y and starting timer...\n\n\r");

  /* get time at start of multiplication */
  XTime_GetTime(&tStart_sw);

  /* no print statements within this function */
  multiply(x, y, z);

  /* get time at end of multiplication */
  XTime_GetTime(&tEnd_sw);
  xil_printf("Stopping timer...\n\n\r");

  int acc_sw=0;
  xil_printf("Result Matrix Z\n\r");

  for (i = 0; i < SIZE; i++)
  {
    for (j = 0; j < SIZE; j++)
    {
      xil_printf("%d\t", z[i*SIZE+j]);
      acc_sw+=z[i*SIZE+j];

    }
    xil_printf("\n\r");
  }



  //ADDED CODE FOR THE UNOPTIMIZED ACCELERATOR
  XArray_mult array_mult_hw; //instantiation struct for the unoptmized accelerator
  XArray_mult_Config *array_mult_hw_cfg; //configuration struct pointer for the ununoptmized accelerator
  int status;

  array_mult_hw_cfg=XArray_mult_LookupConfig(XPAR_ARRAY_MULT_0_DEVICE_ID);//loads the configuration of the accelerator
  if (!array_mult_hw_cfg){//checks that the configuration has been loaded
	  xil_printf("Error loading the configuration of the accelerator\n\r");
  }
  status=XArray_mult_CfgInitialize(&array_mult_hw,array_mult_hw_cfg);//initilizes the accelerator
  if (status!= XST_SUCCESS){//checks that the accelerator has been initialized
	  xil_printf("Error initializing the accelerator\n\r");
  }





  XArray_mult_Write_in_a_Bytes(&array_mult_hw,0,x,sizeof(x)*(rows*cols));//sends the input array x

  XArray_mult_Write_in_b_Bytes(&array_mult_hw,0,y,sizeof(y)*(rows*cols));//sends the input array y

  xil_printf("Starting the timer for the unoptmized accelerator \n\r");
  XTime_GetTime(&tStart_hw);//stores start time


  XArray_mult_Start(&array_mult_hw);//starts the accelerator


  while(!XArray_mult_IsDone(&array_mult_hw));//waiting for the computation to end
  XTime_GetTime(&tEnd_hw);//end time
  xil_printf("Unoptimized accelerator has finished processing \n\r");

  XArray_mult_Read_res_Bytes(&array_mult_hw,0,hw_res,sizeof(hw_res)*(rows*cols));//reads the output of the accelerator

  int error_flag=0;
  for (i = 0; i < SIZE; i++){
    for (j = 0; j < SIZE; j++){
      xil_printf("%d\t", hw_res[i*SIZE+j]); //prints the result from the unoptimized accelerator
      if (hw_res[i*SIZE+j]!=z[i*SIZE+j]){//checks the result from the unoptimized accelerator and the expected one
    	  error_flag=1;
      }
    }
    xil_printf("\n\r");
  }


  //ADDED CODE FOR OPTIMIZED ACCELERATOR
  XArray_mult_p array_mult_p_hw;//instantiation struct for the unoptmized accelerator
  XArray_mult_p_Config *array_mult_p_hw_cfg;//configuration struct pointer for the ununoptmized accelerator


  array_mult_p_hw_cfg=XArray_mult_p_LookupConfig(XPAR_ARRAY_MULT_P_0_DEVICE_ID);//loads the configuration of the accelerator
  if (!array_mult_p_hw_cfg){//checks that the configuration has been loaded
	  xil_printf("Error loading the configuration of the accelerator 2\n\r");
  }
  status=XArray_mult_p_CfgInitialize(&array_mult_p_hw,array_mult_p_hw_cfg);//initilizes the accelerator
  if (status!= XST_SUCCESS){//checks that the accelerator has been initialized
	  xil_printf("Error initializing the accelerator 2\n\r");
  }


  XArray_mult_p_Write_in_a_Bytes(&array_mult_p_hw,0,x,sizeof(x)*(rows*cols));//sends the input array x


  XArray_mult_p_Write_in_b_Bytes(&array_mult_p_hw,0,y,sizeof(y)*(rows*cols));//sends the input array y

  xil_printf("Starting the timer for the optimized accelerator\n\r");
  XTime_GetTime(&tStart_hw2);//stores start time
  XArray_mult_p_Start(&array_mult_p_hw);//starts the accelerator



  while(!XArray_mult_p_IsDone(&array_mult_p_hw));//waiting for the computation to end
  XTime_GetTime(&tEnd_hw2);//end time

  XArray_mult_p_Read_res_Bytes(&array_mult_p_hw,0,hw_res2,sizeof(hw_res2)*(rows*cols));//reads the output of the accelerator


  xil_printf("After the while loop\n\r");

  xil_printf("After reading the data back\n\r");



  xil_printf("Result Matrix hw 2\n\r");
  xil_printf("Here1\n\r");

  int error_flag2=0;

  for (i = 0; i < SIZE; i++){
    for (j = 0; j < SIZE; j++){
      xil_printf("%d\t", hw_res2[i*SIZE+j]);//prints the result from the optimized accelerator
      if (hw_res2[i*SIZE+j]!=z[i*SIZE+j]){//checks the result from the optimized accelerator and the expected one
    	  error_flag2=1;
      }
    }
    xil_printf("\n\r");
  }




  if (error_flag){
	  xil_printf("The arrays do NOT match: Unoptimized accelerator\n\r");
  } else {
	  xil_printf("The arrays match: Unoptimized accelerator\n\r");
  }


  if (error_flag2){
	  xil_printf("The arrays do NOT match: Optimized accelerator\n\r");
  } else {
	  xil_printf("The arrays match: Optimized accelerator\n\r");
  }
  /* deallocate the array from memory */
  free(x);
  free(y);



  /* 2 clock cycles per unit of time */
#if !defined(DEBUG)
  printf("\nMultiplication of X & Y in SW took %llu clock cycles.\n", 2 * (tEnd_sw - tStart_sw));
  printf("Multiplication of X & Y in SW took %.3f us.\n", 1.0 * (tEnd_sw - tStart_sw) / (COUNTS_PER_SECOND / 1000000));

  printf("\nMultiplication of X & Y in HW took %llu clock cycles.\n", 2 * (tEnd_hw - tStart_hw));
  printf("Multiplication of X & Y in HW took %.3f us.\n", 1.0 * (tEnd_hw - tStart_hw) / (COUNTS_PER_SECOND / 1000000));


  printf("\nMultiplication of X & Y in HW2 took %llu clock cycles.\n", 2 * (tEnd_hw2 - tStart_hw2));
  printf("Multiplication of X & Y in HW2 took %.3f us.\n", 1.0 * (tEnd_hw2 - tStart_hw2) / (COUNTS_PER_SECOND / 1000000));
#else
  /* printing text over serial is clock cycle expensive and introduces additional overhead */
  printf("\nMultiplication was run under DEBUG, timer is no longer accurate.\n\r");
#endif
  cleanup_platform();
  return 0;
}
