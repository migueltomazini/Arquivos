#include "funcoes.h"            // Inclui o arquivo de cabeçalho "funcoes.h" que contém as definições de funções auxiliares.
#include "funcoesAuxiliares.h"  // Inclui o arquivo de cabeçalho "funcoesAuxiliares.h" que contém as definições de funções adicionais.
#include "bTree.h"              // Inclui o arquivo de cabeçalho "bTree.h" que contém as definições relacionadas à estrutura da árvore-B.

// Função para selecionar registros da árvore-B usando um arquivo de índice.
void selectFromBTree(char* nomeArquivo, char* nomeIndice, int n) {
    FILE *arquivoDados = fopen(nomeArquivo, "rb"); // Abre o arquivo de dados para leitura binária.
    if (arquivoDados == NULL) { // Verifica se a abertura do arquivo falhou.
        printf("Falha no processamento do arquivo.\n"); // Imprime mensagem de erro.
        exit(0); // Encerra o programa com erro.
    }

    CABECALHO cabecalho; // Declaração do cabeçalho do arquivo de dados.
    ler_cabecalho(arquivoDados, &cabecalho); // Lê o cabeçalho do arquivo de dados.
    if (cabecalho.status == '0') { // Verifica se o status do cabeçalho é inválido.
        printf("Falha no processamento do arquivo.\n"); // Imprime mensagem de erro.
        fclose(arquivoDados); // Fecha o arquivo de dados.
        exit(0); // Encerra o programa com erro (corrigido para 1 para indicar erro).
    }

    FILE *arquivoIndice = fopen(nomeIndice, "rb"); // Abre o arquivo de índice para leitura binária.
    if (arquivoIndice == NULL) { // Verifica se a abertura do arquivo falhou.
        printf("Falha no processamento do arquivo.\n"); // Imprime mensagem de erro.
        fclose(arquivoDados); // Fecha o arquivo de dados.
        exit(0); // Encerra o programa com erro.
    }

    BTREE arvore; // Declaração da estrutura da árvore-B.
    recuperarCabecalhoArvB(arquivoIndice, &arvore); // Lê o cabeçalho do arquivo de índice.
    if (arvore.status == '0') { // Verifica se o status do cabeçalho é inválido.
        printf("Falha no processamento do arquivo.\n"); // Imprime mensagem de erro.
        fclose(arquivoDados); // Fecha o arquivo de dados.
        fclose(arquivoIndice); // Fecha o arquivo de índice.
        exit(0); // Encerra o programa com erro.
    }

    int vetor[n]; // Declaração de um vetor para armazenar as chaves de busca.
    for (int i = 0; i < n; i++){ // Loop para ler as chaves de busca.
        char id[2]; 
        int chave;
        scanf("%s %d", id, &chave); // Lê a chave de busca.
        vetor[i]=chave; // Armazena a chave no vetor.
    }

    for (int i = 0; i < n; i++) { // Loop para realizar a busca de cada chave.
        long int byteOffset = buscarChave(&arvore, arquivoIndice, arvore.noRaiz, vetor[i]); // Busca a chave na árvore-B.
        if (byteOffset == -1) { // Verifica se a chave não foi encontrada.
            printf("BUSCA %d\n\n", i + 1);
            printf("Registro inexistente.\n\n");
        } else {
            fseek(arquivoDados, byteOffset, SEEK_SET); // Posiciona o ponteiro no byte offset encontrado.
            REGISTRO registro; // Declaração do registro.
            ler_registro(&registro, arquivoDados); // Lê o registro do arquivo de dados.
            // Verifica se o registro foi removido logicamente.
            if (registro.removido == '1') {
                printf("BUSCA %d\n\n", i + 1);
                printf("Registro inexistente.\n\n");
            } else {
                // Imprime o registro recuperado.
                printf("BUSCA %d\n\n", i + 1);
                impressaoRegistro(&registro);
            }
        }
    }

    fclose(arquivoDados); // Fecha o arquivo de dados.
    fclose(arquivoIndice); // Fecha o arquivo de índice.
}

