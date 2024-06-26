#include "funcoes.h"
#include "funcoesAuxiliares.h"
#include "bTree.h"

void createBTreeIndex(char *nomeArquivo, char *nomeIndice) {
    FILE *arquivoDados = fopen(nomeArquivo, "rb"); // Abre o arquivo de dados para leitura binária
    if (arquivoDados == NULL) { // Verifica se a abertura do arquivo falhou
        printf("Falha no processamento do arquivo.\n"); // Imprime mensagem de erro
        exit(0);
    }

    CABECALHO cabecalho; // Declaração do cabeçalho do arquivo de dados
    ler_cabecalho(arquivoDados, &cabecalho); // Lê o cabeçalho do arquivo de dados
    if (cabecalho.status == '0') { // Verifica se o status do cabeçalho é inválido
        printf("Falha no processamento do arquivo.\n"); // Imprime mensagem de erro
        fclose(arquivoDados); // Fecha o arquivo de dados
        exit(0);
    }

    FILE *arquivoIndice = fopen(nomeIndice, "wb"); // Abre o arquivo de índice para escrita binária
    if (arquivoIndice == NULL) { // Verifica se a abertura do arquivo falhou
        printf("Falha no processamento do arquivo.\n"); // Imprime mensagem de erro
        fclose(arquivoDados); // Fecha o arquivo de dados
        exit(0);
    }

    // Tamanhos máximos iniciais dos campos de texto
    int maxNomeJog = 100;
    int maxNacionalidade = 100;
    int maxNomeClube = 100;

    BTREE arvore;
    inicializarArvoreB(&arvore, "arvoreB.bin");

    arvore.status = '0'; // Define o status inicial do cabeçalho do índice como '0' (inválido)
    fwrite(&arvore.status, sizeof(char), 1, arquivoIndice); // Escreve o status no arquivo de índice

    REGISTRO *registro = NULL;
    alocarRegistro(&registro, maxNomeJog, maxNacionalidade, maxNomeClube);
    
    int byteOffset = TAM_CABECALHO;
    fseek(arquivoDados, byteOffset, SEEK_SET);
    
    // Atribuição das chaves na árvore B enquanto não chegar ao fim do arquivo de dados
    while (!feof(arquivoDados)) {
        recuperarRegistro(&registro, arquivoDados, byteOffset, &maxNomeJog, &maxNacionalidade, &maxNomeClube);
        if (registro->removido == '0')
            inserirChave(&arvore, nomeIndice, registro->id, byteOffset);
        byteOffset += registro->tamanhoRegistro;
        byteOffset += ler_lixo(arquivoDados);
    }

    fseek(arquivoIndice, 0, SEEK_SET); // Posiciona o ponteiro no início do arquivo de índice
    arvore.status = '1'; // Define o status do cabeçalho do índice como '1' (válido)
    fwrite(&arvore.status, sizeof(char), 1, arquivoIndice); // Escreve o status atualizado no arquivo de índice

    free(registro);
    fclose(arquivoDados);
    fclose(arquivoIndice);
}