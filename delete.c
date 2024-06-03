#include "funcoes.h"

// Função responsável por marcar um registro como lógicamente removido
void removeRegistro(FILE *arquivo, REGISTRO *registro, int byteOffset) {
    if (registro == NULL)
        return;

    fseek(arquivo, byteOffset, SEEK_SET);
    fwrite("1", sizeof(char), 1, arquivo);
}

// Função responsável por atualizar a lista encadeada de registros removidos, de modo que ela
// esteja ordenada de forma crescente em relação ao tamanho de seus itens
void updateList(FILE * arquivo, long int novo, int tamanhoRegistroNovo) {
    long int atual = 0;             // Armazena o valor do byteOffset de um item da lista
    long int prox = 0;              // Armazenar o valor do byteOffset do sucessor de atual
    int tamanhoRegistroAtual = -1;  // Armazena o tamanho do registro de byteOffset atual

    // Ler o valor armazenado no topo (head)
    fseek(arquivo, 1, SEEK_SET);
    fread(&atual, sizeof(long int), 1, arquivo);

    // Caso o topo esteja vazio, o item novo é adicionado no topo
    if(atual == -1) {
        fseek(arquivo, 1, SEEK_SET);
        fwrite(&novo, sizeof(long int), 1, arquivo);
        return;
    }

    // Ler o valor do tamanho do registro atual
    fseek(arquivo, atual + 1, SEEK_SET);
    fread(&tamanhoRegistroAtual, sizeof(int), 1, arquivo);

    // Trata o caso no qual o registro já deve ser insirido em topo (cabeçalho).
    // Isso ocorre quando o tamanho do registro novo é menor que aquele em topo
    if (tamanhoRegistroNovo >= tamanhoRegistroAtual) {
        fread(&prox, sizeof(long int), 1, arquivo);

        // Atualizar o topo (cabeçalho), atribuindo o valor de novo
        fseek(arquivo, 1, SEEK_SET);
        fwrite(&novo, sizeof(long int), 1, arquivo);

        // Atualizar o proximo em novo de forma que ele "aponte" para atual
        fseek(arquivo, novo + 5, SEEK_SET);
        fwrite(&atual, sizeof(long int), 1, arquivo);

        return;
    }

    // Trata o restante dos casos, em que o registro novo será inserido no meio ou fim
    // da lista. Para isso, percorre-a até que seja encontrado o local para sua inserçao
    while (tamanhoRegistroNovo >= tamanhoRegistroAtual) {
        // Ler o valor do tamanho do registro atual
        fseek(arquivo, atual + 1, SEEK_SET);
        fread(&tamanhoRegistroAtual, sizeof(int), 1, arquivo);

        // Caso chegue ao fim da lista, sai do loop
        if (atual == -1)
            break;

        if (tamanhoRegistroNovo >= tamanhoRegistroAtual)
            fread(&atual, sizeof(long int), 1, arquivo);
    }
    fread(&prox, sizeof(long int), 1, arquivo);

    // Atualiza o valor proximo de novo de forma que ele aponte para prox
    fseek(arquivo, novo + 5, SEEK_SET);
    fwrite(&prox, sizeof(long int), 1, arquivo);
    
    // Atualiza o valor proximo de atual para que ele "aponte" para novo
    fseek(arquivo, atual + 5, SEEK_SET);
    fwrite(&novo, sizeof(long int), 1, arquivo);
}

// Função responsável por atualizar o cabeçalho do arquivo, modificando os valores contendo o
// número de registros presentes e removidos do arquivo
void updateCabecalho(FILE *arquivo, int nroRemocoes) {
    int nroReg;
    int nroRegRem;

    // Realiza a leitura do número de registros presentes e removidos no cabeçalho
    fseek(arquivo, 17, SEEK_SET);
    fread(&nroReg, sizeof(int), 1, arquivo);
    fread(&nroRegRem, sizeof(int), 1, arquivo);

    // Atualiza os valores de acordo com o número de remoções realizadas
    nroReg -= nroRemocoes;
    nroRegRem += nroRemocoes;

    // Reescreve os valores atualizados nos determinados campos 
    fseek(arquivo, 17, SEEK_SET);
    fwrite(&nroReg, sizeof(int), 1, arquivo);
    fwrite(&nroRegRem, sizeof(int), 1, arquivo);
}

// Função responsável por remover registros do arquivo a partir de n buscas que utilizando m campos
// em cada uma delas (Funcionalidade 5)
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
    
    // Cria o arquivo de índice, para uso nas buscas a serem realizadas na função
    createIndex(nomeArquivo, nomeIndice);
    
    // Abertura e testagem do arquivo principal
    FILE *arquivo = NULL; // Ponteiro para o arquivo binário
    if ((arquivo = fopen(nomeArquivo, "r+b")) == NULL) {
        printf("Falha no processamento do arquivo.\n");

        exit(1); // Encerra o programa, caso haja falha no processamento
    }

    if (testarArquivo(arquivo) == 1)
        exit(1); // Encerra o programa, caso haja falha no processamento 

    FILE *indice = NULL;
    if ((indice = fopen(nomeIndice, "rb")) == NULL) {
        printf("Falha no processamento do indice.\n");
        return;
    }  

    REGISTRO *registro; // Declaração de um ponteiro para o registro
    alocarRegistro(&registro, maxNomeJog, maxNacionalidade, maxNomeClube); // Aloca memória para o registro

    REGISTRO_IND *vetorInd;
    vetorInd = recoverIndex(arquivo, indice);

    // Recuperar número de registros nó arquivo
    fseek(arquivo, 17, SEEK_SET);
    fread(&nroRegistros, sizeof(int), 1, arquivo);  

    // Loop para realizar as remoções especificadas
    for (int j = 1; j <= nroRemocoes; j++) {
        byteOffset = TAM_CABECALHO; // Reinicia o contador do Byte Offset

        fseek(arquivo, byteOffset, SEEK_SET); // Volta ao início do arquivo, após o cabeçalho
        idIndex = comandoBusca(&nroComandos, &comando, &palavraChave); // Lê os comandos de busca e as palavras-chave

        // Trata os casos nos quais a remoção envolvem um id, podendo ser realizadas a partir do arquivo
        // de índice
        if (strcmp(comando[idIndex], "id") == 0) {
            byteOffset = buscaId(registro, vetorInd, nroRegistros, nroComandos, comando, palavraChave);
            if (byteOffset != -1) {
                recuperarRegistro(&registro, arquivo, byteOffset, &maxNomeJog, &maxNacionalidade, &maxNomeClube);
                retornoBusca = busca(registro, vetorInd, nroComandos, comando, palavraChave);

                if (retornoBusca == 0) {
                    removeRegistro(arquivo, registro, byteOffset); // Imprime os campos do registro
                    updateList(arquivo, byteOffset, registro->tamanhoRegistro);
                    // Retornar a posição do arquivo anterior a chamada das funções
                    fseek(arquivo, byteOffset + registro->tamanhoRegistro, SEEK_SET);
                    nroRegistrosRemovidos++; // Incrementa o contador de registros encontrados
                }
            }
            continue;
        }

        // Loop para ler e processar os registros do arquivo
        while (!feof(arquivo)) { // Lê cada caractere do arquivo até encontrar o final do arquivo (EOF)
            recuperarRegistro(&registro, arquivo, -1, &maxNomeJog, &maxNacionalidade, &maxNomeClube); // Recupera o registro do arquivo

            if (registro->removido == '0') { // Verifica se o registro já foi removido
                retornoBusca = busca(registro, vetorInd, nroComandos, comando, palavraChave); // Realiza a busca no registro

                // Verifica o resultado da busca e remove o registro se correspondente
                if (retornoBusca == 0 || retornoBusca == 2) {
                    removeRegistro(arquivo, registro, byteOffset); // Remove o registro
                    updateList(arquivo, byteOffset, registro->tamanhoRegistro);
                    // Retornar a posição do arquivo anterior a chamada das funções
                    fseek(arquivo, byteOffset + registro->tamanhoRegistro, SEEK_SET);     
                    nroRegistrosRemovidos++; // Incrementa o contador de registros removidos
                }
            }
            byteOffset += registro->tamanhoRegistro; // Recalcula o novo Byte offset
        }
    }
    // Atualiza o cabeçalho de acordo com as remoçôes
    updateCabecalho(arquivo, nroRegistrosRemovidos);

    // Recria o arquivo de índice, de forma a descondiderar os registros removidos durante essa função
    createIndex(nomeArquivo, nomeIndice);

    free(vetorInd);
    desalocarRegistro(&registro); // Libera a memória alocada para o registro
    fclose(arquivo); // Fecha o arquivo após a leitura
}