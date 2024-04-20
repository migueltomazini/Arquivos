/* Utilizado na manipulação dos dados, tendo um arquivo .csv como entrada
e um arquivo binário como saída*/

#ifndef FUNCOES_H
    #define FUNCOES_H
    
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>

    #define TAM_CABECALHO 25

    typedef struct cabecalho_ CABECALHO;
    typedef struct registro_ REGISTRO;

    struct cabecalho_ {
        char status;                // '0' para inconsistente / '1' para OK
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

    // Funções fornecidas
    void binarioNaTela(char *nomeArquivoBinario);
    void scan_quote_string(char *str);

    // Funções auxiliares 
    void inserirRegistro(REGISTRO *registro, FILE *arquivo);
    void alocarRegistro(REGISTRO **registro, int maxNomeJog, int maxNacionalidade, int maxNomeClube);
    void desalocarRegistro(REGISTRO **registro);

    // Lista o arquivo de saída no modo binário usando a função fornecida binarioNaTela (Funcionalidade 1)
    void createTable(char *nomeArquivoCsv, char *nomeArquivoBin);
    // Lista todos os dados de um arquivo de entrada de forma organizada (Funcionalidade 2)
    void selectFrom(char *nomeArquivo);
    // Função que realiza n buscas em um dado arquivo (Funcionalidade 3)
    void selectFromWhere(char *nomeArquivo, int nroBuscas);
    
#endif