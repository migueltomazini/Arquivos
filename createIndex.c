#include "funcoes.h"


void bubblesort(REGISTRO_IND* vetor[], int n) {
    int i, j;
    REGISTRO_IND* aux;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n - 1; j++) {
            if (vetor[j]->id > vetor[j + 1]->id) {
                aux = vetor[j];
                vetor[j] = vetor[j + 1];
                vetor[j + 1] = aux;
            }
        }
    }
}

void createIndex(char *nomeArquivo, char *nomeIndice) {
    FILE *arquivoDados = fopen(nomeArquivo, "rb");
    if (arquivoDados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        exit(1);
    }

    CABECALHO cabecalho;
    fread(&cabecalho.status, sizeof(char), 1, arquivoDados);
    fread(&cabecalho.topo, sizeof(long int), 1, arquivoDados);
    fread(&cabecalho.prosByteOffset, sizeof(long int), 1, arquivoDados);
    fread(&cabecalho.nroRegArq, sizeof(int), 1, arquivoDados);
    fread(&cabecalho.nroRegRem, sizeof(int), 1, arquivoDados);

    if (cabecalho.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivoDados);
        exit(0);
    }

    FILE *arquivoIndice = fopen(nomeIndice, "wb");
    if (arquivoIndice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivoDados);
        exit(1);
    }

    CABECALHO_IND cabecalhoIndice;
    cabecalhoIndice.status = '0';
    fwrite(&cabecalhoIndice.status, sizeof(char), 1, arquivoIndice);

    int cont = cabecalho.nroRegArq + cabecalho.nroRegRem;

    REGISTRO_IND* vetor[cabecalho.nroRegArq];
    int aux = 0;

    for (int i = 0; i < cont; i++) {
        long int byteOffset = ftell(arquivoDados);
        REGISTRO* registro = malloc(sizeof(REGISTRO));
        resgatarRegistro(registro, arquivoDados);
        lixo(arquivoDados);
        if (registro->removido == '0') {
            REGISTRO_IND* registroIndice = malloc(sizeof(REGISTRO_IND));
            registroIndice->id = registro->id;
            registroIndice->byteOffset = byteOffset;
            vetor[aux] = registroIndice;
            aux++;
        }
        free(registro);
    }

    bubblesort(vetor, aux);

    for (int i = 0; i < aux; i++) {
        fwrite(&vetor[i]->id, sizeof(int), 1, arquivoIndice);
        fwrite(&vetor[i]->byteOffset, sizeof(long int), 1, arquivoIndice);
        free(vetor[i]);
    }

    fseek(arquivoIndice, 0, SEEK_SET);
    cabecalhoIndice.status = '1';
    fwrite(&cabecalhoIndice.status, sizeof(char), 1, arquivoIndice);

    fclose(arquivoDados);
    fclose(arquivoIndice);
}

// Função responsável por recuperar o arquivo de índice da memória secundária e passá-lo
// para a memória primária
REGISTRO_IND *recoverIndex(FILE *arquivo, FILE *indice) {
    int nroRegistros;
    REGISTRO_IND *registro;

    if ((getc(indice)) == '0') {
        printf("Falha no processamento do indice.\n");
        return NULL;
    }

    // Recuperar o número de registros 
    fseek(arquivo, 17, SEEK_SET);
    fread(&nroRegistros, sizeof(int), 1, arquivo);

    registro = malloc(sizeof(REGISTRO_IND) * (nroRegistros + 1));
        if (registro == NULL) {
        printf("Erro ao alocar memória.\n");
        return NULL;
    }

    // Passar o conteúdo do arquivo índice para a memória primaria
    for (int i = 0; i < nroRegistros; i++) {
        fread(&registro[i].id, sizeof(int), 1, indice);
        fread(&registro[i].byteOffset, sizeof(long int), 1, indice);
    }

    return(registro);
}