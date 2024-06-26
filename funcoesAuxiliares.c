#include "funcoesAuxiliares.h" // Inclui o cabeçalho com as funções auxiliares
#include "funcoes.h" // Inclui o cabeçalho com as funções principais
#include <stdlib.h> // Inclui a biblioteca padrão de funções de alocação de memória
#include <stdio.h> // Inclui a biblioteca padrão de funções de entrada e saída
#include <string.h> // Inclui a biblioteca padrão de manipulação de strings
#include <stdbool.h> // Inclui a biblioteca padrão para trabalhar com o tipo booleano

// Função para reinserir o cabeçalho no arquivo
void reinserir_cabecalho(FILE *arquivo, CABECALHO *cabecalho, char status) {
    cabecalho->status = status; // Atualiza o status do cabeçalho
    fseek(arquivo, 0, SEEK_SET); // Posiciona o ponteiro do arquivo no início
    fwrite(&cabecalho->status, sizeof(char), 1, arquivo); // Escreve o status no arquivo
    fwrite(&cabecalho->topo, sizeof(long int), 1, arquivo); // Escreve o topo no arquivo
    fwrite(&cabecalho->prosByteOffset, sizeof(long int), 1, arquivo); // Escreve o próximo byte offset no arquivo
    fwrite(&cabecalho->nroRegArq, sizeof(int), 1, arquivo); // Escreve o número de registros no arquivo
    fwrite(&cabecalho->nroRegRem, sizeof(int), 1, arquivo); // Escreve o número de registros removidos no arquivo
}

void reinserir_cabecalho_ind(FILE *arquivo, CABECALHO_IND *cabecalho, char status) {
    cabecalho->status = status; // Atualiza o status do cabeçalho
    fseek(arquivo, 0, SEEK_SET); // Posiciona o ponteiro do arquivo no início
    fwrite(&cabecalho->status, sizeof(char), 1, arquivo); // Escreve o status no arquivo
}

void ler_cabecalho_ind(FILE *arquivo, CABECALHO_IND *cabecalho) {
    fseek(arquivo, 0, SEEK_SET); // Posiciona o ponteiro do arquivo no início
    fread(&cabecalho->status, sizeof(char), 1, arquivo); // Lê o status do arquivo
}

// Função para ler o cabeçalho do arquivo
void ler_cabecalho(FILE *arquivo, CABECALHO *cabecalho) {
    fseek(arquivo, 0, SEEK_SET); // Posiciona o ponteiro do arquivo no início
    fread(&cabecalho->status, sizeof(char), 1, arquivo); // Lê o status do arquivo
    fread(&cabecalho->topo, sizeof(long int), 1, arquivo); // Lê o topo do arquivo
    fread(&cabecalho->prosByteOffset, sizeof(long int), 1, arquivo); // Lê o próximo byte offset do arquivo
    fread(&cabecalho->nroRegArq, sizeof(int), 1, arquivo); // Lê o número de registros do arquivo
    fread(&cabecalho->nroRegRem, sizeof(int), 1, arquivo); // Lê o número de registros removidos do arquivo
}

// Função para ler um registro do arquivo
void ler_registro(REGISTRO *registro, FILE *arquivo) {
    fread(&registro->removido, sizeof(char), 1, arquivo); // Lê o campo removido do registro
    fread(&registro->tamanhoRegistro, sizeof(int), 1, arquivo); // Lê o tamanho do registro
    fread(&registro->prox, sizeof(long int), 1, arquivo); // Lê o próximo registro
    fread(&registro->id, sizeof(int), 1, arquivo); // Lê o ID do jogador
    fread(&registro->idade, sizeof(int), 1, arquivo); // Lê a idade do jogador
    fread(&registro->tamNomeJog, sizeof(int), 1, arquivo); // Lê o tamanho do nome do jogador
    registro->nomeJogador = (char *)malloc((registro->tamNomeJog + 1) * sizeof(char)); // Aloca memória para o nome do jogador
    fread(registro->nomeJogador, sizeof(char), registro->tamNomeJog, arquivo); // Lê o nome do jogador
    registro->nomeJogador[registro->tamNomeJog] = '\0'; // Adiciona o caractere nulo ao final do nome do jogador
    fread(&registro->tamNacionalidade, sizeof(int), 1, arquivo); // Lê o tamanho da nacionalidade do jogador
    registro->nacionalidade = (char *)malloc((registro->tamNacionalidade + 1) * sizeof(char)); // Aloca memória para a nacionalidade do jogador
    fread(registro->nacionalidade, sizeof(char), registro->tamNacionalidade, arquivo); // Lê a nacionalidade do jogador
    registro->nacionalidade[registro->tamNacionalidade] = '\0'; // Adiciona o caractere nulo ao final da nacionalidade do jogador
    fread(&registro->tamNomeClube, sizeof(int), 1, arquivo); // Lê o tamanho do nome do clube do jogador
    registro->nomeClube = (char *)malloc((registro->tamNomeClube + 1) * sizeof(char)); // Aloca memória para o nome do clube do jogador
    fread(registro->nomeClube, sizeof(char), registro->tamNomeClube, arquivo); // Lê o nome do clube do jogador
    registro->nomeClube[registro->tamNomeClube] = '\0'; // Adiciona o caractere nulo ao final do nome do clube do jogador
}

// Função para preencher espaços vazios no arquivo
void preeche_vazio(FILE *arquivo, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        fwrite("$", sizeof(char), 1, arquivo); // Escreve o caractere '$' no arquivo
    }
}

// Função para ler lixo do arquivo
int ler_lixo(FILE *file) {
    char lixo;
    int tam = -1;
    do {
        lixo = getc(file); // Lê um caractere do arquivo
        tam++;
    } while (lixo == '$'); // Continua lendo enquanto o caractere for '$'
    ungetc(lixo, file);

    return tam;
}

// Função para atualizar a lista encadeada de registros removidos
void updateList(FILE *arquivo, long int novo, int tamanhoRegistroNovo) {
    long int atual = 0; // Armazena o byte offset de um item da lista
    long int prox = 0; // Armazena o byte offset do próximo item da lista
    int tamanhoRegistroAtual = -1; // Armazena o tamanho do registro atual

    fseek(arquivo, 1, SEEK_SET); // Posiciona o ponteiro no segundo byte (após o status)
    fread(&atual, sizeof(long int), 1, arquivo); // Lê o valor armazenado no topo

    if (atual == -1) { // Caso o topo esteja vazio
        fseek(arquivo, 1, SEEK_SET); // Posiciona o ponteiro no segundo byte
        fwrite(&novo, sizeof(long int), 1, arquivo); // Escreve o novo valor no topo
        return;
    }

    fseek(arquivo, atual + 1, SEEK_SET); // Posiciona o ponteiro no registro atual
    fread(&tamanhoRegistroAtual, sizeof(int), 1, arquivo); // Lê o tamanho do registro atual

    if (tamanhoRegistroNovo >= tamanhoRegistroAtual) { // Se o novo registro for menor que o atual
        fread(&prox, sizeof(long int), 1, arquivo); // Lê o próximo registro

        fseek(arquivo, 1, SEEK_SET); // Posiciona o ponteiro no segundo byte
        fwrite(&novo, sizeof(long int), 1, arquivo); // Escreve o novo valor no topo

        fseek(arquivo, novo + 5, SEEK_SET); // Posiciona o ponteiro no novo registro
        fwrite(&atual, sizeof(long int), 1, arquivo); // Escreve o valor do próximo registro

        return;
    }

    while (tamanhoRegistroNovo >= tamanhoRegistroAtual) { // Percorre a lista até encontrar o local para inserção
        fseek(arquivo, atual + 1, SEEK_SET); // Posiciona o ponteiro no registro atual
        fread(&tamanhoRegistroAtual, sizeof(int), 1, arquivo); // Lê o tamanho do registro atual

        if (atual == -1) // Caso chegue ao fim da lista
            break;

        if (tamanhoRegistroNovo >= tamanhoRegistroAtual)
            fread(&atual, sizeof(long int), 1, arquivo); // Lê o próximo registro
    }
    fread(&prox, sizeof(long int), 1, arquivo); // Lê o próximo registro

    fseek(arquivo, novo + 5, SEEK_SET); // Posiciona o ponteiro no novo registro
    fwrite(&prox, sizeof(long int), 1, arquivo); // Escreve o valor do próximo registro

    fseek(arquivo, atual + 5, SEEK_SET); // Posiciona o ponteiro no registro atual
    fwrite(&novo, sizeof(long int), 1, arquivo); // Escreve o novo valor no próximo registro
}

// Função para atualizar o cabeçalho do arquivo
void updateCabecalho(FILE *arquivo, int nroRemocoes) {
    int nroReg; // Número de registros
    int nroRegRem; // Número de registros removidos

    fseek(arquivo, 17, SEEK_SET); // Posiciona o ponteiro no campo do número de registros
    fread(&nroReg, sizeof(int), 1, arquivo); // Lê o número de registros
    fread(&nroRegRem, sizeof(int), 1, arquivo); // Lê o número de registros removidos

    nroReg -= nroRemocoes; // Atualiza o número de registros
    nroRegRem += nroRemocoes; // Atualiza o número de registros removidos

    fseek(arquivo, 17, SEEK_SET); // Posiciona o ponteiro no campo do número de registros
    fwrite(&nroReg, sizeof(int), 1, arquivo); // Escreve o número de registros atualizado
    fwrite(&nroRegRem, sizeof(int), 1, arquivo); // Escreve o número de registros removidos atualizado
}

// Função para recuperar um registro do arquivo binário
void recuperarRegistro(REGISTRO **registro, FILE *arquivo, int byteOffset ,int *maxNomeJog, int *maxNacionalidade, int *maxNomeClube) {
    REGISTRO *reg = *registro; // Declaração e inicialização de um ponteiro para o registro
    int i = 0; // Variável de controle para loops

    if (byteOffset != -1)
        fseek(arquivo, byteOffset, SEEK_SET); // Posiciona o ponteiro no byte offset especificado

    fread(&reg->removido, sizeof(char), 1, arquivo); // Lê o campo "removido" do registro
    fread(&reg->tamanhoRegistro, sizeof(int), 1, arquivo); // Lê o tamanho do registro
    fread(&reg->prox, sizeof(long int), 1, arquivo); // Lê o ponteiro para o próximo registro
    fread(&reg->id, sizeof(int), 1, arquivo); // Lê o ID do jogador
    fread(&reg->idade, sizeof(int), 1, arquivo); // Lê a idade do jogador

    fread(&reg->tamNomeJog, sizeof(int), 1, arquivo); // Lê o tamanho do nome do jogador
    if (reg->tamNomeJog > *maxNomeJog) { // Verifica se é necessário realocar memória para o nome do jogador
        *maxNomeJog = reg->tamNomeJog + 5; // Atualiza o tamanho máximo do nome do jogador
        reg->nomeJogador = realloc(reg->nomeJogador, *maxNomeJog * sizeof(char)); // Realoca memória para o nome do jogador
        if (reg->nomeJogador == NULL) exit(1); // Encerra o programa se não for possível alocar memória
    }

    for(i = 0; i < reg->tamNomeJog; i++) // Lê o nome do jogador caractere por caractere
        reg->nomeJogador[i] = getc(arquivo); // Lê um caractere do arquivo e o armazena no nome do jogador

    reg->nomeJogador[i] = '\0'; // Adiciona o caractere nulo ao final do nome do jogador

    fread(&reg->tamNacionalidade, sizeof(int), 1, arquivo); // Lê o tamanho da nacionalidade do jogador
    if (reg->tamNacionalidade > *maxNacionalidade) { // Verifica se é necessário realocar memória para a nacionalidade do jogador
        *maxNacionalidade = reg->tamNacionalidade + 5; // Atualiza o tamanho máximo da nacionalidade do jogador
        reg->nacionalidade = realloc(reg->nacionalidade, *maxNacionalidade * sizeof(char)); // Realoca memória para a nacionalidade do jogador
        if (reg->nacionalidade == NULL) exit(1); // Encerra o programa se não for possível alocar memória
    }

    for(i = 0; i < reg->tamNacionalidade; i++) // Lê a nacionalidade do jogador caractere por caractere
        reg->nacionalidade[i] = getc(arquivo); // Lê um caractere do arquivo e o armazena na nacionalidade do jogador

    reg->nacionalidade[i] = '\0'; // Adiciona o caractere nulo ao final da nacionalidade do jogador

    fread(&reg->tamNomeClube, sizeof(int), 1, arquivo); // Lê o tamanho do nome do clube do jogador
    if (reg->tamNomeClube > *maxNomeClube) { // Verifica se é necessário realocar memória para o nome do clube do jogador
        *maxNomeClube = reg->tamNomeClube + 5; // Atualiza o tamanho máximo do nome do clube do jogador
        reg->nomeClube = realloc(reg->nomeClube, *maxNomeClube * sizeof(char)); // Realoca memória para o nome do clube do jogador
        if (reg->nomeClube == NULL) exit(1); // Encerra o programa se não for possível alocar memória
    }

    for(i = 0; i < reg->tamNomeClube; i++) // Lê o nome do clube do jogador caractere por caractere
        reg->nomeClube[i] = getc(arquivo); // Lê um caractere do arquivo e o armazena no nome do clube do jogador

    reg->nomeClube[i] = '\0'; // Adiciona o caractere nulo ao final do nome do clube do jogador
}

// Função auxiliar para abrir e testar o arquivo
int testarArquivo(FILE *arquivo) {
    char status; // Variável para armazenar o status do arquivo
    fread(&status, sizeof(char), 1, arquivo); // Lê o status do arquivo
    if (arquivo == NULL || status =='0') { // Verifica se o arquivo é nulo ou se o status é inválido
        printf("Falha no processamento do arquivo.\n"); // Imprime mensagem de erro
        return 1; // Retorna 1 para indicar falha
    }
    fseek(arquivo, 0, SEEK_SET);
    return 0; // Retorna 0 para indicar sucesso
}

// Função para processar os comandos de busca
int comandoBusca(int *nroComandos, char (*comando)[5][20], char (*palavraChave)[5][100]) {
    int i; // Variável de controle para loops
    int j; // Variável de controle para loops
    char c = '\0'; // Variável para armazenar caracteres temporariamente

    scanf("%d", nroComandos); // Lê o número de comandos de busca
    getchar(); // Descarta o caractere de quebra de linha

    for (j = 0; j < *nroComandos; j++) {
        c = getchar(); // Lê o primeiro caractere do comando
        for (i = 0; c != ' '; i++) { // Lê o comando de busca caractere por caractere até encontrar um espaço
            (*comando)[j][i] = c; // Armazena o caractere lido no comando
            c = getchar(); // Lê o próximo caractere
        }
        (*comando)[j][i] = '\0'; // Adiciona o caractere nulo ao final do comando

        scan_quote_string((*palavraChave)[j]); // Lê a palavra-chave da busca
        getchar(); // Descarta o caractere de quebra de linha após a palavra-chave
    }

    for (j = 0; j < *nroComandos; j++) {
        if ((strcmp((*comando)[j], "id") == 0)) // Verifica se o comando é "id"
            return j; // Retorna o índice do comando "id"
    }

    return -1; // Retorna -1 se nenhum comando "id" for encontrado
}

// Função de busca nos registros
int busca(REGISTRO *registro, REGISTRO_IND *vetorInd, int nroComandos, char comando[5][20], char palavraChave[5][100]) {
    int temp; // Variável temporária para armazenar valores convertidos
    int retorno = 1; // Inicializa o retorno como 1 (registro não possui todos os atributos buscados)

    for (int i = 0; i < nroComandos; i++) { // Loop sobre todos os comandos de busca
        if (strcmp(comando[i], "idade") == 0) { // Verifica se o comando atual é "idade"
            temp = atoi(palavraChave[i]); // Converte a palavra-chave para inteiro
            if (temp != registro->idade) // Se o valor convertido não corresponder à idade do registro
                return retorno; // Retorna o valor atual de retorno
        }
        if (strcmp(comando[i], "nomeJogador") == 0) { // Verifica se o comando atual é "nomeJogador"
            if (strcmp(palavraChave[i], registro->nomeJogador) != 0) // Se o nome do jogador não corresponder à palavra-chave
                return retorno; // Retorna o valor atual de retorno
        }
        if (strcmp(comando[i], "nacionalidade") == 0) { // Verifica se o comando atual é "nacionalidade"
            if (strcmp(palavraChave[i], registro->nacionalidade) != 0) // Se a nacionalidade não corresponder à palavra-chave
                return retorno; // Retorna o valor atual de retorno
        }
        if (strcmp(comando[i], "nomeClube") == 0) { // Verifica se o comando atual é "nomeClube"
            if (strcmp(palavraChave[i], registro->nomeClube) != 0) // Se o nome do clube não corresponder à palavra-chave
                return retorno; // Retorna o valor atual de retorno
        }
    }
    return (retorno - 1); // O registro possui os atributos buscados se o retorno for 0 ou 2
}

// Função para buscar um registro pelo ID
long int buscaId(REGISTRO *registro, REGISTRO_IND *vetorInd, int nroRegistros, int nroComandos, char comando[5][20], char palavraChave[5][100]) {
    int left = 0; // Índice do início da busca
    int right = nroRegistros - 1; // Índice do final da busca
    int middle = 0; // Índice do meio da busca

    int i = 0; // Variável de controle para loops
    int id = 0; // Variável para armazenar o ID

    while (i < nroComandos && strcmp(comando[i], "id") != 0) // Procura pelo comando "id"
        i++;

    id = atoi(palavraChave[i]); // Converte a palavra-chave para inteiro

    while(left <= right) { // Loop de busca binária
        middle = left + (right - left) / 2; // Calcula o índice do meio

        if (vetorInd[middle].id == id) // Se o ID no meio corresponde ao ID buscado
            return vetorInd[middle].byteOffset; // Retorna o byte offset do registro
        if (vetorInd[middle].id < id) // Se o ID no meio é menor que o ID buscado
            left = middle + 1; // Move o índice do início para a direita
        else
            right = middle - 1; // Move o índice do final para a esquerda
    }

    return -1; // Retorna -1 se o ID não for encontrado
}

// Função para imprimir os campos de um registro
void impressaoRegistro(REGISTRO *registro) {
    if (registro->removido == '0') { // Verifica se o registro não foi removido
        if (registro->tamNomeJog != 0) // Verifica se o campo nome do jogador não está vazio
            printf("Nome do Jogador: %s\n", registro->nomeJogador); // Imprime o nome do jogador
        else
            printf("Nome do Jogador: SEM DADO\n"); // Imprime "SEM DADO" se o campo estiver vazio

        if (registro->tamNacionalidade != 0) // Verifica se o campo nacionalidade não está vazio
            printf("Nacionalidade do Jogador: %s\n", registro->nacionalidade); // Imprime a nacionalidade do jogador
        else
            printf("Nacionalidade do Jogador: SEM DADO\n"); // Imprime "SEM DADO" se o campo estiver vazio

        if (registro->tamNomeClube != 0) // Verifica se o campo nome do clube não está vazio
            printf("Clube do Jogador: %s\n", registro->nomeClube); // Imprime o nome do clube do jogador
        else
            printf("Clube do Jogador: SEM DADO\n"); // Imprime "SEM DADO" se o campo estiver vazio

        printf("\n"); // Imprime uma linha em branco para separar os registros
    }
}

// Função para recuperar o índice do arquivo
REGISTRO_IND *recoverIndex(FILE *arquivo, FILE *indice) {
    int nroRegistros; // Número de registros
    REGISTRO_IND *registro; // Ponteiro para os registros

    if ((getc(indice)) == '0') { // Verifica se o índice está válido
        printf("Falha no processamento do indice.\n"); // Imprime mensagem de erro
        return NULL; // Retorna NULL para indicar falha
    }

    fseek(arquivo, 17, SEEK_SET); // Posiciona o ponteiro no campo do número de registros
    fread(&nroRegistros, sizeof(int), 1, arquivo); // Lê o número de registros

    registro = malloc(sizeof(REGISTRO_IND) * (nroRegistros + 1)); // Aloca memória para os registros
    if (registro == NULL) { // Verifica se a alocação de memória foi bem-sucedida
        printf("Erro ao alocar memória.\n"); // Imprime mensagem de erro
        return NULL; // Retorna NULL para indicar falha
    }

    for (int i = 0; i < nroRegistros; i++) { // Passa o conteúdo do arquivo índice para a memória primária
        fread(&registro[i].id, sizeof(int), 1, indice); // Lê o ID do registro
        fread(&registro[i].byteOffset, sizeof(long int), 1, indice); // Lê o byte offset do registro
    }

    return registro; // Retorna o ponteiro para os registros
}

// Função para criar um novo registro
REGISTRO *criar_registro(int id, int idade, const char *nomeJogador, const char *nacionalidade, const char *clube) {
    REGISTRO *novoRegistro = (REGISTRO *)malloc(sizeof(REGISTRO)); // Aloca memória para o novo registro
    if (!novoRegistro) { // Verifica se a alocação de memória foi bem-sucedida
        printf("Falha no processamento do arquivo.\n"); // Imprime mensagem de erro
        exit(1); // Encerra o programa com erro
    }

    novoRegistro->id = id; // Atribui o ID do jogador ao novo registro
    novoRegistro->idade = idade; // Atribui a idade do jogador ao novo registro
    novoRegistro->tamNacionalidade = strlen(nacionalidade); // Calcula e atribui o tamanho da nacionalidade
    novoRegistro->tamNomeJog = strlen(nomeJogador); // Calcula e atribui o tamanho do nome do jogador
    novoRegistro->tamNomeClube = strlen(clube); // Calcula e atribui o tamanho do nome do clube

    novoRegistro->nomeJogador = (char *)malloc((novoRegistro->tamNomeJog + 1) * sizeof(char)); // Aloca memória para o nome do jogador
    novoRegistro->nacionalidade = (char *)malloc((novoRegistro->tamNacionalidade + 1) * sizeof(char)); // Aloca memória para a nacionalidade do jogador
    novoRegistro->nomeClube = (char *)malloc((novoRegistro->tamNomeClube + 1) * sizeof(char)); // Aloca memória para o nome do clube do jogador

    if (!novoRegistro->nomeJogador || !novoRegistro->nacionalidade || !novoRegistro->nomeClube) { // Verifica se a alocação de memória foi bem-sucedida
        printf("Falha no processamento do arquivo.\n"); // Imprime mensagem de erro
        exit(1); // Encerra o programa com erro
    }

    strcpy(novoRegistro->nomeJogador, nomeJogador); // Copia o nome do jogador para o novo registro
    strcpy(novoRegistro->nacionalidade, nacionalidade); // Copia a nacionalidade do jogador para o novo registro
    strcpy(novoRegistro->nomeClube, clube); // Copia o nome do clube do jogador para o novo registro

    novoRegistro->tamanhoRegistro = 33 + novoRegistro->tamNomeJog + novoRegistro->tamNomeClube + novoRegistro->tamNacionalidade; // Calcula o tamanho total do registro
    novoRegistro->removido = '0'; // Define o campo "removido" como '0' (não removido)
    novoRegistro->prox = -1; // Define o ponteiro para o próximo registro como -1 (nulo)

    return novoRegistro; // Retorna o novo registro
}

// Função para inserir o registro no arquivo binário
void inserirRegistro(REGISTRO *registro, FILE *arquivo) {
    if (registro == NULL || arquivo == NULL) // Verifica se o registro ou o arquivo são nulos
        return; // Retorna se algum dos parâmetros for nulo

    putc(registro->removido, arquivo); // Escreve o campo "removido" no arquivo
    fwrite(&registro->tamanhoRegistro, sizeof(int), 1, arquivo); // Escreve o tamanho do registro no arquivo
    fwrite(&registro->prox, sizeof(long int), 1, arquivo); // Escreve o ponteiro para o próximo registro no arquivo
    fwrite(&registro->id, sizeof(int), 1, arquivo); // Escreve o ID do jogador no arquivo
    fwrite(&registro->idade, sizeof(int), 1, arquivo); // Escreve a idade do jogador no arquivo
    fwrite(&registro->tamNomeJog, sizeof(int), 1, arquivo); // Escreve o tamanho do nome do jogador no arquivo
    fputs(registro->nomeJogador, arquivo); // Escreve o nome do jogador no arquivo
    fwrite(&registro->tamNacionalidade, sizeof(int), 1, arquivo); // Escreve o tamanho da nacionalidade no arquivo
    fputs(registro->nacionalidade, arquivo); // Escreve a nacionalidade no arquivo
    fwrite(&registro->tamNomeClube, sizeof(int), 1, arquivo); // Escreve o tamanho do nome do clube no arquivo
    fputs(registro->nomeClube, arquivo); // Escreve o nome do clube no arquivo
}

// Função auxiliar para alocar memória dinamicamente para o registro
void alocarRegistro(REGISTRO **registro, int maxNomeJog, int maxNacionalidade, int maxNomeClube) {
    *registro = malloc(sizeof(REGISTRO)); // Aloca memória para a estrutura REGISTRO

    if (*registro == NULL) // Verifica se a alocação de memória foi bem-sucedida
        return; // Retorna se não for possível alocar memória

    (*registro)->nomeJogador = malloc(maxNomeJog * sizeof(char)); // Aloca memória para o nome do jogador
    (*registro)->nomeClube = malloc(maxNacionalidade * sizeof(char)); // Aloca memória para o nome do clube do jogador
    (*registro)->nacionalidade = malloc(maxNomeClube * sizeof(char)); // Aloca memória para a nacionalidade do jogador

    if ((*registro)->nomeJogador == NULL || (*registro)->nomeClube == NULL || (*registro)->nacionalidade == NULL) // Verifica se a alocação de memória foi bem-sucedida
        return; // Retorna se não for possível alocar memória
}
void desalocarRegistro(REGISTRO **registro) {
    // Libera a memória alocada para os campos de strings dentro da estrutura REGISTRO
    free((*registro)->nomeJogador);
    free((*registro)->nacionalidade);
    free((*registro)->nomeClube);
    // Libera a memória alocada para a estrutura REGISTRO
    free((*registro));
    // Define o ponteiro original como NULL
    *registro = NULL;
}