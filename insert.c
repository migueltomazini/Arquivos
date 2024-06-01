#include "funcoes.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void reinserir_cabecalho(FILE *arquivo, CABECALHO *cabecalho, char status) {
    cabecalho->status = status;
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&cabecalho->status, sizeof(char), 1, arquivo);
    fwrite(&cabecalho->topo, sizeof(long int), 1, arquivo);
    fwrite(&cabecalho->prosByteOffset, sizeof(long int), 1, arquivo);
    fwrite(&cabecalho->nroRegArq, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->nroRegRem, sizeof(int), 1, arquivo);
}

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
        printf("Falha no processamento do arquivo.\n");
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

void resgatarRegistro(REGISTRO *registro, FILE *arquivo) {
    fread(&registro->removido, sizeof(char), 1, arquivo);
    fread(&registro->tamanhoRegistro, sizeof(int), 1, arquivo);
    fread(&registro->prox, sizeof(long int), 1, arquivo);
    fread(&registro->id, sizeof(int), 1, arquivo);
    fread(&registro->idade, sizeof(int), 1, arquivo);
    fread(&registro->tamNomeJog, sizeof(int), 1, arquivo);
    registro->nomeJogador = (char *)malloc((registro->tamNomeJog + 1) * sizeof(char));
    fread(registro->nomeJogador, sizeof(char), registro->tamNomeJog, arquivo);
    registro->nomeJogador[registro->tamNomeJog] = '\0';
    fread(&registro->tamNacionalidade, sizeof(int), 1, arquivo);
    registro->nacionalidade = (char *)malloc((registro->tamNacionalidade + 1) * sizeof(char));
    fread(registro->nacionalidade, sizeof(char), registro->tamNacionalidade, arquivo);
    registro->nacionalidade[registro->tamNacionalidade] = '\0';
    fread(&registro->tamNomeClube, sizeof(int), 1, arquivo);
    registro->nomeClube = (char *)malloc((registro->tamNomeClube + 1) * sizeof(char));
    fread(registro->nomeClube, sizeof(char), registro->tamNomeClube, arquivo);
    registro->nomeClube[registro->tamNomeClube] = '\0';
}

void preeche_vazio(FILE *arquivo, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        fwrite("$", sizeof(char), 1, arquivo);
    }
}

void lixo (FILE *arquivo) {
    char lixo = 'a';
    while (lixo != '$' && !feof(arquivo)) {
        fread(&lixo, sizeof(char), 1, arquivo);
    }
    fseek(arquivo, -1, SEEK_CUR);
}

void insertIntoAux(char *nomeArquivo) {
    int id, idade; 
    char nomeJogador[30], nacionalidade[30], nomeClube[30], auxIdade[4];

    scanf("%d", &id);
    scan_quote_string(auxIdade);
    scan_quote_string(nomeJogador);
    scan_quote_string(nacionalidade);
    scan_quote_string(nomeClube);
    idade = atoi(auxIdade);
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
    if (cabecalho.topo == -1) { // não tem registros removidos
        fseek(nomearq, 0, SEEK_END);
        inserirRegistro(registro, nomearq);
        cabecalho.prosByteOffset += registro->tamanhoRegistro;
        cabecalho.nroRegArq++;
        desalocarRegistro(&registro);
        reinserir_cabecalho(nomearq, &cabecalho, '1');
        fclose(nomearq);
        return;
    } 

    REGISTRO* anterior = (REGISTRO*) malloc(sizeof(REGISTRO));
    if (!anterior) {
        printf("Falha no processamento do arquivo.\n");
        exit(1);
    }
    fseek(nomearq, cabecalho.topo, SEEK_SET);
    resgatarRegistro(anterior, nomearq);

    if (anterior->prox == -1) { // se só existe 1 registro removido
        if (registro->tamanhoRegistro <= anterior->tamanhoRegistro) {
            fseek(nomearq, (-1) * anterior->tamanhoRegistro, SEEK_CUR);
            int tamaux = registro->tamanhoRegistro;
            registro->tamanhoRegistro = anterior->tamanhoRegistro;
            inserirRegistro(registro, nomearq);
            preeche_vazio(nomearq, anterior->tamanhoRegistro - tamaux);
            cabecalho.topo = -1;
            cabecalho.nroRegRem--;
            cabecalho.nroRegArq++;
            reinserir_cabecalho(nomearq, &cabecalho, '1');
            desalocarRegistro(&anterior);
            desalocarRegistro(&registro);
            fclose(nomearq);
            return;
        } else { // insere no fim
            fseek(nomearq, 0, SEEK_END);
            inserirRegistro(registro, nomearq);
            cabecalho.prosByteOffset += registro->tamanhoRegistro;
            cabecalho.nroRegArq++;
            desalocarRegistro(&registro);
            reinserir_cabecalho(nomearq, &cabecalho, '1');
            fclose(nomearq);
            return;
        }
    } else if (registro->tamanhoRegistro <= anterior->tamanhoRegistro) { // se for o primeiro
        fseek(nomearq, (-1) * anterior->tamanhoRegistro, SEEK_CUR);
        int tamaux = registro->tamanhoRegistro;
        registro->tamanhoRegistro = anterior->tamanhoRegistro;
        inserirRegistro(registro, nomearq);
        preeche_vazio(nomearq, anterior->tamanhoRegistro - tamaux);
        cabecalho.topo = anterior->prox;
        cabecalho.nroRegRem--;
        cabecalho.nroRegArq++;
        reinserir_cabecalho(nomearq, &cabecalho, '1');
        desalocarRegistro(&anterior);
        desalocarRegistro(&registro);
        fclose(nomearq);
        return;
    }

    long long int aux_anterior = cabecalho.topo; // anterior da onde eu quero inserir
    long long int aux_atual = anterior->prox; // aonde eu quero inserir
    int num = cabecalho.nroRegRem;
    for (int i = 1; i < num; i++) { // pula o primeiro pq tratamos antes
        /* existe tres casos:
        1- só tem um removido (feita acima)
        2- inserção no início (feita acima)
        3- inserção no meio (feita abaixo)
        4- chegou ao fim */

        REGISTRO* atual = (REGISTRO*) malloc(sizeof(REGISTRO));
        if (!atual) {
            printf("Falha no processamento do arquivo.\n");
            exit(1);
        }
        fseek(nomearq, aux_atual, SEEK_SET);
        resgatarRegistro(atual, nomearq);

        if (registro->tamanhoRegistro <= atual->tamanhoRegistro) {
            fseek(nomearq, (-1) * atual->tamanhoRegistro, SEEK_CUR);
            int tamaux = registro->tamanhoRegistro;
            registro->tamanhoRegistro = atual->tamanhoRegistro;
            inserirRegistro(registro, nomearq);
            preeche_vazio(nomearq, atual->tamanhoRegistro - tamaux);
            cabecalho.nroRegRem--;
            cabecalho.nroRegArq++;
            if (atual->prox == -1) { // se for o último
                cabecalho.topo = -1;
            } else {
                cabecalho.topo = atual->prox;
            }

            anterior->prox = atual->prox;
            fseek(nomearq, aux_anterior, SEEK_SET);
            inserirRegistro(anterior, nomearq);
            reinserir_cabecalho(nomearq, &cabecalho, '1');
            desalocarRegistro(&atual);
            desalocarRegistro(&anterior);
            desalocarRegistro(&registro);
            fclose(nomearq);
            return; 
        }
        desalocarRegistro(&anterior);
        anterior = atual;
        aux_anterior = aux_atual;
        aux_atual = atual->prox;
    } 

    fseek(nomearq, 0, SEEK_END);
    inserirRegistro(registro, nomearq);
    cabecalho.nroRegArq++;
    cabecalho.prosByteOffset += registro->tamanhoRegistro;
    reinserir_cabecalho(nomearq, &cabecalho, '1');
    desalocarRegistro(&registro);
    fclose(nomearq);
}

void insertInto(char *nomeArquivo, char *nomeIndice, int nroAdicoes) {
    for (int i = 0; i < nroAdicoes; i++) {
        insertIntoAux(nomeArquivo);
    }
    createIndex(nomeArquivo, nomeIndice);
}
