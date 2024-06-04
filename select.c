#include "funcoes.h"
#include "funcoesAuxiliares.h"

// Função para listar todos os registros de um arquivo binário (Funcionalidade 2)
void selectFrom(char *nomeArquivo) {
    char c = '\0'; // Caractere temporário para leitura
    int nroRegistros = 0; // Contador de registros

    // Tamanhos máximos iniciais dos campos de texto
    int maxNomeJog = 30;
    int maxNacionalidade = 30;
    int maxNomeClube = 30;

    REGISTRO *registro; // Declaração de um ponteiro para o registro
    alocarRegistro(&registro, maxNomeJog, maxNacionalidade, maxNomeClube); // Aloca memória para o registro

    FILE *arquivo = NULL; // Ponteiro para o arquivo binário
    arquivo = fopen(nomeArquivo, "rb"); // Abre o arquivo no modo leitura binária

    // Verifica se o arquivo foi aberto corretamente e se o processamento falhou
    if (registro == NULL || testarArquivo(arquivo) == 1) {
        desalocarRegistro(&registro); // Libera a memória alocada para o registro
        exit (0); // Retorna se houve falha no processamento
    }

    fseek(arquivo, TAM_CABECALHO, SEEK_SET); // Posiciona o ponteiro do arquivo após o cabeçalho

    // Loop para ler e imprimir os registros do arquivo
    while (!feof(arquivo)) { // Lê cada caractere do arquivo até encontrar o final do arquivo (EOF)     
        recuperarRegistro(&registro, arquivo, -1, &maxNomeJog, &maxNacionalidade, &maxNomeClube); // Recupera o registro do arquivo

        if (registro->removido == '0') { // Verifica se o registro não foi removido
            impressaoRegistro(registro); // Imprime os campos do registro
            nroRegistros++; // Incrementa o contador de registros
        }
    }

    if (nroRegistros == 0) // Verifica se nenhum registro foi encontrado
        printf("Registro inexistente.\n\n"); // Imprime uma mensagem indicando que nenhum registro foi encontrado

    desalocarRegistro(&registro); // Libera a memória alocada para o registro

    fclose(arquivo); // Fecha o arquivo após a leitura
}

// Função que realiza n buscas em um dado arquivo (Funcionalidade 3)
void selectFromWhere(char *nomeArquivo, char *nomeIndice, int nroBuscas) {
    char comando[5][20]; // Matriz para armazenar os comandos de busca (Ex: "id", "nomeJogador")
    char palavraChave[5][100]; // Matriz para armazenar as palavras-chave de busca
    char c = '\0'; // Caractere temporário para leitura

    int nroComandos = 0; // Número de comandos de busca em cada busca
    int nroRegistros = 0; // Número de registros totais 
    int nroRegistrosEncontrados = 0; // Número de registros encontrados em uma busca
    
    int retornoBusca = 0; // Retorno da função de busca
    int byteOffset = 0; // Endereço do registro para busca com id
    int idIndex = 0; // Armazena a posição na qual o comando é id, caso exista

    // Tamanhos máximos iniciais dos campos de texto
    int maxNomeJog = 30;
    int maxNacionalidade = 30;
    int maxNomeClube = 30;

    // Cria o arquivo de índice, para uso nas buscas a serem realizadas na função
    createIndex(nomeArquivo, nomeIndice);

    // Abertura e testagem do arquivo principal
    FILE *arquivo = NULL; // Ponteiro para o arquivo binário
    if ((arquivo = fopen(nomeArquivo, "rb")) == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Abertura e testagem do arquivo de índice
    FILE *indice = NULL; // Ponteiro para o arquivo binário
    if ((indice = fopen(nomeIndice, "rb")) == NULL) {
        printf("Falha no processamento do indice.\n");
        return;
    }

    REGISTRO *registro; // Declaração de um ponteiro para o registro
    alocarRegistro(&registro, maxNomeJog, maxNacionalidade, maxNomeClube); // Aloca memória para o registro

    REGISTRO_IND *vetorInd;
    vetorInd = recoverIndex(arquivo, indice);

    // Verifica se os arquivos estão integros e se a alocação de registro e vetorInd funcionou
    if (testarArquivo(arquivo) == 1 || testarArquivo(indice) == 1 || 
        registro == NULL || vetorInd == NULL) {
        desalocarRegistro(&registro); // Libera a memória alocada para o registro
        free(vetorInd);
        return; // Retorna se houve falha no processamento
    }

    // Recuperar número de registros nó arquivo
    fseek(arquivo, 17, SEEK_SET);
    fread(&nroRegistros, sizeof(int), 1, arquivo);  

    // Loop para realizar as buscas especificadas
    for (int j = 1; j <= nroBuscas; j++) {
        nroRegistrosEncontrados = 0; // Reinicia o contador de registros encontrados

        fseek(arquivo, TAM_CABECALHO, SEEK_SET); // Volta ao início do arquivo, após o cabeçalho
        idIndex = comandoBusca(&nroComandos, &comando, &palavraChave); // Lê os comandos de busca e as palavras-chave

        printf("Busca %d\n\n", j); // Imprime o número da busca

        // Trata os casos nos quais a busca envolvem um id, podendo ser realizadas a partir do arquivo
        // de índice
        if (strcmp(comando[idIndex], "id") == 0) {
            byteOffset = buscaId(registro, vetorInd, nroRegistros, nroComandos, comando, palavraChave);
            if (byteOffset != -1) {

                recuperarRegistro(&registro, arquivo, byteOffset, &maxNomeJog, &maxNacionalidade, &maxNomeClube);
                retornoBusca = busca(registro, vetorInd, nroComandos, comando, palavraChave);

                if (retornoBusca == 0) {
                    impressaoRegistro(registro); // Imprime os campos do registro
                    nroRegistrosEncontrados++; // Incrementa o contador de registros encontrados
                }
            }
            continue;
        }

        // Loop para ler e processar os registros do arquivo
        while (!feof(arquivo)) { // Lê cada caractere do arquivo até encontrar o final do arquivo (EOF)
            recuperarRegistro(&registro, arquivo, -1, &maxNomeJog, &maxNacionalidade, &maxNomeClube); // Recupera o registro do arquivo

            if (registro->removido == '0') { // Verifica se o registro não foi removido
                retornoBusca = busca(registro, vetorInd, nroComandos, comando, palavraChave); // Realiza a busca no registro

                // Verifica o resultado da busca e imprime o registro se correspondente
                if (retornoBusca == 0) {
                    impressaoRegistro(registro); // Imprime os campos do registro
                    nroRegistrosEncontrados++; // Incrementa o contador de registros encontrados
                }
            }
        }

        // Verifica se nenhum registro foi encontrado durante a busca atual
        if (nroRegistrosEncontrados == 0)
            printf("Registro inexistente.\n\n"); // Imprime uma mensagem indicando que nenhum registro foi encontrado
    }

    // Recria o arquivo de índice, de forma a descondiderar os registros removidos durante essa função
    createIndex(nomeArquivo, nomeIndice);

    free(vetorInd);
    desalocarRegistro(&registro); // Libera a memória alocada para o registro
    fclose(arquivo); // Fecha o arquivo após a leitura
}
