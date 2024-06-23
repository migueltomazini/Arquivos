#include "funcoes.h" // Inclui o cabeçalho com as funções principais
#include "funcoesAuxiliares.h" // Inclui o cabeçalho com as funções auxiliares

// Função de ordenação Bubble Sort para o vetor de ponteiros para REGISTRO_IND
void bubblesort(REGISTRO_IND* vetor[], int n) {
    int i, j; // Variáveis de controle para os loops
    REGISTRO_IND* aux; // Ponteiro auxiliar para a troca de elementos
    for (i = 0; i < n; i++) { // Loop externo do Bubble Sort
        for (j = 0; j < n - 1; j++) { // Loop interno do Bubble Sort
            if (vetor[j]->id > vetor[j + 1]->id) { // Compara os IDs dos registros
                aux = vetor[j]; // Armazena o elemento atual no auxiliar
                vetor[j] = vetor[j + 1]; // Move o próximo elemento para a posição atual
                vetor[j + 1] = aux; // Coloca o elemento armazenado no auxiliar na posição seguinte
            }
        }
    }
}

// Função para criar o arquivo de índice a partir do arquivo de dados
void createIndex(char *nomeArquivo, char *nomeIndice) {
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

    CABECALHO_IND cabecalhoIndice; // Declaração do cabeçalho do arquivo de índice
    cabecalhoIndice.status = '0'; // Define o status inicial do cabeçalho do índice como '0' (inválido)
    fwrite(&cabecalhoIndice.status, sizeof(char), 1, arquivoIndice); // Escreve o status no arquivo de índice

    int cont = cabecalho.nroRegArq + cabecalho.nroRegRem; // Calcula o total de registros (incluindo removidos)

    REGISTRO_IND* vetor[cabecalho.nroRegArq]; // Declaração do vetor de ponteiros para REGISTRO_IND
    int aux = 0; // Variável auxiliar para contar os registros válidos

    for (int i = 0; i < cont; i++) { // Loop para processar todos os registros
        long int byteOffset = ftell(arquivoDados); // Armazena o byte offset do início do registro
        REGISTRO* registro = malloc(sizeof(REGISTRO)); // Aloca memória para um registro
        ler_registro(registro, arquivoDados); // Lê o registro do arquivo de dados
        ler_lixo(arquivoDados); // Lê possíveis espaços vazios após o registro
        if (registro->removido == '0') { // Verifica se o registro não foi removido
            REGISTRO_IND* registroIndice = malloc(sizeof(REGISTRO_IND)); // Aloca memória para um registro de índice
            registroIndice->id = registro->id; // Atribui o ID do registro ao registro de índice
            registroIndice->byteOffset = byteOffset; // Atribui o byte offset do registro ao registro de índice
            vetor[aux] = registroIndice; // Adiciona o registro de índice ao vetor
            aux++; // Incrementa o contador de registros válidos
        }
        free(registro); // Libera a memória alocada para o registro
    }

    bubblesort(vetor, aux); // Ordena o vetor de registros de índice usando Bubble Sort

    for (int i = 0; i < aux; i++) { // Loop para escrever os registros de índice no arquivo de índice
        fwrite(&vetor[i]->id, sizeof(int), 1, arquivoIndice); // Escreve o ID do registro de índice
        fwrite(&vetor[i]->byteOffset, sizeof(long int), 1, arquivoIndice); // Escreve o byte offset do registro de índice
        free(vetor[i]); // Libera a memória alocada para o registro de índice
    }

    fseek(arquivoIndice, 0, SEEK_SET); // Posiciona o ponteiro no início do arquivo de índice
    cabecalhoIndice.status = '1'; // Define o status do cabeçalho do índice como '1' (válido)
    fwrite(&cabecalhoIndice.status, sizeof(char), 1, arquivoIndice); // Escreve o status atualizado no arquivo de índice

    fclose(arquivoDados); // Fecha o arquivo de dados
    fclose(arquivoIndice); // Fecha o arquivo de índice
}

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
}