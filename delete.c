#include "funcoes.h" // Inclui o cabeçalho com as funções principais
#include "funcoesAuxiliares.h" // Inclui o cabeçalho com as funções auxiliares

// Função responsável por marcar um registro como logicamente removido
void removeRegistro(FILE *arquivo, REGISTRO *registro, int byteOffset) {
    if (registro == NULL)
        return; // Retorna se o registro for nulo

    fseek(arquivo, byteOffset, SEEK_SET); // Posiciona o ponteiro no byte offset especificado
    fwrite("1", sizeof(char), 1, arquivo); // Escreve '1' para marcar o registro como removido
}

// Função responsável por remover registros do arquivo a partir de n buscas que utilizam m campos em cada uma delas (Funcionalidade 5)
void deleteFromWhere(char *nomeArquivo, char *nomeIndice, int nroRemocoes) {
    char comando[5][20]; // Matriz para armazenar os comandos de busca (Ex: "id", "nomeJogador")
    char palavraChave[5][100]; // Matriz para armazenar as palavras-chave de busca
    char c = '\0'; // Caractere temporário para leitura

    int nroComandos = 0; // Número de comandos de busca em cada busca
    int retornoBusca = 0; // Retorno da função de busca
    long int byteOffset = TAM_CABECALHO; // Armazena o byte atual do arquivo

    // Tamanhos máximos iniciais dos campos de texto
    int maxNomeJog = 30;
    int maxNacionalidade = 30;
    int maxNomeClube = 30;

    int nroRegistros = 0; // Número de registros totais
    int nroRegistrosRemovidos = 0; // Número de registros removidos no arquivo
    int tamRegRem = 0; // Armazena o tamanho do registro a ser removido
    int idIndex = 0; // Armazena a posição na qual o comando é id, caso exista

    // Abertura e testagem do arquivo principal
    FILE *arquivo = NULL; // Ponteiro para o arquivo binário
    if ((arquivo = fopen(nomeArquivo, "r+b")) == NULL) { // Abre o arquivo para leitura e escrita em binário
        printf("Falha no processamento do arquivo.\n"); // Imprime mensagem de erro
        exit(1); // Encerra o programa, caso haja falha no processamento
    }

    if (testarArquivo(arquivo) == 1) // Testa se o arquivo é válido
        exit(1); // Encerra o programa, caso haja falha no processamento 

    // Cria o arquivo de índice para uso nas buscas a serem realizadas na função
    createIndex(nomeArquivo, nomeIndice);

    FILE *indice = NULL; // Ponteiro para o arquivo de índice
    if ((indice = fopen(nomeIndice, "rb")) == NULL) { // Abre o arquivo de índice para leitura em binário
        printf("Falha no processamento do indice.\n"); // Imprime mensagem de erro
        fclose(arquivo); // Fecha o arquivo principal
        return;
    }

    REGISTRO *registro; // Declaração de um ponteiro para o registro
    alocarRegistro(&registro, maxNomeJog, maxNacionalidade, maxNomeClube); // Aloca memória para o registro

    REGISTRO_IND *vetorInd; // Ponteiro para o vetor de índices
    vetorInd = recoverIndex(arquivo, indice); // Recupera o índice do arquivo de dados

    // Recuperar número de registros no arquivo
    fseek(arquivo, 17, SEEK_SET); // Posiciona o ponteiro no campo do número de registros
    fread(&nroRegistros, sizeof(int), 1, arquivo); // Lê o número de registros

    // Loop para realizar as remoções especificadas
    for (int j = 1; j <= nroRemocoes; j++) {
        byteOffset = TAM_CABECALHO; // Reinicia o contador do Byte Offset

        fseek(arquivo, byteOffset, SEEK_SET); // Volta ao início do arquivo, após o cabeçalho
        idIndex = comandoBusca(&nroComandos, &comando, &palavraChave); // Lê os comandos de busca e as palavras-chave

        // Trata os casos nos quais a remoção envolve um id, podendo ser realizadas a partir do arquivo de índice
        if (strcmp(comando[idIndex], "id") == 0) { // Verifica se o comando é "id"
            byteOffset = buscaId(registro, vetorInd, nroRegistros, nroComandos, comando, palavraChave); // Busca o registro pelo ID
            if (byteOffset != -1) { // Se o registro foi encontrado
                recuperarRegistro(&registro, arquivo, byteOffset, &maxNomeJog, &maxNacionalidade, &maxNomeClube); // Recupera o registro do arquivo
                retornoBusca = busca(registro, vetorInd, nroComandos, comando, palavraChave); // Realiza a busca no registro

                if (retornoBusca == 0) { // Se a busca encontrou o registro
                    removeRegistro(arquivo, registro, byteOffset); // Marca o registro como removido
                    updateList(arquivo, byteOffset, registro->tamanhoRegistro); // Atualiza a lista encadeada de registros removidos
                    fseek(arquivo, byteOffset + registro->tamanhoRegistro, SEEK_SET); // Retorna a posição do arquivo anterior à chamada das funções
                    nroRegistrosRemovidos++; // Incrementa o contador de registros removidos
                }
            }
            continue; // Continua para a próxima remoção
        }

        // Loop para ler e processar os registros do arquivo
        while (!feof(arquivo)) { // Lê cada caractere do arquivo até encontrar o final do arquivo (EOF)
            recuperarRegistro(&registro, arquivo, -1, &maxNomeJog, &maxNacionalidade, &maxNomeClube); // Recupera o registro do arquivo

            if (registro->removido == '0') { // Verifica se o registro não foi removido
                retornoBusca = busca(registro, vetorInd, nroComandos, comando, palavraChave); // Realiza a busca no registro

                // Verifica o resultado da busca e remove o registro se correspondente
                if (retornoBusca == 0 || retornoBusca == 2) {
                    removeRegistro(arquivo, registro, byteOffset); // Marca o registro como removido
                    updateList(arquivo, byteOffset, registro->tamanhoRegistro); // Atualiza a lista encadeada de registros removidos
                    fseek(arquivo, byteOffset + registro->tamanhoRegistro, SEEK_SET); // Retorna a posição do arquivo anterior à chamada das funções
                    nroRegistrosRemovidos++; // Incrementa o contador de registros removidos
                }
            }
            byteOffset += registro->tamanhoRegistro; // Recalcula o novo Byte offset
        }
    }

    // Atualiza o cabeçalho de acordo com as remoções
    updateCabecalho(arquivo, nroRegistrosRemovidos);

    // Recria o arquivo de índice, de forma a desconsiderar os registros removidos durante essa função
    createIndex(nomeArquivo, nomeIndice);
    free(vetorInd); // Libera a memória alocada para o vetor de índices
    desalocarRegistro(&registro); // Libera a memória alocada para o registro
    fclose(arquivo); // Fecha o arquivo principal
    fclose(indice); // Fecha o arquivo de índice
}
