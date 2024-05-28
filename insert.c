#include "funcoes.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

REGISTRO *criar_registro(int id, int idade, const char *nomeJogador, const char *nacionalidade, const char *clube) {
    REGISTRO *novoRegistro = (REGISTRO *)malloc(sizeof(REGISTRO));
    if (!novoRegistro) {
        printf("Falha no processamento do arquivo.\n");
        exit(1);
    }

    novoRegistro->id = id;
    novoRegistro->idade = idade;
    novoRegistro->tamNacionalidade = strlen(nacionalidade);
    novoRegistro->tamNomeJog = strlen(nomeJogador);
    novoRegistro->tamNomeClube = strlen(clube);

    novoRegistro->nomeJogador = (char *)malloc((novoRegistro->tamNomeJog + 1) * sizeof(char));
    novoRegistro->nacionalidade = (char *)malloc((novoRegistro->tamNacionalidade + 1) * sizeof(char));
    novoRegistro->nomeClube = (char *)malloc((novoRegistro->tamNomeClube + 1) * sizeof(char));

    if (!novoRegistro->nomeJogador || !novoRegistro->nacionalidade || !novoRegistro->nomeClube) {
        printf("Falha no procesamento do arquivo.\n");
        exit(1);
    }

    strcpy(novoRegistro->nomeJogador, nomeJogador);
    strcpy(novoRegistro->nacionalidade, nacionalidade);
    strcpy(novoRegistro->nomeClube, clube);

    novoRegistro->tamanhoRegistro = 33 + novoRegistro->tamNomeJog + novoRegistro->tamNomeClube + novoRegistro->tamNacionalidade;
    novoRegistro->removido = '0';
    novoRegistro->prox = -1;

    return novoRegistro;
}

void insertIntoAux(char *nomeArquivo) {
    int id, idade; 
    char nomeJogador[30], nacionalidade[30], nomeClube[30];

    scanf("%d", &id);
    scan_quote_string(nomeJogador);
    scan_quote_string(nacionalidade);
    scan_quote_string(nomeClube);

    if (idade == 0) {
        idade = -1;
    }

    FILE* nomearq = fopen(nomeArquivo, "rb+");
    if (nomearq == NULL) {
        printf("Falha no processamento do arquivo\n");
        exit(1);
    }

    CABECALHO cabecalho;
    fread(&cabecalho.status, sizeof(char), 1, nomearq);
    fread(&cabecalho.topo, sizeof(long int), 1, nomearq);
    fread(&cabecalho.prosByteOffset, sizeof(long int), 1, nomearq);
    fread(&cabecalho.nroRegArq, sizeof(int), 1, nomearq);
    fread(&cabecalho.nroRegRem, sizeof(int), 1, nomearq);

    if (cabecalho.status == '0') {
        printf("Falha no processamento do arquivo\n");
        fclose(nomearq);
        exit(1);
    }

    REGISTRO* registro = criar_registro(id, idade, nomeJogador, nacionalidade, nomeClube);
    REGISTRO* atual = NULL;
    int maxNomeJog = 30, maxNacionalidade = 30, maxNomeClube = 30;

    if (cabecalho.topo != -1) {
        fseek(nomearq, cabecalho.topo, SEEK_SET);
        recuperarRegistro(&atual, nomearq, &maxNomeJog, &maxNacionalidade, &maxNomeClube);
    }

    if (cabecalho.topo == -1) {
        fseek(nomearq, 0, SEEK_END);
        inserirRegistro(registro, nomearq);
    } else {
        long int num = cabecalho.nroRegRem;
        REGISTRO* aux = NULL;
        for (long int i = 0; i <= num; i++) {
            if (registro->tamanhoRegistro <= atual->tamanhoRegistro) {
                fseek(nomearq, (-1) * (atual->tamanhoRegistro), SEEK_CUR);
                inserirRegistro(registro, nomearq);
                aux->prox = atual->prox;
                atual = aux;
                break;
            }
            aux = atual;
            fseek(nomearq, atual->prox, SEEK_SET);
            recuperarRegistro(&atual, nomearq, &maxNomeJog, &maxNacionalidade, &maxNomeClube);
            num--;
        }
        if (num == 0) {
            fseek(nomearq, 0, SEEK_END);
            inserirRegistro(registro, nomearq);
        }
            desalocarRegistro(&aux);
    }

    
    fclose(nomearq);
}

void insertInto(char *nomeArquivo, char *nomeIndice, int nroAdicoes) {
    for (int i = 0; i < nroAdicoes; i++) {
        insertIntoAux(nomeArquivo);
    }
    createIndex(nomeArquivo, nomeIndice);
}
