#include "funcoes.h"

// Função auxiliar para auxiliar a recuperar um registro
void recuperarRegistro(REGISTRO **registro, FILE *arquivo) {
    REGISTRO *reg = *registro;
    int i = 0;
    char teste[30];
    char c;

    fread(&reg->removido, sizeof(char), 1, arquivo);
    fread(&reg->tamanhoRegistro, sizeof(int), 1, arquivo);
    fread(&reg->prox, sizeof(long int), 1, arquivo);
    fread(&reg->id, sizeof(int), 1, arquivo);
    fread(&reg->idade, sizeof(int), 1, arquivo);

    fread(&reg->tamNomeJog, sizeof(int), 1, arquivo);
    for(i = 0; i < reg->tamNomeJog; i++)
        reg->nomeJogador[i] = getc(arquivo);
    reg->nomeJogador[i] = '\0';

    fread(&reg->tamNacionalidade, sizeof(int), 1, arquivo);
    for(i = 0; i < reg->tamNacionalidade; i++)
        reg->nacionalidade[i] = getc(arquivo);
    reg->nacionalidade[i] = '\0';

    fread(&reg->tamNomeClube, sizeof(int), 1, arquivo);
    for(i = 0; i < reg->tamNomeClube; i++)
        reg->nomeClube[i] = getc(arquivo);
    reg->nomeClube[i] = '\0';
}

// Função auxiliar para abrir (no modo leitura) e realizar testagem do arquivo
int testarArquivo(FILE *arquivo, char *nomeArquivo) {
    char status;

    if (arquivo == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return 1;
    }
    if (fread(&status, sizeof(char), 1, arquivo) == 0) {
        printf("Falha no processamento do arquivo.\n");
        return 1;
    }

    return 0;       // Retorna 0 se a abertura ocorrer sem problemas
}

int comandoBusca(int *nroComandos, char (*comando)[5][10], char (*palavraChave)[5][30]) {
    int i;
    int j;
    char c = '\0';
    char temp[30];

    scanf("%d", nroComandos);

    getchar();     // Pegar o caractere '\n'
    
    for (j = 0; j < *nroComandos; j++) {
        c = getchar();
        for (i = 0; c != ' '; i++) {
            (*comando)[j][i] = c;
            c = getchar();
        }
        (*comando)[j][i] = '\0';

        if (strcmp((*comando)[j], "id")  == 0 || 
            strcmp((*comando)[j], "idade") == 0) {
            c = getchar();
            for (i = 0; (c != ' ') && (c != '\n'); i++) {
                (*palavraChave)[j][i] = c;
                c = getchar();
            }
            (*palavraChave)[j][i] = '\0';
        } else {
            scan_quote_string(temp);
            strcpy((*palavraChave)[j], temp);
            c = getchar();
        }

        //printf("%s\n", (*comando)[j]);
        //printf("%s\n", (*palavraChave)[j]);
    }
    return j;
}

/* Função que realiza a busca apartir de um comando (categoria) e a palavra chave
   Retorno: 0 - Registro possui os tributos buscados
            1 - Registro não possui os tributos buscados
            2 - Registro possui os atributos buscados e busca deve ser interrompida (comando = "id")
            3 - Registro não possui os atributos buscados e busca deve ser interrompida (comando = "id")*/
int busca(REGISTRO *registro, int nroComandos, char comando[5][10], char palavraChave[5][30]) { // acertar tamanho
    char c;
    int temp;
    int retorno = 1;
    //printf("%s\n", comando[0]);
    //printf("%s\n", palavraChave[0]);

    for (int i = 0; i < nroComandos; i++) {
        //printf("%s\n", comando[i]);
        //printf("%s\n", palavraChave[i]);
        //printf("%d\n", atoi(palavraChave[i]));
        if (strcmp(comando[i], "id") == 0) {
            temp = atoi(palavraChave[i]);
            if (temp != registro->id)
                return retorno;
            retorno = 3;
        }
        if (strcmp(comando[i], "idade") == 0) {
            temp = atoi(palavraChave[i]);
            if (temp != registro->idade)
                return retorno;
        }
        if (strcmp(comando[i], "nomeJogador") == 0) {
            if (strcmp(palavraChave[i], registro->nomeJogador) != 0)
                return retorno;
        }
        if (strcmp(comando[i], "nacionalidade") == 0) {
            if (strcmp(palavraChave[i], registro->nacionalidade) != 0)
                return retorno;
        }
        if (strcmp(comando[i], "nomeClube") == 0) {
            if (strcmp(palavraChave[i], registro->nomeClube) != 0)
                return retorno;
        }   
    }


    return (retorno - 1);        // O registro possui os atributos buscados se o retorno for 0 ou 2
}

void impressaoRegistro(REGISTRO *registro) {
    if (registro->removido == '0') { // Verifica se o registro não foi removido  
            if (registro->tamNomeJog != 0)
                printf("Nome do jogador: %s\n", registro->nomeJogador);
            else
                printf("Nome do jogador: SEM DADO\n");
            if (registro->tamNacionalidade != 0)
                printf("Nacionalidade do Jogador: %s\n", registro->nacionalidade);
            else
                printf("Nacionalidade do Jogador: SEM DADO\n");
            if (registro->tamNomeClube != 0)
                printf("Clube do jogador: %s\n", registro->nomeClube);
            else
                printf("Clube do jogador: SEM DADO\n");
            printf("\n");
        }
}

// Lista todos os dados de um arquivo de entrada de forma organizada (Funcionalidade 2)
void selectFrom(char *nomeArquivo) {
    char c = '\0';
    int nroRegistros = 0;

    REGISTRO *registro;
    alocarRegistro(&registro);

    FILE *arquivo = NULL;
    arquivo = fopen(nomeArquivo, "rb");

    if (registro == NULL || testarArquivo(arquivo, nomeArquivo) == 1) {
        desalocarRegistro(&registro);
        return;
    }

    fseek(arquivo, TAM_CABECALHO, SEEK_SET); // Pular cabeçalho    

    while ((c = getc(arquivo)) != EOF) {
            ungetc(c, arquivo);

            recuperarRegistro(&registro, arquivo);

            if (registro->removido == '0') {
                impressaoRegistro(registro);
                nroRegistros++;
            }
    }

    if (nroRegistros == 0)      // Se uma determinada busca não retornar registros
            printf("Registro inexistente.\n\n");

    desalocarRegistro(&registro);

    // Fecha o arquivo após a leitura
    fclose(arquivo);
}

// Função que realiza n buscas em um dado arquivo (Funcionalidade 3)
void selectFromWhere(char *nomeArquivo, int nroBuscas) {
    char comando[5][10];        // Contêm os campos que devem ser buscados (Ex.id, nome)
    char palavraChave[5][30];   // Contêm o que deve ser buscado (Ex. 1637, "Calleri")
    char c = '\0';
    int nroComandos = 0;        // Número de comandos em cada busca
    int nroRegistros = 0;       // Número de registros encontrados em uma busca
    int retornoBusca = 0;       // Retorno da função Busca
    
    FILE *arquivo = NULL;       // Arquivo no qual a busca será realizada
    arquivo = fopen(nomeArquivo, "rb");

    REGISTRO *registro;
    alocarRegistro(&registro);

    if (registro == NULL || testarArquivo(arquivo, nomeArquivo) == 1) {
        desalocarRegistro(&registro);
        return;
    }

    for (int j = 1; j <= nroBuscas; j++) {
        printf("Busca %d\n\n", j);
        nroRegistros = 0;

        fseek(arquivo, TAM_CABECALHO, SEEK_SET);    // Voltar ao início desconsiderando cabeçalho
        nroComandos = comandoBusca(&nroComandos, &comando, &palavraChave);
        while ((c = getc(arquivo)) != EOF) {
            ungetc(c, arquivo);

            recuperarRegistro(&registro, arquivo);

            if (registro->removido == '0') {
                retornoBusca = busca(registro, nroComandos, comando, palavraChave);

                if (retornoBusca == 0 || retornoBusca == 2) {
                    impressaoRegistro(registro);
                    nroRegistros++;
                }

                if (retornoBusca == 2 || retornoBusca == 3)
                    break;
            }
        }
        if (nroRegistros == 0)      // Se uma determinada busca não retornar registros
            printf("Registro inexistente.\n\n");
    }
    printf("\n");

    desalocarRegistro(&registro);
}