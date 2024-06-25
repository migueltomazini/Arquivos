#ifndef BTREE_H
    #define BTREE_H

    #include <stdio.h>
    #include <stdlib.h>

    #define ORDEM 4
    #define MAX_CHAVES (ORDEM - 1)
    #define TAM_PAG 60

    // Definição das estruturas CABECALHO e NO da árvore binária
    typedef struct cabecalho_btree_ BTREE;
    typedef struct no_btree_ NO_BTREE;

    struct cabecalho_btree_ {
        char status;                // '0' para inconsistente / '1' para OK
        int noRaiz;                 // Armazena o RRN do nó raiz
        int proxRRN;                // Armazena o valor do próximo RRN a ser usado
        int nroChaves;              // Armazena o número de chaves indexadas pela árvore B
    };

    struct no_btree_ {
        int alturaNo;               // Armazena a áltura do nó
        int nroChaves;              // Armazena o número de chaves no nó
        int chaves[MAX_CHAVES];     // Armazena a chave primária
        long byteOffset[MAX_CHAVES];// Armazena o byteffset do registro no arquivo de dados
        int ponteiroNo[ORDEM]; // Vetor para armazenar o RRN dos nós apontados por esse
        long rrn;
    };   

    void criarNo(NO_BTREE *no);
    void inserirCabecalhoArvB (FILE *arquivo, BTREE *cabecalho);
    void recuperarCabecalhoArvB (FILE *arquivo, BTREE *cabecalho);
    void escreverNo(FILE * arquivo, long rrn, NO_BTREE *no);
    void recuperarNo(FILE * arquivo, long rrn, NO_BTREE *no);
    void inicializarArvoreB(BTREE *arvore, const char *nomeArquivo);
    void dividirNo(int chave, long byteOffset, NO_BTREE *no, int *chavePromovida, long *byteOffsetPromovido, int *descendenteDireita, NO_BTREE *novoNoDireita);
    int inserirChaveRecursivo(FILE *arquivo, BTREE *arvore, int rrnAtual, int chave, long byteOffset, int *promoKey, long *promoOffset, int *promoRChild);
    void printDescendente(BTREE * arvore, int rrn, FILE * arquivo);
    void printRaiz(BTREE * arvore, FILE * arquivo);
    void inserirChave(BTREE *arvore, const char *nomeArquivo, int chave, long byteOffset);
    long buscarChave(BTREE *arvore, FILE *arquivo, long rrn, int chave);
    

    
#endif