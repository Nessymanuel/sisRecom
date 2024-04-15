#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <omp.h>

#define ALEATORIO ((double)rand() / (double)RAND_MAX)

void multiplicar_matrizes(int row1, int col1, double m1[row1][col1], int row2, int col2, double m2[row2][col2], double m3[row1][col2]);
void copiar_matriz(int num_linhas, int num_cols, double dest[num_linhas][num_cols], double orig[num_linhas][num_cols]);
void executar_algoritmo(int num_iteracoes, double alpha, int nF, int nU, int nI, int num_avaliacoes, double L[][nF], double R[][nI], double A[][nI], double B[][nI]);

int main(int argc, char *argv[]) {
    double start, end;
    start = omp_get_wtime(); 

    double alpha;
    int num_iteracoes;
    int nU, nF, nI;
    int num_avaliacoes;
    FILE *entrada;
    FILE *saida;

    if (argc < 2 || ((entrada = fopen(argv[1], "r")) == NULL)) {
        printf("Erro ao abrir o arquivo de entrada: %s\n", argv[1]);
        return 0;
    }
    printf("Carregando o %s...\n", argv[1]);

    saida = fopen("saida.txt", "w");
    if (saida == NULL) {
        printf("Erro ao abrir o arquivo de saída.\n");
        return 0;
    }

    fscanf(entrada, "%d", &num_iteracoes);
    fscanf(entrada, "%lf", &alpha);
    fscanf(entrada, "%d", &nF);
    fscanf(entrada, "%d", &nU);
    fscanf(entrada, "%d", &nI);
    fscanf(entrada, "%d", &num_avaliacoes);

    double L[nU][nF];
    double R[nF][nI];
    double B[nU][nI];
    double A[nU][nI];

    memset(A, 0, sizeof(A[0]) * nU);
    for (int a = 0; a < num_avaliacoes; a++) {
        int i, j;
        fscanf(entrada, "%d", &i);
        fscanf(entrada, "%d", &j);
        fscanf(entrada, "%lf", &A[i][j]);
    }

    srandom(0);
    for (int i = 0; i < nU; i++)
        for (int j = 0; j < nF; j++)
            L[i][j] = ALEATORIO / (double)nF;

    for (int i = 0; i < nF; i++)
        for (int j = 0; j < nI; j++)
            R[i][j] = ALEATORIO / (double)nF;

    executar_algoritmo(num_iteracoes, alpha, nF, nU, nI, num_avaliacoes, L, R, A, B);

    for (int i = 0; i < nU; i++) {
        double avaliacao_especulada = 0.0;
        int recomendacao = 0;
        for (int j = 0; j < nI; j++) {
            if (A[i][j] == 0 && avaliacao_especulada < B[i][j]) {
                avaliacao_especulada = B[i][j];
                recomendacao = j;
            }
        }
        printf("%d\n", recomendacao);
        fprintf(saida, "%d\n", recomendacao);
    }

    fclose(entrada);
    fclose(saida);

    end = omp_get_wtime();
    
    printf("Tempo de execução %f", end - start);
    return 0;
}

void multiplicar_matrizes(int row1, int col1, double m1[row1][col1], int row2, int col2, double m2[row2][col2], double m3[row1][col2]) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < row1; i++) {
        for (int j = 0; j < col2; j++) {
            m3[i][j] = 0.0;
        }
    }

    #pragma omp parallel for collapse(3)
    for (int i = 0; i < row1; i++) {
        for (int j = 0; j < col2; j++) {
            for (int k = 0; k < col1; k++)
                m3[i][j] += m1[i][k] * m2[k][j];
        }
    }
}

void copiar_matriz(int num_linhas, int num_cols, double dest[num_linhas][num_cols], double orig[num_linhas][num_cols]) {
    for (int i = 0; i < num_linhas; i++) {
        for (int j = 0; j < num_cols; j++)
            dest[i][j] = orig[i][j];
    }
}

void executar_algoritmo(int num_iteracoes, double alpha, int nF, int nU, int nI, int num_avaliacoes, double L[][nF], double R[][nI], double A[][nI], double B[][nI]) {
    double L_t[nU][nF];
    double R_t[nF][nI];

    for (int iter = 0; iter < num_iteracoes; iter++) {
        double soma;

        #pragma omp parallel for collapse(2) private(soma)
        for (int k = 0; k < nF; k++) {
            for (int j = 0; j < nI; j++) {
                 soma = 0.0;
                for (int i = 0; i < nU; i++)
                    if (A[i][j] > 0)
                        soma += 2 * (A[i][j] - B[i][j]) * (-L[i][k]);
                R_t[k][j] = R[k][j] - alpha * soma;
            }
        }

        #pragma omp parallel for collapse(2) private(soma)
        for (int i = 0; i < nU; i++) {
            for (int k = 0; k < nF; k++) {
                 soma = 0.0;
                for (int j = 0; j < nI; j++)
                    if (A[i][j] > 0)
                        soma += 2 * (A[i][j] - B[i][j]) * (-R[k][j]);
                L_t[i][k] = L[i][k] - alpha * soma;
            }
        }

        copiar_matriz(nF, nI, R, R_t);
        copiar_matriz(nU, nF, L, L_t);
        multiplicar_matrizes(nU, nF, L, nF, nI, R, B);
    }
}

