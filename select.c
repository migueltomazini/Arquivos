#include "funcoes.h"

// Função auxiliar para recuperar um registro do arquivo binário
void recuperarRegistro(REGISTRO **registro, FILE *arquivo, int *maxNomeJog, int *maxNacionalidade, int *maxNomeClube) {
    REGISTRO *reg = *registro; // Declaração e inicialização de um ponteiro para o registro
    int i = 0; // Variável de controle para loops

    // Lê os campos do registro do arquivo binário
    fread(&reg->removido, sizeof(char), 1, arquivo); // Lê o campo "removido" do registro
    fread(&reg->tamanhoRegistro, sizeof(int), 1, arquivo); // Lê o tamanho do registro
    fread(&reg->prox, sizeof(long int), 1, arquivo); // Lê o ponteiro para o próximo registro
    fread(&reg->id, sizeof(int), 1, arquivo); // Lê o ID do jogador
    fread(&reg->idade, sizeof(int), 1, arquivo); // Lê a idade do jogador

    // Lê o tamanho do nome do jogador e realoca memória se necessário
    fread(&reg->tamNomeJog, sizeof(int), 1, arquivo);
    if (reg->tamNomeJog > *maxNomeJog) { // Verifica se é necessário realocar memória para o nome do jogador
        *maxNomeJog = reg->tamNomeJog + 5; // Atualiza o tamanho máximo do nome do jogador
        reg->nomeJogador = realloc(reg->nomeJogador, *maxNomeJog * sizeof(char)); // Realoca memória para o nome do jogador
        if (reg->nomeJogador == NULL) exit(1); // Encerra o programa se não for possível alocar memória
    }
    // Lê o nome do jogador caractere por caractere
    for(i = 0; i < reg->tamNomeJog; i++)
        reg->nomeJogador[i] = getc(arquivo); // Lê um caractere do arquivo e o armazena no nome do jogador
    reg->nomeJogador[i] = '\0'; // Adiciona o caractere nulo ao final do nome do jogador

    // Lê o tamanho da nacionalidade do jogador e realoca memória se necessário
    fread(&reg->tamNacionalidade, sizeof(int), 1, arquivo);
    if (reg->tamNacionalidade > *maxNacionalidade) { // Verifica se é necessário realocar memória para a nacionalidade do jogador
        *maxNacionalidade = reg->tamNacionalidade + 5; // Atualiza o tamanho máximo da nacionalidade do jogador
        reg->nacionalidade = realloc(reg->nacionalidade, *maxNacionalidade * sizeof(char)); // Realoca memória para a nacionalidade do jogador
        if (reg->nacionalidade == NULL) exit(1); // Encerra o programa se não for possível alocar memória
    }
    // Lê a nacionalidade do jogador caractere por caractere
    for(i = 0; i < reg->tamNacionalidade; i++)
        reg->nacionalidade[i] = getc(arquivo); // Lê um caractere do arquivo e o armazena na nacionalidade do jogador
    reg->nacionalidade[i] = '\0'; // Adiciona o caractere nulo ao final da nacionalidade do jogador

    // Lê o tamanho do nome do clube do jogador e realoca memória se necessário
    fread(&reg->tamNomeClube, sizeof(int), 1, arquivo);
    if (reg->tamNomeClube > *maxNomeClube) { // Verifica se é necessário realocar memória para o nome do clube do jogador
        *maxNomeClube = reg->tamNomeClube + 5; // Atualiza o tamanho máximo do nome do clube do jogador
        reg->nomeClube = realloc(reg->nomeClube, *maxNomeClube * sizeof(char)); // Realoca memória para o nome do clube do jogador
        if (reg->nomeClube == NULL) exit(1); // Encerra o programa se não for possível alocar memória
    }
    // Lê o nome do clube do jogador caractere por caractere
    for(i = 0; i < reg->tamNomeClube; i++)
        reg->nomeClube[i] = getc(arquivo); // Lê um caractere do arquivo e o armazena no nome do clube do jogador
    reg->nomeClube[i] = '\0'; // Adiciona o caractere nulo ao final do nome do clube do jogador
}
// Função auxiliar para abrir e testar o arquivo
int testarArquivo(FILE *arquivo, char *nomeArquivo) {
    char status;
    fread(&status, sizeof(char), 1, arquivo);
    // Verifica se o arquivo ou a leitura do status falharam
    if (arquivo == NULL || status =='0') {
        printf("Falha no processamento do arquivo.\n");
        return 1; // Retorna 1 se a abertura ou a leitura do status falharam
    }

    return 0; // Retorna 0 se a abertura e a leitura do status foram bem-sucedidas
}

// Função para processar os comandos de busca
void comandoBusca(int *nroComandos, char (*comando)[5][10], char (*palavraChave)[5][100]) {
    int i;
    int j;
    char c = '\0';

    scanf("%d", nroComandos); // Lê o número de comandos de busca

    getchar(); // Descarta o caractere de quebra de linha

    for (j = 0; j < *nroComandos; j++) {
        c = getchar();
        for (i = 0; c != ' '; i++) { // Lê o comando de busca caractere por caractere até encontrar um espaço
            (*comando)[j][i] = c; // Armazena o caractere lido no comando
            c = getchar(); // Lê o próximo caractere
        }
        (*comando)[j][i] = '\0'; // Adiciona o caractere nulo ao final do comando

        scan_quote_string((*palavraChave)[j]); // Lê a palavra-chave da busca
        getchar(); // Descarta o caractere de quebra de linha após a palavra-chave
    }
}
// Função de busca nos registros
// Retorna:
//   0 se o registro possui todos os atributos buscados
//   1 se o registro não possui todos os atributos buscados
//   2 se o registro possui os atributos buscados e a busca deve ser interrompida (comando = "id")
//   3 se o registro não possui os atributos buscados e a busca deve ser interrompida (comando = "id")
int busca(REGISTRO *registro, int nroComandos, char comando[5][10], char palavraChave[5][100]) {
    int temp; // Variável temporária para armazenar valores convertidos
    int retorno = 1; // Inicializa o retorno como 1 (registro não possui todos os atributos buscados)

    // Loop sobre todos os comandos de busca
    for (int i = 0; i < nroComandos; i++) {
        // Verifica se o comando atual é "id"
        if (strcmp(comando[i], "id") == 0) {
            temp = atoi(palavraChave[i]); // Converte a palavra-chave para inteiro
            // Se o valor convertido não corresponder ao ID do registro, retorna o valor atual de retorno
            if (temp != registro->id)
                return retorno;
            retorno = 3; // Define o retorno como 3 (registro possui os atributos buscados e busca deve ser interrompida)
        }
        // Verifica se o comando atual é "idade"
        if (strcmp(comando[i], "idade") == 0) {
            temp = atoi(palavraChave[i]); // Converte a palavra-chave para inteiro
            // Se o valor convertido não corresponder à idade do registro, retorna o valor atual de retorno
            if (temp != registro->idade)
                return retorno;
        }
        // Verifica se o comando atual é "nomeJogador"
        if (strcmp(comando[i], "nomeJogador") == 0) {
            // Se o nome do jogador não corresponder à palavra-chave, retorna o valor atual de retorno
            if (strcmp(palavraChave[i], registro->nomeJogador) != 0)
                return retorno;
        }
        // Verifica se o comando atual é "nacionalidade"
        if (strcmp(comando[i], "nacionalidade") == 0) {
            // Se a nacionalidade não corresponder à palavra-chave, retorna o valor atual de retorno
            if (strcmp(palavraChave[i], registro->nacionalidade) != 0)
                return retorno;
        }
        // Verifica se o comando atual é "nomeClube"
        if (strcmp(comando[i], "nomeClube") == 0) {
            // Se o nome do clube não corresponder à palavra-chave, retorna o valor atual de retorno
            if (strcmp(palavraChave[i], registro->nomeClube) != 0)
                return retorno;
        }   
    }

    // Se todas as verificações passarem, o registro possui todos os atributos buscados
    // Retorna 0 se a busca deve continuar e 2 se a busca deve ser interrompida (apenas para "id")
    return (retorno - 1); // O registro possui os atributos buscados se o retorno for 0 ou 2
}

// Função para imprimir os campos de um registro
void impressaoRegistro(REGISTRO *registro) {
    if (registro->removido == '0') { // Verifica se o registro não foi removido
        // Imprime o nome do jogador, ou "SEM DADO" se o campo estiver vazio
        if (registro->tamNomeJog != 0)
            printf("Nome do Jogador: %s\n", registro->nomeJogador);
        else
            printf("Nome do Jogador: SEM DADO\n");
        
        // Imprime a nacionalidade do jogador, ou "SEM DADO" se o campo estiver vazio
        if (registro->tamNacionalidade != 0)
            printf("Nacionalidade do Jogador: %s\n", registro->nacionalidade);
        else
            printf("Nacionalidade do Jogador: SEM DADO\n");
        
        // Imprime o clube do jogador, ou "SEM DADO" se o campo estiver vazio
        if (registro->tamNomeClube != 0)
            printf("Clube do Jogador: %s\n", registro->nomeClube);
        else
            printf("Clube do Jogador: SEM DADO\n");
        
        printf("\n"); // Imprime uma linha em branco para separar os registros
    }
}

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
    if (registro == NULL || testarArquivo(arquivo, nomeArquivo) == 1) {
        desalocarRegistro(&registro); // Libera a memória alocada para o registro
        exit (0); // Retorna se houve falha no processamento
    }

    fseek(arquivo, TAM_CABECALHO, SEEK_SET); // Posiciona o ponteiro do arquivo após o cabeçalho

    // Loop para ler e imprimir os registros do arquivo
    while ((c = getc(arquivo)) != EOF) { // Lê cada caractere do arquivo até encontrar o final do arquivo (EOF)
        ungetc(c, arquivo); // Coloca o caractere de volta no buffer do arquivo

        recuperarRegistro(&registro, arquivo, &maxNomeJog, &maxNacionalidade, &maxNomeClube); // Recupera o registro do arquivo

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
void selectFromWhere(char *nomeArquivo, int nroBuscas) {
    char comando[5][10]; // Matriz para armazenar os comandos de busca (Ex: "id", "nomeJogador")
    char palavraChave[5][100]; // Matriz para armazenar as palavras-chave de busca
    char c = '\0'; // Caractere temporário para leitura

    int nroComandos = 0; // Número de comandos de busca em cada busca
    int nroRegistros = 0; // Número de registros encontrados em uma busca
    int retornoBusca = 0; // Retorno da função de busca

    // Tamanhos máximos iniciais dos campos de texto
    int maxNomeJog = 30;
    int maxNacionalidade = 30;
    int maxNomeClube = 30;
    
    FILE *arquivo = NULL; // Ponteiro para o arquivo binário
    arquivo = fopen(nomeArquivo, "rb"); // Abre o arquivo no modo leitura binária

    REGISTRO *registro; // Declaração de um ponteiro para o registro
    alocarRegistro(&registro, maxNomeJog, maxNacionalidade, maxNomeClube); // Aloca memória para o registro

    // Verifica se o arquivo foi aberto corretamente e se o processamento falhou
    if (registro == NULL || testarArquivo(arquivo, nomeArquivo) == 1) {
        desalocarRegistro(&registro); // Libera a memória alocada para o registro
        return; // Retorna se houve falha no processamento
    }

    // Loop para realizar as buscas especificadas
    for (int j = 1; j <= nroBuscas; j++) {
        nroRegistros = 0; // Reinicia o contador de registros encontrados

        fseek(arquivo, TAM_CABECALHO, SEEK_SET); // Volta ao início do arquivo, após o cabeçalho
        comandoBusca(&nroComandos, &comando, &palavraChave); // Lê os comandos de busca e as palavras-chave

        printf("Busca %d\n\n", j); // Imprime o número da busca

        // Loop para ler e processar os registros do arquivo
        while ((c = getc(arquivo)) != EOF) { // Lê cada caractere do arquivo até encontrar o final do arquivo (EOF)
            ungetc(c, arquivo); // Coloca o caractere de volta no buffer do arquivo

            recuperarRegistro(&registro, arquivo, &maxNomeJog, &maxNacionalidade, &maxNomeClube); // Recupera o registro do arquivo

            if (registro->removido == '0') { // Verifica se o registro não foi removido
                retornoBusca = busca(registro, nroComandos, comando, palavraChave); // Realiza a busca no registro

                // Verifica o resultado da busca e imprime o registro se correspondente
                if (retornoBusca == 0 || retornoBusca == 2) {
                    impressaoRegistro(registro); // Imprime os campos do registro
                    nroRegistros++; // Incrementa o contador de registros encontrados

                    // Se a busca for pelo ID, interrompe a busca
                    if (retornoBusca == 2 || retornoBusca == 3)
                        break;
                }
            }
        }

        // Verifica se nenhum registro foi encontrado durante a busca atual
        if (nroRegistros == 0)
            printf("Registro inexistente.\n\n"); // Imprime uma mensagem indicando que nenhum registro foi encontrado
    }

    desalocarRegistro(&registro); // Libera a memória alocada para o registro
    fclose(arquivo); // Fecha o arquivo após a leitura
}
