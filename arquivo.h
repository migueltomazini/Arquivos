/* Utilizado na manipulação dos dados, tendo um arquivo .csv como entrada
e um arquivo binário como saída*/

#ifndef ARQUIVO_H
    #define ARQUIVO_H
    
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>

    typedef struct cabecalho_ CABECALHO;
    typedef struct registro_ REGISTRO;

    struct cabecalho_ {
        char status;
        long int topo;
        long int prosByteOffset;
        int nroRegArq;
        int nroRegRem;
    };

    struct registro_ {
        char removido;              // '0' não removido / '1' removido
        int tamanhoRegistro;
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


    void csvToBin(char *nomeArquivoCsv, char *nomeArquivoBin);
    void inserirRegistro(REGISTRO *registro, FILE *arquivo);
    
#endif