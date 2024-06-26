#include "funcoes.h"
#include "funcoesAuxiliares.h"

void insertbtree(char *nomeArquivo, char *nomeIndice, int nroAdicoes) {
    // Abrir o arquivo de índice no modo de leitura e escrita binária.
    FILE *arquivoIndice = fopen(nomeIndice, "rb+");
    if (arquivoIndice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        exit(1);
    }
    
    // Abrir o arquivo de dados no modo de leitura e escrita binária.
    FILE *arquivo = fopen(nomeArquivo, "rb+");
    if (arquivo == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivoIndice);
        exit(1);
    }

    // Variáveis para armazenar registros e nós da árvore-B.
    REGISTRO registro;
    BTREE arvoreB;
    NO_BTREE novoNo;
   
    criarNo(&novoNo);  // Corrigir para passar o endereço do novo nó.
   

    for (int i = 0; i < nroAdicoes; i++) {
        // Inserir o registro no arquivo de dados usando a função auxiliar.
        insertIntoAux(nomeArquivo);
        // Ler o registro do arquivo de dados.
        ftell(arquivo);
        ler_registro(&registro, arquivo);
        // Inserir a chave no arquivo de índice árvore-B.
        inserirChave(&arvoreB, nomeIndice, registro.id, ftell(arquivo));
        
    }

}
