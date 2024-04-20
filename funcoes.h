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

    // Funções fornecidas
    void binarioNaTela(char *nomeArquivoBinario); // Exibe o conteúdo de um arquivo binário na tela
    void scan_quote_string(char *str); // Trata strings com aspas no formato CSV

    // Funções auxiliares 
    void inserirRegistro(REGISTRO *registro, FILE *arquivo); // Insere um registro no arquivo binário
    void alocarRegistro(REGISTRO **registro, int maxNomeJog, int maxNacionalidade, int maxNomeClube); // Aloca memória para um registro
    void desalocarRegistro(REGISTRO **registro); // Desaloca a memória alocada para um registro

    // Lista o arquivo de saída no modo binário usando a função fornecida binarioNaTela (Funcionalidade 1)
    void createTable(char *nomeArquivoCsv, char *nomeArquivoBin);
    // Lista todos os dados de um arquivo de entrada de forma organizada (Funcionalidade 2)
    void selectFrom(char *nomeArquivo);
    // Função que realiza n buscas em um dado arquivo (Funcionalidade 3)
    void selectFromWhere(char *nomeArquivo, int nroBuscas);
    
#endif // FUNCOES_H
