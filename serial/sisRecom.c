//  1. Gracieth Manuel - 20200780
//  2. Jesus Finda - 20201064
//  3. Victor Soares - 20200011

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// Definição do valor aleatório para a multiplicação de matrizes
#define ALEATORIO ((double)rand() / (double)RAND_MAX)

// Protótipos das funções
void multiplicar_matrizes(int row1, int col1, double m1[row1][col1], int row2, int col2, double m2[row2][col2], double m3[row1][col2]);
void copiar_matriz(int num_linhas, int num_cols, double dest[num_linhas][num_cols], double orig[num_linhas][num_cols]);
void imprimir_matriz(int row, int column, double matriz[row][column]);
void executar_algoritmo(int num_iteracoes, double alpha, int nF, int nU, int nI, int num_avaliacoes, double L[nU][nF], double R[nF][nI], double A[nU][nI], double B[nU][nI]);

int main(int argc, char *argv[]) {
    double alpha; // Coeficiente de aprendizagem
    int num_iteracoes; // Número de iterações do algoritmo
    int nU, nF, nI; // Número de usuários, características e itens
    int num_avaliacoes; // Número de avaliações

    FILE *entrada; // Ponteiro para o arquivo de entrada
    FILE *saida; // Ponteiro para o arquivo de saída

    // Verifica se o nome do arquivo de entrada foi fornecido como argumento de linha de comando
    if (argc < 2 || ((entrada = fopen(argv[1], "r")) == NULL)) {
        printf("Erro ao abrir o arquivo de entrada: %s\n", argv[1]);
        return 0;
    }
    printf("Carregando o %s...\n", argv[1]);

    // Abre o arquivo de saída para escrita
    saida = fopen("saida.txt", "w");
    if (saida == NULL) {
        printf("Erro ao abrir o arquivo de saída.\n");
        return 0;
    }

    // Lê os parâmetros do arquivo de entrada
    fscanf(entrada, "%d", &num_iteracoes);
    fscanf(entrada, "%lf", &alpha);
    fscanf(entrada, "%d", &nF);
    fscanf(entrada, "%d", &nU);
    fscanf(entrada, "%d", &nI);
    fscanf(entrada, "%d", &num_avaliacoes);

    // Declaração das matrizes
    double L[nU][nF]; // Matriz L
    double R[nF][nI]; // Matriz R
    double B[nU][nI]; // Matriz de resultados das recomendações
    double A[nU][nI]; // Matriz de avaliações

    // Inicializa a matriz de avaliações com zeros
    memset(A, 0, sizeof(A[0]) * nU);
    // Preenche a matriz de avaliações com os valores do arquivo de entrada
    for (int a = 0; a < num_avaliacoes; a++) {
        int i, j;
        fscanf(entrada, "%d", &i);
        fscanf(entrada, "%d", &j);
        fscanf(entrada, "%lf", &A[i][j]);
    }

    // Inicializa as matrizes L e R com valores aleatórios
    srandom(0); // Define a semente para geração de números aleatórios
    for (int i = 0; i < nU; i++)
        for (int j = 0; j < nF; j++)
            L[i][j] = ALEATORIO / (double)nF; // Valores aleatórios para L

    for (int i = 0; i < nF; i++)
        for (int j = 0; j < nI; j++)
            R[i][j] = ALEATORIO / (double)nF; // Valores aleatórios para R

    // Chama a função para executar o algoritmo de recomendação
    executar_algoritmo(num_iteracoes, alpha, nF, nU, nI, num_avaliacoes, L, R, A, B);

    // Recomendações
    for (int i = 0; i < nU; i++) {
        double avaliacao_especulada = 0.0;
        int recomendacao = 0;
        // Determina a melhor recomendação para cada usuário
        for (int j = 0; j < nI; j++) {
            if (A[i][j] == 0 && avaliacao_especulada < B[i][j]) {
                avaliacao_especulada = B[i][j];
                recomendacao = j;
            }
        }
        printf("%d\n", recomendacao); // Imprime a recomendação para o usuário
        // Escreve no arquivo de saída
        fprintf(saida, "%d\n", recomendacao);
    }

    fclose(entrada); // Fecha o arquivo de entrada
    fclose(saida); // Fecha o arquivo de saída
    return 0;
}

// Função para multiplicar duas matrizes e armazenar o resultado em uma terceira matriz
void multiplicar_matrizes(int row1, int col1, double m1[row1][col1], int row2, int col2, double m2[row2][col2], double m3[row1][col2]) {
    // Inicializa a matriz de resultado com zeros
    for (int i = 0; i < row1; i++) {
        for (int j = 0; j < col2; j++) {
            m3[i][j] = 0.0;
        }
    }

    // Realiza a multiplicação das matrizes m1 e m2
    for (int i = 0; i < row1; i++) {
        for (int j = 0; j < col2; j++) {
            for (int k = 0; k < col1; k++)
                m3[i][j] += m1[i][k] * m2[k][j];
        }
    }
}

// Função para copiar uma matriz para outra matriz
void copiar_matriz(int num_linhas, int num_cols, double dest[num_linhas][num_cols], double orig[num_linhas][num_cols]) {
    // Copia os elementos da matriz orig para a matriz dest
    for (int i = 0; i < num_linhas; i++) {
        for (int j = 0; j < num_cols; j++)
            dest[i][j] = orig[i][j];
    }
}

// Função para imprimir uma matriz
void imprimir_matriz(int row, int column, double matriz[row][column]) {
    // Imprime os elementos da matriz
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            printf("%lf ", matriz[i][j]);
        }
        putchar('\n');
    }
}

// Função para executar o algoritmo de recomendação
void executar_algoritmo(int num_iteracoes, double alpha, int nF, int nU, int nI, int num_avaliacoes, double L[nU][nF], double R[nF][nI], double A[nU][nI], double B[nU][nI]) {
    // Declaração de matrizes temporárias para atualização de L e R
    double L_t[nU][nF];
    double R_t[nF][nI];

    // Loop principal para iterar sobre as iterações especificadas
    for (int iter = 0; iter < num_iteracoes; iter++) {
        // Atualiza a matriz R
        for (int k = 0; k < nF; k++) {
            for (int j = 0; j < nI; j++) {
                double soma = 0.0;
                // Calcula a soma para a atualização da matriz R
                for (int i = 0; i < nU; i++)
                    if (A[i][j] > 0)
                        soma += 2 * (A[i][j] - B[i][j]) * (-L[i][k]);
                // Atualiza a matriz R_t
                R_t[k][j] = R[k][j] - alpha * soma;
            }
            // Atualiza a matriz L
            for (int i = 0; i < nU; i++) {
                double soma = 0.0;
                // Calcula a soma para a atualização da matriz L
                for (int j = 0; j < nI; j++)
                    if (A[i][j] > 0)
                        soma += 2 * (A[i][j] - B[i][j]) * (-R[k][j]);
                // Atualiza a matriz L_t
                L_t[i][k] = L[i][k] - alpha * soma;
            }
        }
        // Copia as matrizes atualizadas de volta para L e R
        copiar_matriz(nF, nI, R, R_t);
        copiar_matriz(nU, nF, L, L_t);
        // Multiplica as matrizes L e R novamente para obter a nova matriz B
        multiplicar_matrizes(nU, nF, L, nF, nI, R, B);
    }
}
