#include "array_mult.h"


void  array_mult(int in_a[ROWS][COLS],int in_b[ROWS][COLS],int res[ROWS][COLS]){
//Defines the ports as axilite
#pragma HLS INTERFACE s_axilite port=return bundle=CRTL

#pragma HLS INTERFACE s_axilite port=in_a bundle=DATA_IN
#pragma HLS INTERFACE s_axilite port=in_b bundle=DATA_IN
#pragma HLS INTERFACE s_axilite port=res  bundle=DATA_OUT

int i,j,k;
int mult_acc;

int acc=0;
ROWS_LOOP: for (i=0;i<ROWS;i++){//iterates through the rows
	COLS_LOOP: for (j=0;j<COLS;j++){//iterates through the columns
		mult_acc=0;
		MULT_ACC_LOOP: for (k=0;k<MULT_ACC;k++){//multiply and accumulation
			mult_acc+=in_a[i][k]*in_b[k][j];
		}
		res[i][j]=mult_acc;
	}
}


}
