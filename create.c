#include "funcoes.h"

// Função responsável pela montagem e inserção do cabeçalho
void cabecalho(FILE *arquivo, int nroRegArq) {
    CABECALHO *cabecalho = malloc(sizeof(CABECALHO));
    int tamCabecalho = sizeof(char) + 2 * sizeof(int) + 2 * sizeof(long int);

    if (cabecalho == NULL)
        return;
    
    cabecalho->status = '1';
    cabecalho->topo = -1;
    cabecalho->prosByteOffset = 0;
    cabecalho->nroRegArq = nroRegArq;
    cabecalho->nroRegRem = 0;

    putc(cabecalho->status, arquivo);
    fwrite(&cabecalho->topo, sizeof(long int), 1, arquivo);
    fwrite(&cabecalho->prosByteOffset, sizeof(long int), 1, arquivo);
    fwrite(&cabecalho->nroRegArq, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->nroRegRem, sizeof(int), 1, arquivo);

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
    if ((*registro)->nomeJogador == NULL || (*registro)->nomeClube == NULL || 
        (*registro)->nacionalidade == NULL)
        exit(1);
}

// Função auxiliar responsável por desalocar a memória do registro
void desalocarRegistro(REGISTRO **registro) {
    free((*registro)->nomeJogador);
    free((*registro)->nacionalidade);
    free((*registro)->nomeClube);
    free((*registro));
    *registro = NULL;
}

// Função responsável pela montagem e inserção dos registros
void criarRegistro(FILE *dados, FILE *arquivo) {        // Mudar nome apenas para Registro
    char c = '\0';                                      // Criar nova função apenas para coletar os dados
    REGISTRO *registro;

    int i = 0;
    int regAdicionados = 0;
    long int proxByteOffset = TAM_CABECALHO;
    char charId[8];
    char charIdade[4];

    alocarRegistro(&registro);

    // Coleta os dados enquanto não chegar ao final do arquivo csv (origem)
    while ((c = getc(dados)) != EOF) { 
        ungetc(c, dados);

        registro->prox = -1;

        for (i = 0; (c = getc(dados)) != ','; i++)      // Id
            charId[i] = c;
        charId[i++] = '\0';
        registro->id = atoi(charId);

        for (i = 0; (c = getc(dados)) != ','; i++)      // Idade
            charIdade[i] = c;
        charIdade[i++] = '\0';
        if (i == 1)    
            registro->idade = -1;   // Se não houver idade registrada, marcar como -1
        else
            registro->idade = atoi(charIdade);

        registro->removido = '0';

        for (i = 0; (c = getc(dados)) != ','; i++)      // Nome e tamanho Jogador
            registro->nomeJogador[i] = c;
        registro->nomeJogador[i] = '\0';
        registro->tamNomeJog = i;
        
        for (i = 0; (c = getc(dados)) != ','; i++)      // Nome e tamanho nacionalidade
            registro->nacionalidade[i] = c;
        registro->nacionalidade[i] = '\0';
        registro->tamNacionalidade = i;
        
        for (i = 0; (c = getc(dados)) != '\n'; i++)     // Nome e tamanho Clube
            registro->nomeClube[i] = c;
            registro->nomeClube[i] = '\0';
        registro->tamNomeClube = i;
        
        registro->tamanhoRegistro = (sizeof(char) + 6 * sizeof(int) + sizeof(long int) +
            registro->tamNomeJog + registro->tamNomeClube + registro->tamNacionalidade);

        inserirRegistro(registro, arquivo);
        regAdicionados++;
        proxByteOffset += registro->tamanhoRegistro;
    }

    // Atualizar cabeçalho
    fseek(arquivo, 9, SEEK_SET);
    fwrite(&proxByteOffset, sizeof(long int), 1, arquivo);
    fseek(arquivo, 17, SEEK_SET);
    fwrite(&regAdicionados, sizeof(int), 1, arquivo);

    desalocarRegistro(&registro);
}

// Função responsável por inserir o registro no arquivo bin (destino)
void inserirRegistro(REGISTRO *registro, FILE *arquivo) {
    if (registro == NULL || arquivo == NULL)
        return;

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
void createTable(char *nomeArquivoCsv, char *nomeArquivoBin) {
    FILE *arquivo;      // Arquivo binário para qual serão passados os dados
    FILE *dados;        // Arquivo .csv contendo os dados

    // Abertura e testagem dos arquivos 
    if ((dados = fopen(nomeArquivoCsv, "r")) == NULL ||
        (arquivo = fopen(nomeArquivoBin, "wb")) == NULL) { // Abrir arquivo no modo escrita
        printf("Erro na abertura do arquivo.\n");
        return;
    }

    // Pular primeira linha
    while ((fgetc(dados)) != '\n') {}
    cabecalho(arquivo, 0);
    criarRegistro(dados, arquivo);

    fclose(arquivo);
    fclose(dados);
}