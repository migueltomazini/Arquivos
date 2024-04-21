#include "funcoes.h"

// Função responsável pela montagem e inserção do cabeçalho
void cabecalho(FILE *arquivo, int nroRegArq) {
    // Aloca memória para a estrutura CABECALHO
    CABECALHO *cabecalho = malloc(sizeof(CABECALHO));

    // Verifica se a alocação de memória foi bem-sucedida
    if (cabecalho == NULL)
        return; // Retorna se não for possível alocar memória
    
    // Define os valores dos campos da estrutura CABECALHO
    cabecalho->status = '1'; // Define o status como '1', indicando que o arquivo está OK
    cabecalho->topo = -1; // Define o topo da pilha de registros removidos como -1, indicando que não há registros removidos
    cabecalho->prosByteOffset = 0; // Define o próximo byte offset como 0
    cabecalho->nroRegArq = nroRegArq; // Define o número total de registros no arquivo
    cabecalho->nroRegRem = 0; // Define o número de registros removidos como 0, pois nenhum registro foi removido ainda

    // Escreve os dados do cabeçalho no arquivo binário
    putc(cabecalho->status, arquivo); // Escreve o status no arquivo
    fwrite(&cabecalho->topo, sizeof(long int), 1, arquivo); // Escreve o topo da pilha de registros removidos no arquivo
    fwrite(&cabecalho->prosByteOffset, sizeof(long int), 1, arquivo); // Escreve o próximo byte offset no arquivo
    fwrite(&cabecalho->nroRegArq, sizeof(int), 1, arquivo); // Escreve o número total de registros no arquivo
    fwrite(&cabecalho->nroRegRem, sizeof(int), 1, arquivo); // Escreve o número de registros removidos no arquivo

    // Libera a memória alocada para a estrutura CABECALHO
    free(cabecalho);
}

// Função auxiliar responsável por alocar memória dinamicamente para o registro
void alocarRegistro(REGISTRO **registro, int maxNomeJog, int maxNacionalidade, int maxNomeClube) {
    // Aloca memória para a estrutura REGISTRO
    *registro = malloc(sizeof(REGISTRO));

    // Verifica se a alocação de memória foi bem-sucedida
    if (*registro == NULL) 
        return; // Retorna se não for possível alocar memória

    // Aloca memória para os campos de strings dentro da estrutura REGISTRO
    (*registro)->nomeJogador = malloc(maxNomeJog * sizeof(char));
    (*registro)->nomeClube = malloc(maxNacionalidade * sizeof(char));
    (*registro)->nacionalidade = malloc(maxNomeClube * sizeof(char));

    // Verifica se a alocação de memória foi bem-sucedida para os campos de strings
    if ((*registro)->nomeJogador == NULL || (*registro)->nomeClube == NULL || 
        (*registro)->nacionalidade == NULL)
        return; // Retorna se não for possível alocar memória
}

// Função auxiliar responsável por desalocar a memória do registro
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

// Função responsável pela montagem e inserção dos registros
void Registro(FILE *dados, FILE *arquivo) {       
    char c = '\0';                                      // Variável para armazenar o caractere lido do arquivo CSV
    REGISTRO *registro;                                 // Ponteiro para a estrutura REGISTRO

    int i = 0;                                          // Variável de iteração
    int regAdicionados = 0;                             // Contador de registros adicionados ao arquivo binário
    long int proxByteOffset = TAM_CABECALHO;            // Próximo byte offset a ser escrito no cabeçalho do arquivo binário

    int maxNomeJog = 30;                                // Tamanho máximo inicial para o nome do jogador
    int maxNacionalidade = 30;                          // Tamanho máximo inicial para a nacionalidade
    int maxNomeClube = 30;                              // Tamanho máximo inicial para o nome do clube

    char charId[8];         // Utilizado para ler o ID em dados
    char charIdade[4];      // Utilizado para ler a idade em dados

    // Aloca memória para a estrutura REGISTRO
    alocarRegistro(&registro, maxNomeJog, maxNacionalidade, maxNomeClube);

    // Loop para coletar os dados enquanto não chegar ao final do arquivo CSV (origem)
    while ((c = getc(dados)) != EOF) { 
        ungetc(c, dados); // Coloca o caractere lido de volta no buffer do arquivo

        registro->prox = -1; // Define o ponteiro para o próximo registro como -1

        // Lê o ID do jogador
        for (i = 0; (c = getc(dados)) != ','; i++)
            charId[i] = c;
        charId[i++] = '\0';
        registro->id = atoi(charId); // Converte o ID para inteiro

        // Lê a idade do jogador
        for (i = 0; (c = getc(dados)) != ','; i++)
            charIdade[i] = c;
        charIdade[i++] = '\0';
        if (i == 1)    
            registro->idade = -1;   // Se não houver idade registrada, marca como -1
        else
            registro->idade = atoi(charIdade); // Converte a idade para inteiro

        registro->removido = '0'; // Define o registro como não removido

        // Loop para ler o nome do jogador
        for (i = 0; (c = getc(dados)) != ','; i++) {    
            // Verifica se é necessário realocar memória para o nome do jogador
            if (i == maxNomeJog - 1) {
                maxNomeJog += 5; // Aumenta o tamanho máximo
                registro->nomeJogador = realloc(registro->nomeJogador, maxNomeJog * sizeof(char)); // Realoca memória
                if (registro->nomeJogador == NULL) exit(1); // Encerra o programa se não for possível alocar memória
            }
            registro->nomeJogador[i] = c; // Armazena o caractere no nome do jogador
        }
        registro->nomeJogador[i] = '\0'; // Adiciona o terminador de string
        registro->tamNomeJog = i; // Atualiza o tamanho do nome do jogador
        
        // Loop para ler a nacionalidade do jogador
        for (i = 0; (c = getc(dados)) != ','; i++) {    
            // Verifica se é necessário realocar memória para a nacionalidade
            if (i == maxNacionalidade - 1) {
                maxNacionalidade += 5; // Aumenta o tamanho máximo
                registro->nacionalidade = realloc(registro->nacionalidade, maxNacionalidade * sizeof(char)); // Realoca memória
                if (registro->nacionalidade == NULL) exit(1); // Encerra o programa se não for possível alocar memória
            }
            registro->nacionalidade[i] = c; // Armazena o caractere na nacionalidade
        }
        registro->nacionalidade[i] = '\0'; // Adiciona o terminador de string
        registro->tamNacionalidade = i; // Atualiza o tamanho da nacionalidade
        
        // Loop para ler o nome do clube
        for (i = 0; (c = getc(dados)) != '\n'; i++) {   
            // Verifica se é necessário realocar memória para o nome do clube
            if (i == maxNomeClube - 1) {
                maxNomeClube += 5; // Aumenta o tamanho máximo
                registro->nomeClube = realloc(registro->nomeClube, maxNomeClube * sizeof(char)); // Realoca memória
                if (registro->nomeClube == NULL) exit(1); // Encerra o programa se não for possível alocar memória
            }
            registro->nomeClube[i] = c; // Armazena o caractere no nome do clube
        }
        registro->nomeClube[i] = '\0'; // Adiciona o terminador de string
        registro->tamNomeClube = i; // Atualiza o tamanho do nome do clube
        
        // Calcula o tamanho total do registro
        registro->tamanhoRegistro = (sizeof(char) + 6 * sizeof(int) + sizeof(long int) +
            registro->tamNomeJog + registro->tamNomeClube + registro->tamNacionalidade);

        // Insere o registro no arquivo binário
        inserirRegistro(registro, arquivo);
        regAdicionados++; // Incrementa o contador de registros adicionados
        proxByteOffset += registro->tamanhoRegistro; // Atualiza o próximo byte offset
    }

    // Atualiza o cabeçalho do arquivo binário com as informações finais
    fseek(arquivo, 9, SEEK_SET); // Posiciona o ponteiro do arquivo no início do campo do próximo byte offset
    fwrite(&proxByteOffset, sizeof(long int), 1, arquivo); // Escreve o próximo byte offset no arquivo
    fseek(arquivo, 17, SEEK_SET); // Posiciona o ponteiro do arquivo no início do campo do número de registros adicionados
    fwrite(&regAdicionados, sizeof(int), 1, arquivo); // Escreve o número de registros adicionados no arquivo

    // Libera a memória alocada para a estrutura REGISTRO
    desalocarRegistro(&registro);
}

// Função responsável por inserir o registro no arquivo binário
void inserirRegistro(REGISTRO *registro, FILE *arquivo) {
    // Verifica se o registro ou o arquivo são nulos
    if (registro == NULL || arquivo == NULL)
        return; // Retorna se algum dos parâmetros for nulo

    // Escreve os campos do registro no arquivo binário
    putc(registro->removido, arquivo); // Escreve o campo removido no arquivo
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

// Função responsável por passar os dados do arquivo .csv para o binário
void createTable(char *nomeArquivoCsv, char *nomeArquivoBin) {
    FILE *arquivo;      // Arquivo binário para qual serão passados os dados
    FILE *dados;        // Arquivo .csv contendo os dados

    // Abertura e testagem dos arquivos 
    if ((dados = fopen(nomeArquivoCsv, "r")) == NULL ||
        (arquivo = fopen(nomeArquivoBin, "wb")) == NULL) { // Abre o arquivo binário no modo escrita
        printf("Erro na abertura do arquivo.\n");
        return; // Retorna se não for possível abrir os arquivos
    }

    // Pula a primeira linha do arquivo .csv (cabeçalho)
    while ((fgetc(dados)) != '\n') {}
    
    // Escreve o cabeçalho no arquivo binário
    cabecalho(arquivo, 0);
    // Cria e insere os registros no arquivo binário
    Registro(dados, arquivo);

    // Fecha os arquivos
    fclose(arquivo);
    fclose(dados);
}
