#ifndef BTREE_H  // Verifica se BTREE_H não foi definido ainda.
    #define BTREE_H  // Define BTREE_H para evitar inclusões múltiplas.

    #include <stdio.h>  // Inclui a biblioteca padrão de entrada e saída.
    #include <stdlib.h>  // Inclui a biblioteca padrão de funções utilitárias.

    #define ORDEM 4  // Define a ordem da árvore B.
    #define MAX_CHAVES (ORDEM - 1)  // Define o número máximo de chaves em um nó da árvore B.
    #define TAM_PAG 60  // Define o tamanho da página.

    // Definição das estruturas CABECALHO e NO da árvore binária.
    typedef struct cabecalho_btree_ BTREE;  // Declaração da estrutura BTREE.
    typedef struct no_btree_ NO_BTREE;  // Declaração da estrutura NO_BTREE.

    struct cabecalho_btree_ {
        char status;                // '0' para inconsistente / '1' para OK.
        int noRaiz;                 // Armazena o RRN do nó raiz.
        int proxRRN;                // Armazena o valor do próximo RRN a ser usado.
        int nroChaves;              // Armazena o número de chaves indexadas pela árvore B.
    };

    struct no_btree_ {
        int alturaNo;               // Armazena a altura do nó.
        int nroChaves;              // Armazena o número de chaves no nó.
        int chaves[MAX_CHAVES];     // Armazena a chave primária.
        long int byteOffset[MAX_CHAVES];// Armazena o byte offset do registro no arquivo de dados.
        int ponteiroNo[ORDEM];      // Vetor para armazenar o RRN dos nós apontados por esse.
        long int rrn;                   // Armazena o RRN do nó.
    };   

    // Declaração das funções para manipulação da árvore B.

    //funçõa para inserir o cabeçalho da árvore B
    void inserirCabecalhoArvB (FILE *arquivo, BTREE *cabecalho);
    //função para recuperar o cabeçalho da árvore B
    void recuperarCabecalhoArvB (FILE *arquivo, BTREE *cabecalho);
    //função para criar um no para a árvore B
    void criarNo(NO_BTREE *no);
    //função para escrever um no no arquivo
    void escreverNo(FILE * arquivo, long int rrn, NO_BTREE *no);
    //função para recuperar um no do arquivo
    void recuperarNo(FILE * arquivo, long int rrn, NO_BTREE *no);
    //função para inicializar a árvore B
    void inicializarArvoreB(BTREE *arvore, const char *nomeArquivo);
    //função dividir o no caso cheio
    void dividirNo(int chave, long int byteOffset, NO_BTREE *no, int *chavePromovida, long int *byteOffsetPromovido, int *descendenteDireita, NO_BTREE *novoNoDireita);
    //função para inserir uma chave na árvore B
    void inserirChave(BTREE *arvore, const char *nomeArquivo, int chave, long int byteOffset);
    //função para busca uma chave da árvore B
    long int buscarChave(BTREE *arvore, FILE *arquivo, long int rrn, int chave);    
#endif  
