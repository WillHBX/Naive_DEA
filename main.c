/* Copyright 2021, Gurobi Optimization, LLC */

/* This example formulates and solves the following simple MIP model:

     maximize    x +   y + 2 z
     subject to  x + 2 y + 3 z <= 4    ->  - x - 2 y - 3 z >= -4
                 x +   y       >= 1
                 x, y, z binary
*/

/*
     k is a variable matrix

     maximize ck          ->  c = [1 1 2]
                          ->  k = [ x |
                                  | y |
                                  | z ]

     constraint Az >= b   ->  A = [ -1 -2 -3 |
                                  |  1  1  0 ]

                          ->  b = [ -4 |
                                  |  1 ]
*/


#include "header_2_3.h"

// change some important parameter
#define INPUT_FILENAME "test.csv"
#define OUTPUT_FILENAME "QAQ.csv"
#define MAXFILELEN 30
#define X_ROW 2
#define Y_ROW 3
#define COL 3000
#define LEN_OF_LIST 3000

int main(int argc, char* argv[])
{
    double* objval = NULL;
    clock_t t1, t2;
    MATRIX a;
    MATRIX* A = &a;
    A->matrix = NULL;
    A->col = COL;
    A->x_row = X_ROW;
    A->y_row = Y_ROW;

    /* Set the list or runs for all cases */
    size_t* list = NULL;
    size_t list_len = LEN_OF_LIST;


    readXY(A, INPUT_FILENAME);
    objval = (double*)malloc(sizeof(double) * (list_len));

    t1 = clock();
    optimizer( &objval, A, list, list_len);
    t2 = clock();

    FILE* outFile;
    fopen_s(&outFile, OUTPUT_FILENAME, "w"); // output file

    fprintf(outFile,"time: %lf\n", (t2 - t1) / (double)(CLOCKS_PER_SEC));

    for (size_t i = 0; i <list_len; ++i)
        fprintf(outFile, "%.6f\n", objval[i]);
    fclose(outFile);


    for (size_t i = 0; i < A->x_row + A->y_row; ++i)
        free(a.matrix[i]);
    free(a.matrix);
    a.matrix = NULL;
    free(objval);

    return 0;
}
