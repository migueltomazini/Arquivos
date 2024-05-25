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
    char comando[5][10]; // Matriz para armazenar os comandos de busca (Ex: "id", "nomeJogador")
    char palavraChave[5][100]; // Matriz para armazenar as palavras-chave de busca
    char c = '\0'; // Caractere temporário para leitura

    int nroComandos = 0; // Número de comandos de busca em cada busca
    int retornoBusca = 0; // Retorno da função de busca
    long int byteOffset = TAM_CABECALHO; // Armazena o byte atual do arquivo

    // Tamanhos máximos iniciais dos campos de texto
    int maxNomeJog = 30;
    int maxNacionalidade = 30;
    int maxNomeClube = 30;
    
    int nroRegistrosRemovidos = 0; // Número de registros removidos no arquivo
    int tamRegRem = 0;
    
    FILE *arquivo = NULL; // Ponteiro para o arquivo binário
    arquivo = fopen(nomeArquivo, "r+b"); // Abre o arquivo no modo leitura e escrita binária

    // Cria o arquivo de índice, para uso nas buscas a serem realizadas na função
    createIndex(nomeArquivo, nomeIndice);

    REGISTRO *registro; // Declaração de um ponteiro para o registro
    alocarRegistro(&registro, maxNomeJog, maxNacionalidade, maxNomeClube); // Aloca memória para o registro

    // Verifica se o arquivo foi aberto corretamente e se o processamento falhou
    if (registro == NULL || testarArquivo(arquivo, nomeArquivo) == 1) {
        desalocarRegistro(&registro); // Libera a memória alocada para o registro
        return; // Retorna se houve falha no processamento
    }

    // Loop para realizar as remoções especificadas
    for (int j = 1; j <= nroRemocoes; j++) {
        byteOffset = TAM_CABECALHO; // Reinicia o contador do Byte Offset

        fseek(arquivo, byteOffset, SEEK_SET); // Volta ao início do arquivo, após o cabeçalho
        comandoBusca(&nroComandos, &comando, &palavraChave); // Lê os comandos de busca e as palavras-chave

        // Loop para ler e processar os registros do arquivo
        while ((c = getc(arquivo)) != EOF) { // Lê cada caractere do arquivo até encontrar o final do arquivo (EOF)
            ungetc(c, arquivo); // Coloca o caractere de volta no buffer do arquivo

            recuperarRegistro(&registro, arquivo, &maxNomeJog, &maxNacionalidade, &maxNomeClube); // Recupera o registro do arquivo

            if (registro->removido == '0') { // Verifica se o registro já foi removido
                retornoBusca = busca(registro, nroComandos, comando, palavraChave); // Realiza a busca no registro

                // Verifica o resultado da busca e remove o registro se correspondente
                if (retornoBusca == 0 || retornoBusca == 2) {
                    removeRegistro(arquivo, registro, byteOffset); // Remove o registro
                    updateList(arquivo, byteOffset, registro->tamanhoRegistro);

                    // Retornar a posição do arquivo anterior a chamada das funções
                    fseek(arquivo, byteOffset + registro->tamanhoRegistro, SEEK_SET); 
                    
                    nroRegistrosRemovidos++; // Incrementa o contador de registros removidos
                    
                    // Se a busca for pelo ID, interrompe a busca
                    if (retornoBusca == 2 || retornoBusca == 3)
                        break;
                }
            }
            byteOffset += registro->tamanhoRegistro; // Recalcula o novo Byte offset
        }
    }
    // Atualiza o cabeçalho de acordo com as remoçôes
    updateCabecalho(arquivo, nroRegistrosRemovidos);

    // Recria o arquivo de índice, de forma a descondiderar os registros removidos durante essa função
    createIndex(nomeArquivo, nomeIndice);

    desalocarRegistro(&registro); // Libera a memória alocada para o registro
    fclose(arquivo); // Fecha o arquivo após a leitura
}