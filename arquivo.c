#include "arquivo.h"

// Função responsável pela montagem e inserção do cabeçalho
void cabecalho(FILE *arquivo) {
    CABECALHO *cabecalho = malloc(sizeof(CABECALHO));
    int tamCabecalho = sizeof(char) + 2 * sizeof(int) + 2 * sizeof(long int);

    if (cabecalho == NULL)
        return;
    
    cabecalho->status = '1';
    cabecalho->topo = -1;
    cabecalho->prosByteOffset = 0;
    cabecalho->nroRegArq = 0;
    cabecalho->nroRegRem = 0;

    putc(cabecalho->status, arquivo);
    fwrite(&cabecalho->topo, sizeof(long int), 1, arquivo);
    fwrite(&cabecalho->prosByteOffset, sizeof(long int), 1, arquivo);
    fwrite(&cabecalho->nroRegArq, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->nroRegRem, sizeof(int), 1, arquivo);

    //fwrite(cabecalho, tamCabecalho, 1, arquivo);

    free(cabecalho);
}

// Função auxiliar responsável por alocar memória dinamicamente para o registro
void alocarRegistro(REGISTRO **registro) {
    *registro = malloc(sizeof(REGISTRO));
    if (*registro == NULL) 
        exit(1);

    (*registro)->nomeJogador = malloc(30 * sizeof(char));
    (*registro)->nomeClube = malloc(30 * sizeof(char));
    (*registro)->nacionalidade = malloc(30 * sizeof(char));
    if ((*registro)->nomeJogador == NULL || (*registro)->nomeClube == NULL|| 
        (*registro)->nacionalidade == NULL)
        exit(1);
}

// Função auxiliar responsável por desalocar a memória do registro
void desalocarRegistro(REGISTRO **registro) {
    free((*registro)->nomeJogador);
    free((*registro)->nacionalidade);
    free((*registro)->nomeClube);
    free((*registro));
    registro = NULL;
}

// Função responsável pela montagem e inserção dos registros
void criarRegistro(FILE *dados, FILE *arquivo) {
    printf("a");
    char c = '\0';
    REGISTRO *registro;
    alocarRegistro(&registro);

    // Coleta os dados enquanto não chegar ao final do arquivo csv (origem)
    while ((c = getc(dados)) != EOF) { 
        ungetc(c, dados);

        registro->removido = '0';
        fread(&registro->id, sizeof(int), 1, arquivo);
        getc(dados);
        fread(&registro->idade, sizeof(int), 1, arquivo);
        for (int i = 0; (c = getc(dados)) != ','; i++) {
            registro->nomeJogador[i] = c;
            registro->tamNomeJog = i + 1;
        }
        for (int i = 0; (c = getc(dados)) != ','; i++) {
            registro->nacionalidade[i] = c;
            registro->tamNacionalidade = i + 1;
        }
        for (int i = 0; (c = getc(dados)) != ','; i++) {
            registro->nomeClube[i] = c;
            registro->tamNomeClube = i + 1;
        }
        registro->tamanhoRegistro = (sizeof(REGISTRO) - 3 * sizeof(char *) +
            registro->tamNomeJog + registro->tamNomeClube + registro->tamNacionalidade);

        inserirRegistro(registro, arquivo);
    }
    desalocarRegistro(&registro);
}

// Função responsável por inserir o registro no arquivo bin (destino)
void inserirRegistro(REGISTRO *registro, FILE *arquivo) {
    putc(registro->removido, arquivo);
    fwrite(&registro->tamanhoRegistro, sizeof(int), 1, arquivo);
    fwrite(&registro->prox, sizeof(long int), 1, arquivo);
    fwrite(&registro->id, sizeof(int), 1, arquivo);
    fwrite(&registro->idade, sizeof(int), 1, arquivo);
    fwrite(&registro->tamNomeJog, sizeof(int), 1, arquivo);
    fputs(registro->nomeJogador, arquivo);
    fwrite(&registro->tamNacionalidade, sizeof(int), 1, arquivo);
    fputs(registro->nacionalidade, arquivo);
    fwrite(&registro->tamNomeClube, sizeof(int), 1, arquivo);
    fputs(registro->nomeClube, arquivo);
}

// Função responsável por passar os dados do arquivo .csv para o binário
void inserirDados(char *nomeArquivoCsv) {
    FILE *arquivo;      // Arquivo binário para qual serão passados os dados
    FILE *dados;        // Arquivo .csv contendo os dados
    
    // Abertura e testagem dos arquivos 
    if ((dados = fopen(nomeArquivoCsv, "r")) == NULL ||
        (arquivo = fopen("arquivo.bin", "ab")) == NULL) {
        printf("Erro na abertura do arquivo.\n");
        return;
    }

    // Pular primeira linha
    while ((fgetc(dados)) != '\n') {}
    cabecalho(arquivo);
    criarRegistro(dados, arquivo);

    fclose(arquivo);
    fclose(dados);
}
