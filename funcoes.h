#ifndef FUNCOES_H
    #define FUNCOES_H
    
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>

    #define TAM_CABECALHO 25 // Define o tamanho padrão do cabeçalho do arquivo binário

    // Definição das estruturas CABECALHO e REGISTRO
    typedef struct cabecalho_ CABECALHO;
    typedef struct registro_ REGISTRO;

    // Definição das estruturas CABECALHO e REGISTRO do índice
    typedef struct cabecalho_ind_ CABECALHO_IND;
    typedef struct registro_ind_ REGISTRO_IND;

    struct cabecalho_ {
        char status;                // '0' para inconsistente / '1' para OK
        long int topo;              // Ponteiro para o topo da pilha de registros removidos
        long int prosByteOffset;    // Próximo byte offset disponível no arquivo
        int nroRegArq;              // Número total de registros no arquivo
        int nroRegRem;              // Número de registros removidos
    };

    struct registro_ {
        char removido;              // '0' não removido / '1' removido
        int tamanhoRegistro;        // Tamanho do registro
        long int prox;              // Ponteiro para o próximo registro
        int id;                     // ID do registro
        int idade;                  // Idade do jogador
        int tamNomeJog;             // Tamanho do nome do jogador
        char *nomeJogador;          // Nome do jogador
        int tamNacionalidade;       // Tamanho da nacionalidade
        char *nacionalidade;        // Nacionalidade do jogador
        int tamNomeClube;           // Tamanho do nome do clube
        char *nomeClube;            // Nome do clube
    };

    struct cabecalho_ind_ {
        char status;                // '0' para inconsistente / '1' para OK
    };

    struct registro_ind_ {
        int id;                     // ID do registro
        long int byteOffset;             // Byte offset do registro de dados referente ao id
    };

    // Funções fornecidas
    void binarioNaTela(char *nomeArquivoBinario); // Exibe o conteúdo de um arquivo binário na tela
    void scan_quote_string(char *str); // Trata strings com aspas no formato CSV
    
    // Funções auxiliares 

    // Função auxiliar para abrir e testar o arquivo
    int testarArquivo(FILE *arquivo, char *nomeArquivo); 
    // Insere um registro no arquivo binário
    void inserirRegistro(REGISTRO *registro, FILE *arquivo); 
    // Aloca memória para um registro
    void alocarRegistro(REGISTRO **registro, int maxNomeJog, int maxNacionalidade, int maxNomeClube); 
    // Desaloca a memória alocada para um registro
    void desalocarRegistro(REGISTRO **registro); 
    // Função auxiliar para recuperar um registro do arquivo binário
    void recuperarRegistro(REGISTRO **registro, FILE *arquivo, int byteOffset, int *maxNomeJog, int *maxNacionalidade, int *maxNomeClube);
    // Função auxiliar para recuperar o índice do arquivo, alocando em memória primária
    REGISTRO_IND *recoverIndex(FILE *arquivo, FILE *indice);
    // Função para processar os comandos de busca
    void comandoBusca(int *nroComandos, char (*comando)[5][20], char (*palavraChave)[5][100]);
    // Função de buscas nos registros
    int busca(REGISTRO *registro, REGISTRO_IND *vetorInd, int nroComandos, char comando[5][20], char palavraChave[5][100]);
   

    // Funções principais

    // Lista o arquivo de saída no modo binário usando a função fornecida binarioNaTela (Funcionalidade 1)
    void createTable(char *nomeArquivoCsv, char *nomeArquivoBin);
    // Lista todos os dados de um arquivo de entrada de forma organizada (Funcionalidade 2)
    void selectFrom(char *nomeArquivo);
    // Função que realiza n buscas em um dado arquivo (Funcionalidade 3)
    void selectFromWhere(char *nomeArquivo, char *nomeIndice, int nroBuscas);
    // Função que cria um índice a partir da chave primária do arquivo de dados (Funcionalidade 4)
    void createIndex(char *nomeArquivo, char *nomeIndice);
    // Função responsável por remover dados de um arquivo (Funcionalidade 5)
    void deleteFromWhere(char *nomeArquivo, char *nomeIndice, int nroRemocoes);
    // Função responsável por inserir dados em um arquivo (Funcionalidade 6)
    void insertInto(char *nomeArquivo, char *nomeIndice, int nroAdicoes);
    
#endif // FUNCOES_H
