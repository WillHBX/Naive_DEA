#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE  


#include "header_2_3.h"


#define MAX_LINE_SIZE 16384
#define ZERO 10e-8

int *ind;
double* eq;

void readXY(MATRIX* A, char* filename)
{
    /*---------------------------------------------------------------------+
    |   After this function, matrix A store data like the format below.    |
    |                                                                      |
    |               For example: X is 2*25 , Y is 3*25                     |
    |             +--                                  --+                 |
    |             |   0   | X(0,0)  X(0,1)  ...  X(0,24) |                 |
    |             |   0   | X(1,0)  X(1,1)  ...  X(1,24) |                 |
    |             | ------+----------------------------- |                 |
    |             |   0   | Y(0,0)  Y(0,1)  ...  Y(0,24) |                 |
    |             |   0   | Y(1,0)  Y(1,1)  ...  Y(1,24) |                 |
    |             |   0   | Y(2,0)  Y(2,1)  ...  Y(2,24) |                 |
    |             +--                                  --+                 |    
    +---------------------------------------------------------------------*/


    FILE* inputFile;
   
    errno_t err = fopen_s(&inputFile, filename, "r");
    if (err) {
        fprintf(stderr, "Fail to open the file.\n");
        exit(1);
    }

    char line[MAX_LINE_SIZE];
    char* result = NULL;
    
    ind = (int*)malloc(sizeof(int) * (A->col + 1));
    eq = (double*)malloc(sizeof(double) * (A->col));

    // declare dynamic array in row major
    A->matrix = (double**)malloc(sizeof(double*) * (A->x_row + A->y_row));
    for (size_t j = 0; j < A->x_row + A->y_row; ++j)
        A->matrix[j] = (double*)malloc(sizeof(double) * (A->col + 1));
    

    // reserve for Xr ( which will keep changing while adding constraint )
    for (size_t i = 0; i < A->x_row + A->y_row; ++i)
        A->matrix[i][0] = ZERO;
    
    
    
    // read -> split -> store
    /*--------------------------+
    | The format is [ X^T Y^T ] |
    +--------------------------*/
    char* next_token = NULL;
    size_t i = 1,j;
    double tmp;
    ind[0] = 0;
    while (fgets(line, MAX_LINE_SIZE, inputFile) != NULL) {
        result = strtok_s(line, ",", &next_token);
        tmp = atof(result);
        A->matrix[0][i] = (tmp==0) ? ZERO : tmp;
        j = 0;
        
        while (1) {
            result = strtok_s(NULL, ",", &next_token);
            if (result == NULL)
                break;
            tmp = atof(result);
            A->matrix[++j][i] = (tmp == 0) ? ZERO : tmp;
        }

        // initialize eq
        eq[i - 1] = 1;
        ind[i] = i;
        ++i;
    }
    
    for (size_t i = 0; i < A->x_row; ++i)
        for (size_t j = 1; j < A->col+1; ++j)
            A->matrix[i][j] *= -1;
    
    fclose(inputFile);
}

int addEqConstraint(GRBmodel* model, MATRIX* A)
{
    return GRBaddconstr(model, A->col, &ind[1], eq, GRB_EQUAL, 1, NULL);
    free(eq);
}

int addXConstraint(GRBmodel* model, MATRIX* A)
{
    int error = 0;
    for (size_t i = 0; i < A->x_row; ++i) {
        error = GRBaddconstr(model, A->col + 1, ind, A->matrix[i], GRB_GREATER_EQUAL, 0, NULL);
        if (error) break;
    }
    return error;
}

int addYConstraint(GRBmodel* model, MATRIX* A)
{
    int error = 0;
    for (size_t i = A->x_row; i < A->x_row + A->y_row; ++i) {
        error = GRBaddconstr(model, A->col, &ind[1], &(A->matrix[i][1]), GRB_GREATER_EQUAL, 1, NULL);
        if (error) break;
    }
    free(ind);
    return error;
}

int init_constraints(GRBmodel* model, MATRIX* A)
{
	// the order is important, DON'T CHANGE the order of functions below
	// the order of constrains is related to the position used when GRBchgcoeffs() and GRBsetdblattrelement()
    int error = 0;
    error = addEqConstraint(model, A);
    if (error) return error;
    error = addXConstraint(model, A);
    if (error) return error;
    error = addYConstraint(model, A);
    if (error) return error;
}

int optimize_all(GRBmodel* model, MATRIX* A,double *objval)
{
    int error = 0;
    int c = 0;
    int r,tmp = 100;
    int optimstatus;
    double* Xr = (double*)malloc(sizeof(double) * (A->x_row));
    

    // 執行次數在這
    for (size_t i = 1; i < A->col+1; ++i) {
        for (size_t j = 0; j < A->x_row; ++j)
            Xr[j] = A->matrix[j][i]*-1;

        for (r = 1; r < A->x_row + 1; ++r) {
            error = GRBchgcoeffs(model, 1, &r, &c, &Xr[r-1]);
            //if (error) return error;
        }

        for (size_t j = A->x_row + 1; j < A->x_row + A->y_row + 1; ++j) {
            error = GRBsetdblattrelement(model, "RHS", j, A->matrix[j - 1][i]);
            //if (error)  return error;
        }
        // Optimize model 
        error = GRBoptimize(model);
        //if (error)   return error;
        
        // get the best value
        error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval[i-1]);
        //if (error) return error;
    }
    free(Xr);
    return error;
}

int optimize_partOf(GRBmodel* model, MATRIX* A, double* objval, size_t* list, size_t list_len)
{
    int error = 0;
    int c = 0;
    int r, tmp = 100;
    int optimstatus;
    double* Xr = malloc(sizeof(double) * (list_len));


    // 執行次數在這
    for (size_t i = 0; i <list_len; ++i) {
        for (size_t j = 0; j < A->x_row; ++j)
            Xr[j] = A->matrix[j][list[i]] * -1;

        for (r = 1; r < A->x_row + 1; ++r) {
            error = GRBchgcoeffs(model, 1, &r, &c, &Xr[r - 1]);
            //if (error) return error;
        }

        for (size_t j = A->x_row + 1; j < A->x_row + A->y_row + 1; ++j) {
            error = GRBsetdblattrelement(model, "RHS", j, A->matrix[j - 1][list[i]]);
            //if (error)  return error;
        }
        // Optimize model 
        error = GRBoptimize(model);
        //if (error) return error;

        // get the best value
        error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval[i]);
        //if (error) return error;
    }
    free(Xr);
    return error;
}

int new_model(GRBenv* env, GRBmodel** model, MATRIX* A)
{
    char* vtype = (char*)malloc((A->col+1) * sizeof(char));
    int error = 0;
    //double    sol[A->col];       // solution
    

    double* c = (double*)malloc(sizeof(double) * (A->col + 1));
    memset(c, 0, sizeof(double) * (A->col + 1));
    c[0] = 1;

    // Create an empty model 
    error = GRBnewmodel(env, model, "mip1", 0, NULL, NULL, NULL, NULL, NULL);
    if (error) goto QUIT;

    error = GRBsetdblparam(GRBgetenv(*model), "OptimalityTol", 1e-5);
    if (error) goto QUIT;
    error = GRBsetintparam(GRBgetenv(*model), "Method", 0);
    if (error) goto QUIT;
    //error = GRBsetintparam(GRBgetenv(*model), "DualReductions", 0);
    //error = GRBsetintparam(GRBgetenv(*model), "InfUnbdInfo", 1);
    //error = GRBsetintparam(GRBgetenv(*model), "NumericFocus", 3);
    error = GRBsetintparam(GRBgetenv(*model), "OutputFlag", 0);
    

    // Add variables 
    memset(vtype, GRB_CONTINUOUS, sizeof(char) * (A->col + 1));
   
    // set the coeffient of the objective matrix
    error = GRBaddvars(*model, A->col+1, 0, NULL, NULL, NULL, c, NULL, NULL, vtype, NULL);
    if (error) goto QUIT;

    // Change objective sense to minimization 
    error = GRBsetintattr(*model, GRB_INT_ATTR_MODELSENSE, GRB_MINIMIZE);
    if (error) goto QUIT;
    
QUIT:

    // Error reporting
    if (error) {
        printf("ERROR: %s\n", GRBgeterrormsg(env));
        free(c);
        exit(1);
    }


    free(vtype);
    free(c);
    return 0;
}

void optimizer(double** objval, MATRIX* A, size_t* list, size_t list_len)
{
    GRBenv* env = NULL; // environment
    GRBmodel* model = NULL;



    int       error = 0;
    int       optimstatus;
    //double* objval = NULL;


    ///* Create environment */
    error = GRBemptyenv(&env);
    if (error) goto QUIT;

    //error = GRBsetstrparam(env, "LogFile", "mip1.log");
    //if (error) goto QUIT;

    error = GRBstartenv(env);
    if (error) goto QUIT;


    error = new_model(env, &model, A);
    //if (error) goto QUIT;
    error = init_constraints(model, A);
    //if (error) goto QUIT;
    if (list == NULL)
        error = optimize_all(model, A, *objval);
    else
        error = optimize_partOf(model, A, *objval, list, list_len);
    //if (error) goto QUIT;


QUIT:
    /* Error reporting */
    if (error) {
        printf("ERROR: %s\n", GRBgeterrormsg(env));

        exit(1);
    }

    /* Free environment */
    GRBfreeenv(env);
    // Free model 
    GRBfreemodel(model);
}