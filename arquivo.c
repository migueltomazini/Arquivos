#include "arquivo.h"

struct cabecalho_ {
    char status;
    long int topo;
    long int prosByteOffset;
    int nroRegArq;
    int nroRegRem;
};

struct registro_ {
    char removido;
    int tamanhoRegistro;//oi
    long int prox;
    int id;
    int idade;
    int tamNomeJog;
    char *nomeJogador;
    int tamNacionalidade;
    char *nacionalidade;
    int tamNomeClube;
    char *nomeClube;
};

// Função responsável pela montagem e inserção do cabeçalho
void cabecalho(FILE *arquivo) {
    CABECALHO *cabecalho = malloc(sizeof(CABECALHO));
    if (cabecalho == NULL)
        return;
    
    cabecalho->status = 0;
    cabecalho->topo = -1;
    cabecalho->prosByteOffset = 0;
    cabecalho->nroRegArq = 0;
    cabecalho->nroRegRem = 0;

    fwrite(cabecalho, sizeof(cabecalho), 1, arquivo);

    free(cabecalho);
}

void registro() {}

// Função responsável por passar os dados do arquivo .csv para o binário
FILE *inserirDados(char *nomeArquivoCsv) {
    FILE *arquivo;      // Arquivo binário para qual serão passados os dados
    FILE *dados;        // Arquivo .csv contendo os dados
    

    printf("%s\n", nomeArquivoCsv);
    // Abertura e testagem dos arquivos 
    if ((dados = fopen(nomeArquivoCsv, "r")) == NULL ||
        (arquivo = fopen("arquivo.bin", "ab")) == NULL) {
        printf("Erro na abertura do arquivo.\n");
        return NULL;
    }

    // Pular primeira linha
    while ((fgetc(dados)) != '\n') {}
    cabecalho(arquivo);

    fclose(arquivo);
    fclose(dados);

    return arquivo;
}
