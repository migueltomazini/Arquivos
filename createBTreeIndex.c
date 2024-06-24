#include "funcoes.h"
#include "funcoesAuxiliares.h"
#include "bTree.h"

void createBTreeIndex(char *nomeArquivo, char *nomeIndice) {
    FILE *arquivoDados = fopen(nomeArquivo, "rb"); // Abre o arquivo de dados para leitura binária
    if (arquivoDados == NULL) { // Verifica se a abertura do arquivo falhou
        printf("Falha no processamento do arquivo.\n"); // Imprime mensagem de erro
        exit(1); // Encerra o programa com erro
    }

    CABECALHO cabecalho; // Declaração do cabeçalho do arquivo de dados
    ler_cabecalho(arquivoDados, &cabecalho); // Lê o cabeçalho do arquivo de dados
    if (cabecalho.status == '0') { // Verifica se o status do cabeçalho é inválido
        printf("Falha no processamento do arquivo.\n"); // Imprime mensagem de erro
        fclose(arquivoDados); // Fecha o arquivo de dados
        exit(0); // Encerra o programa com erro
    }

    FILE *arquivoIndice = fopen(nomeIndice, "wb"); // Abre o arquivo de índice para escrita binária
    if (arquivoIndice == NULL) { // Verifica se a abertura do arquivo falhou
        printf("Falha no processamento do arquivo.\n"); // Imprime mensagem de erro
        fclose(arquivoDados); // Fecha o arquivo de dados
        exit(1); // Encerra o programa com erro
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
    
    int bytOffset = TAM_CABECALHO;
    fseek(arquivoDados, bytOffset, SEEK_SET);
    
    while (!feof(arquivoDados)) {
        recuperarRegistro(&registro, arquivoDados, bytOffset, &maxNomeJog, &maxNacionalidade, &maxNomeClube);
        if (registro->removido == '0')
            inserirChave(&arvore, nomeIndice, registro->id, bytOffset);
        bytOffset += registro->tamanhoRegistro;
        //bytOffset += ler_lixo(arquivoDados);
        printf("%d", bytOffset);
    }

    fseek(arquivoIndice, 0, SEEK_SET); // Posiciona o ponteiro no início do arquivo de índice
    arvore.status = '1'; // Define o status do cabeçalho do índice como '1' (válido)
    fwrite(&arvore.status, sizeof(char), 1, arquivoIndice); // Escreve o status atualizado no arquivo de índice

    free(registro);
    fclose(arquivoDados);
    fclose(arquivoIndice);
}