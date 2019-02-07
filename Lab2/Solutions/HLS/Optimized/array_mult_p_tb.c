#include "array_mult_p.h"
#include <time.h>

void generate(int array[ROWS][COLS])
{
  int i, j;

  for (i = 0; i < SIZE; i++)
  {
    for (j = 0; j < SIZE; j++)
    {
      /* populate rows and columns with rand() values */
      array[i][j] = rand() % (MAX_VAL + 1 - MIN_VAL) + MIN_VAL;
      printf("%d\t", array[i][j]);
    }
    printf("\n\r");
  }
  printf("\n\r");
};

void print_array(int array[ROWS][COLS]){
  int i, j;

  for (i = 0; i < SIZE; i++){
	    for (j = 0; j < SIZE; j++){
	      printf("%d\t", array[i][j]);
	    }
	    printf("\n\r");
	  }
	  printf("\n\r");
};

int main(){
	int i,j,k;
	int mult_acc;
	int error_flag=0;
	int in_a_tb[ROWS][MULT_ACC],in_b_tb[MULT_ACC][COLS],mult_func_exp[ROWS][COLS];
	int mult_func_tb[ROWS][COLS];
	time_t t;
	t=1549294172; //fixed random seed for the RNG
	srand((unsigned) t);
	printf("Random seed: %ld\n",t);

	generate(in_a_tb);
	generate(in_b_tb);

  array_mult_p(in_a_tb,in_b_tb,mult_func_tb);//calls the function in the source file

	int acc_tb,acc_exp;
	acc_exp=0;
  //calculates the expected result and makes the comparison with the one from the source file.
	ROWS_LOOP: for (i=0;i<ROWS;i++){
		COLS_LOOP: for (j=0;j<COLS;j++){
			mult_acc=0;
			MULT_ACC_LOOP: for (k=0;k<MULT_ACC;k++){
				mult_acc+=in_a_tb[i][k]*in_b_tb[k][j];

			}
			mult_func_exp[i][j]=mult_acc;
			if (mult_func_exp[i][j]!=mult_func_tb[i][j]){
				error_flag=1;
			}
		}
	}



	printf("Expected array:\n");
	print_array(mult_func_exp);
	printf("Array accelerator:\n");
  print_array(mult_func_tb);


	 if (error_flag==0){
		 return 0;
	 } else {
		 return 1;
	 }



}
