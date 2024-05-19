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
    FILE *arquivoIndice = fopen(nomeIndice, "wb");
    
    if (arquivoDados == NULL || arquivoIndice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        if (arquivoDados) fclose(arquivoDados);
        if (arquivoIndice) fclose(arquivoIndice);
        return;
    }
    


    CABECALHO_IND* cabecalhoIndice = malloc(sizeof(CABECALHO_IND));
    cabecalhoIndice->status = '0'; // define status para 0 
    fwrite(&cabecalhoIndice->status, sizeof(char), 1, arquivoIndice);


    CABECALHO* cabecalho = malloc(sizeof(CABECALHO));
    fread(&cabecalho->status, sizeof(char), 1, arquivoDados);
    fread(&cabecalho->topo, sizeof(long int), 1, arquivoDados);
    fread(&cabecalho->prosByteOffset, sizeof(long int), 1, arquivoDados);
    fread(&cabecalho->nroRegArq, sizeof(int), 1, arquivoDados);
    fread(&cabecalho->nroRegRem, sizeof(int), 1, arquivoDados);
    
    if (cabecalho->status == '0')
    {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivoDados);
        fclose(arquivoIndice);
        exit(0);
    }
    

    int cont = cabecalho->nroRegArq + cabecalho->nroRegRem;
    
    REGISTRO_IND* vetor[cabecalho->nroRegArq];
    int aux = 0;

    for (int i = 0; i < cont; i++) {
        long int byteOffset = ftell(arquivoDados);
        
        REGISTRO* registro = malloc(sizeof(REGISTRO));
        fread(&registro->removido, sizeof(char), 1, arquivoDados);
        fread(&registro->tamanhoRegistro, sizeof(int), 1, arquivoDados);
        fread(&registro->prox, sizeof(long int), 1, arquivoDados);
        fread(&registro->id, sizeof(int), 1, arquivoDados);
        fread(&registro->idade, sizeof(int), 1, arquivoDados);
        
        fread(&registro->tamNomeJog, sizeof(int), 1, arquivoDados);
        if (registro->tamNomeJog > 0) {
            registro->nomeJogador = malloc(registro->tamNomeJog);
            fread(registro->nomeJogador, sizeof(char), registro->tamNomeJog, arquivoDados);
        } else {
            registro->nomeJogador = NULL;
        }

        fread(&registro->tamNacionalidade, sizeof(int), 1, arquivoDados);
        if (registro->tamNacionalidade > 0) {
            registro->nacionalidade = malloc(registro->tamNacionalidade);
            fread(registro->nacionalidade, sizeof(char), registro->tamNacionalidade, arquivoDados);
        } else {
            registro->nacionalidade = NULL;
        }

        fread(&registro->tamNomeClube, sizeof(int), 1, arquivoDados);
        if (registro->tamNomeClube > 0) {
            registro->nomeClube = malloc(registro->tamNomeClube);
            fread(registro->nomeClube, sizeof(char), registro->tamNomeClube, arquivoDados);
        } else {
            registro->nomeClube = NULL;
        }
        
        if (registro->removido == '0') {
            REGISTRO_IND* registroIndice = malloc(sizeof(REGISTRO_IND));
            registroIndice->id = registro->id; // passa todos os id para o arq indice
            registroIndice->byteOffset = byteOffset; // passa o byteoffset para o arq de indice

            vetor[aux] = registroIndice;
            aux++;
        }
        
        desalocarRegistro(&registro);
    }
    
    bubblesort(vetor, aux);

    for (int i = 0; i < aux; i++) {
        fwrite(&vetor[i]->id, sizeof(int), 1, arquivoIndice);
        fwrite(&vetor[i]->byteOffset, sizeof(long int), 1, arquivoIndice);
        free(vetor[i]);
    }
    
    fseek(arquivoIndice, 0, SEEK_SET);
    cabecalhoIndice->status = '1'; // define status para 1
    fwrite(&cabecalhoIndice->status, sizeof(char), 1, arquivoIndice);

    fclose(arquivoDados);
    fclose(arquivoIndice);
    free(cabecalhoIndice);
    free(cabecalho);
}
