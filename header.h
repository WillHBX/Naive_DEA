#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gurobi_c.h"
#include <stdbool.h>
#include <mkl.h>
#include <time.h>
#include <Windows.h>

typedef struct {
	double** matrix;
	size_t x_row;
	size_t y_row;
	size_t col;
} MATRIX;



void readXY(MATRIX* A,char* filename);

int new_model(GRBenv* env, GRBmodel** model, MATRIX* A);

int addEqConstraint(GRBmodel* model, MATRIX* A);
int addXConstraint(GRBmodel* model, MATRIX* A);
int addYConstraint(GRBmodel* model, MATRIX* A);
int init_constraints(GRBmodel* model, MATRIX* A);

int optimize_all(GRBmodel* model, MATRIX* A, double* objval);
int optimize_partOf(GRBmodel* model, MATRIX* A, double* objval, size_t *list, size_t list_len);

void optimizer(double** objval, MATRIX *A, size_t* list, size_t list_len);