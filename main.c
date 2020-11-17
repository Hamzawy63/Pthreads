#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#define ll long long int
#define MAX 102  ;
int debug = 0;
long long int **a;
long long int **b;
long long int **c;
int r1, c1, r2, c2, r3, c3;
struct data {
    int row_number;
    int column_number;
};

void *calculate_one_element(void *data) {
    struct data *element_data = (struct data *) data;
    ll sum = 0;
    int row = element_data->row_number;
    int col = element_data->column_number;
    for (int i = 0; i < c1; i++) {
        sum += a[row][i] * b[i][col];
    }
    c[row][col] = sum;
    if (debug) fprintf(stderr, "c[%d][%d] = %lld \n", row, col, sum);
    pthread_exit(NULL);
}

void *calculate_one_row(void *data) {

    int *idx_ptr = ((int *) data);
    int i = (int) idx_ptr;


    for (int j = 0; j < c3; j++) {
        c[i][j] = 0;
        for (int k = 0; k < c1; k++) {
            c[i][j] += (a[i][k] * b[k][j]);
        }
    }
    pthread_exit(NULL);

}

void report_file_error() {
    fprintf(stderr, "Error in reading files \n");
    exit(1);
}

void print_mat(ll **mat, int row, int col) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            fprintf(stderr, "%lld ", mat[i][j]);
        }
        fprintf(stderr, "\n");
    }
}

void read_matrix(ll **mat, int row, int col) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            scanf("%lld", &mat[i][j]);
        }
    }

}


int main(int args, char **argv) {

#ifdef badoutput
    debug = 1;
#endif
    char *mat1_file = "in1";
    char *mat2_file = "in2";
    char *output_file = "c.txt";

    /*
     * Read arguments
     * Assumption : if there is more than 3 arguemnts the rest will be ignored
     */
    if (args >= 3) {
        mat1_file = argv[1];
        mat2_file = argv[2];
        if (args == 4) {
            output_file = argv[3];
        }
        printf("intput1 = %s  intput2 = %s   outputfile = %s\n", mat1_file, mat2_file, output_file);
    } else if (args != 1) {
        printf("Wrong number number of arguemnts \n");
        exit(0);
    }
    /*
    * READING INPUT
    */
    freopen(mat1_file, "r", stdin);
    scanf("row = %d col = %d", &r1, &c1);
    a = (ll **) malloc(r1 * sizeof(ll *));
    for (int i = 0; i < r1; i++) {
        a[i] = (ll *) malloc(c1 * sizeof(ll));
    }
//        read_matrix(a, r1, c1);
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c1; j++) {
            a[i][j] = 0;
        }
    }
    read_matrix(a, r1, c1);
    print_mat(a, r1, c1);


    freopen(mat2_file, "r", stdin);
    scanf("row = %d col = %d\n", &r2, &c2);
    b = (ll **) malloc(r2 * sizeof(ll *));
    for (int i = 0; i < r2; i++) {
        b[i] = (ll *) malloc(c2 * sizeof(ll));
    }
    read_matrix(b, r2, c2);
    print_mat(b, r2, c2);

    if (debug)
        fprintf(stderr, "READ Completed\n");

    struct timeval stop, start;

    if (c1 != r2) {
        printf("the number of columns of the first matrix should equal the number of rows of the second matrix\n");
        exit(1);
    }
    // initialize the two arrays
    if (debug)
        fprintf(stderr, "Now initializing the two matrices \n");


    r3 = r1;
    c3 = c2;
    c = (ll **) malloc(r3 * sizeof(ll *));
    for (int i = 0; i < r3; i++) {
        c[i] = (ll *) malloc(sizeof(ll) * c3);
    }
    // start calculating the answer
    // First using a thread for each element

    FILE *output = fopen(output_file, "w");
    gettimeofday(&start, NULL);

    pthread_t threads[r3 + 1][c3 + 1];
    struct data data_args[r3 + 1][c3 + 1];
    for (int i = 0; i < r3; i++) {
        for (int j = 0; j < c3; j++) {
            data_args[i][j].row_number = i;
            data_args[i][j].column_number = j;
            pthread_t element_thread = pthread_create(&threads[i][j], NULL, calculate_one_element,
                                                      &data_args[i][j]);
            if (element_thread) {
                printf("Problem in making thread \n");
            }

        }
    }


    if (debug)
        fprintf(stderr, "Finished and now we are waiting for the threads \n");
    for (int i = 0; i < r3; i++) {
        for (int j = 0; j < c3; j++) {
            pthread_join(threads[i][j], NULL);
        }
    }
    if (debug) {
        fprintf(stderr, "Finished and now we are going to print the result  \n");
        for (int i = 0; i < r3; i++) {
            for (int j = 0; j < c3; j++) {
                fprintf(stderr, "%lld ", c[i][j]);
            }
            fprintf(stderr, "\n");
        }
    }
    gettimeofday(&stop, NULL);
    fprintf(stderr, "When using a thread for each entry ==> Data execution took %lu seconds and %lu microseconds\n",
            stop.tv_sec - start.tv_sec, stop.tv_usec - start.tv_usec);


    fprintf(output, "The matrix using thread for each element\n");
    for (int i = 0; i < r3; i++) {
        for (int j = 0; j < c3; j++) {
            fprintf(output, "%lld ", c[i][j]);
        }
        fprintf(output , "\n") ;
    }
    /*
     *
     *
     * Calculate the answer using a thread for each row
     *
     *
     */


    if (debug) {
        fprintf(stderr, "Now we start to multply matrices using thread for each row \n");
    }
    gettimeofday(&start, NULL);
    pthread_t row_threads[r3 + 1];
    for (int i = 0; i < r3; i++) {
        pthread_t row_thread = pthread_create(&row_threads[i], NULL, calculate_one_row, (void *) i);
        if (row_thread) {
            fprintf(stderr , "Problem in making thread to calculate row number %d  \n", i);
            exit(1) ;
        }
    }
    if (debug) {
        fprintf(stderr, "Finished calling threads and now we are going to wait\n");
    }
    for (int i = 0; i < r3; i++) {
        pthread_join(row_threads[i], NULL);
    }

    gettimeofday(&stop, NULL);
    fprintf(stderr, "When using a thread for each row ==> Data execution took %lu seconds and %lu microseconds \n",
            stop.tv_sec - start.tv_sec, stop.tv_usec - start.tv_usec);


    fprintf(output, "The matrix using thread for each row\n");
    for (int i = 0; i < r3; i++) {
        for (int j = 0; j < c3; j++) {
            fprintf(output, "%lld ", c[i][j]);
        }
        fprintf(output , "\n") ;
    }
    return 0;
}
