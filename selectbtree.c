#include "funcoes.h"            // Inclui o arquivo de cabeçalho "funcoes.h" que contém as definições de funções auxiliares.
#include "funcoesAuxiliares.h"  // Inclui o arquivo de cabeçalho "funcoesAuxiliares.h" que contém as definições de funções adicionais.
#include "bTree.h"              // Inclui o arquivo de cabeçalho "bTree.h" que contém as definições relacionadas à estrutura da árvore-B.

// Função para selecionar registros da árvore-B usando um arquivo de índice.
void selectFromBTree(char* nomeArquivo, char* nomeIndice, int n) {
    FILE *arquivoDados = fopen(nomeArquivo, "rb"); // Abre o arquivo de dados para leitura binária.
    if (arquivoDados == NULL) { // Verifica se a abertura do arquivo falhou.
        printf("Falha no processamento do arquivo.\n"); // Imprime mensagem de erro.
        exit(1); // Encerra o programa com erro.
    }

    CABECALHO cabecalho; // Declaração do cabeçalho do arquivo de dados.
    ler_cabecalho(arquivoDados, &cabecalho); // Lê o cabeçalho do arquivo de dados.
    if (cabecalho.status == '0') { // Verifica se o status do cabeçalho é inválido.
        printf("Falha no processamento do arquivo.\n"); // Imprime mensagem de erro.
        fclose(arquivoDados); // Fecha o arquivo de dados.
        exit(1); // Encerra o programa com erro (corrigido para 1 para indicar erro).
    }

    FILE *arquivoIndice = fopen(nomeIndice, "rb"); // Abre o arquivo de índice para leitura binária.
    if (arquivoIndice == NULL) { // Verifica se a abertura do arquivo falhou.
        printf("Falha no processamento do arquivo.\n"); // Imprime mensagem de erro.
        fclose(arquivoDados); // Fecha o arquivo de dados.
        exit(1); // Encerra o programa com erro.
    }

    BTREE arvore; // Declaração da estrutura da árvore-B.
    recuperarCabecalhoArvB(arquivoIndice, &arvore); // Lê o cabeçalho do arquivo de índice.
    if (arvore.status == '0') { // Verifica se o status do cabeçalho é inválido.
        printf("Falha no processamento do arquivo.\n"); // Imprime mensagem de erro.
        fclose(arquivoDados); // Fecha o arquivo de dados.
        fclose(arquivoIndice); // Fecha o arquivo de índice.
        exit(1); // Encerra o programa com erro.
    }

    int vetor[n]; // Declaração de um vetor para armazenar as chaves de busca.
    for (int i = 0; i < n; i++){ // Loop para ler as chaves de busca.
        char id[2]; 
        int chave;
        scanf("%s %d", id, &chave); // Lê a chave de busca.
        vetor[i]=chave; // Armazena a chave no vetor.
    }

    for (int i = 0; i < n; i++) { // Loop para realizar a busca de cada chave.
        long byteOffset = buscarChave(&arvore, arquivoIndice, arvore.noRaiz, vetor[i]); // Busca a chave na árvore-B.
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

// Função para verificar se um registro corresponde aos critérios de busca.
int verificarCriterios(REGISTRO *registro, int nroComandos, char comando[][20], char palavraChave[][100]) {
    for (int i = 0; i < nroComandos; i++) { // Loop para verificar cada critério de busca.
        if (strcmp(comando[i], "id") == 0 && registro->id != atoi(palavraChave[i])) {
            return 0;
        } else if (strcmp(comando[i], "nomeJogador") == 0 && strcmp(registro->nomeJogador, palavraChave[i]) != 0) {
            return 0;
        } else if (strcmp(comando[i], "nacionalidade") == 0 && strcmp(registro->nacionalidade, palavraChave[i]) != 0) {
            return 0;
        } else if (strcmp(comando[i], "nomeClube") == 0 && strcmp(registro->nomeClube, palavraChave[i]) != 0) {
            return 0;
        } else if (strcmp(comando[i], "idade") == 0 && registro->idade != atoi(palavraChave[i])) {
            return 0;
        }
    }
    return 1; // Retorna 1 se todos os critérios forem satisfeitos.
}

// Função para selecionar registros da árvore-B de acordo com critérios de busca.
void selectwhereBTree(char *nomeArquivo, char *nomeIndice, int nroBuscas) {
    FILE *arquivoDados = fopen(nomeArquivo, "rb"); // Abre o arquivo de dados para leitura binária.
    if (arquivoDados == NULL) { // Verifica se a abertura do arquivo falhou.
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    FILE *arquivoIndice = fopen(nomeIndice, "rb"); // Abre o arquivo de índice para leitura binária.
    if (arquivoIndice == NULL) { // Verifica se a abertura do arquivo falhou.
        printf("Falha no processamento do índice.\n");
        fclose(arquivoDados);
        return;
    }

    BTREE arvore; // Declaração da estrutura da árvore-B.
    recuperarCabecalhoArvB(arquivoIndice, &arvore); // Lê o cabeçalho do arquivo de índice.
    if (arvore.status == '0') { // Verifica se o status do cabeçalho é inválido.
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivoDados);
        fclose(arquivoIndice);
        return;
    }

    int maxNomeJog = 30; // Tamanho máximo do nome do jogador.
    int maxNacionalidade = 30; // Tamanho máximo da nacionalidade.
    int maxNomeClube = 30; // Tamanho máximo do nome do clube.
    REGISTRO *registro; // Declaração do ponteiro para o registro.
    alocarRegistro(&registro, maxNomeJog, maxNacionalidade, maxNomeClube); // Aloca memória para o registro.

    for (int j = 1; j <= nroBuscas; j++) { // Loop para realizar cada busca.
        int nroComandos = 0; // Número de comandos de busca.
        char comando[5][20]; // Comandos de busca.
        char palavraChave[5][100]; // Palavras-chave de busca.
        int idIndex = comandoBusca(&nroComandos, &comando, &palavraChave); // Lê os comandos e palavras-chave de busca.

        printf("Busca %d\n\n", j);

        if (strcmp(comando[idIndex], "id") == 0) { // Verifica se o critério de busca é pelo id.
            long byteOffset = buscarChave(&arvore, arquivoIndice, arvore.noRaiz, atoi(palavraChave[idIndex])); // Busca a chave na árvore-B.
            if (byteOffset != -1) { // Verifica se a chave foi encontrada.
                fseek(arquivoDados, byteOffset, SEEK_SET); // Posiciona o ponteiro no byte offset encontrado.
                recuperarRegistro(&registro, arquivoDados, byteOffset, &maxNomeJog, &maxNacionalidade, &maxNomeClube); // Recupera o registro do arquivo de dados.
                if (verificarCriterios(registro, nroComandos, comando, palavraChave)) { // Verifica se o registro atende aos critérios de busca.
                    impressaoRegistro(registro); // Imprime o registro.
                } else {
                    printf("Registro inexistente.\n\n");
                }
            } else {
                printf("Registro inexistente.\n\n");
            }
        } else {
            fseek(arquivoDados, TAM_CABECALHO, SEEK_SET); // Posiciona o ponteiro após o cabeçalho.
            while (!feof(arquivoDados)) { // Loop até o final do arquivo.
                recuperarRegistro(&registro, arquivoDados, -1, &maxNomeJog, &maxNacionalidade, &maxNomeClube); // Recupera o registro do arquivo de dados.
                if (registro->removido == '0' && verificarCriterios(registro, nroComandos, comando, palavraChave)) { // Verifica se o registro não foi removido e atende aos critérios de busca.
                    impressaoRegistro(registro); // Imprime o registro.
                }
            }
        }
    }

    desalocarRegistro(&registro); // Libera a memória alocada para o registro.
    fclose(arquivoDados); // Fecha o arquivo de dados.
    fclose(arquivoIndice); // Fecha o arquivo de índice.
}
