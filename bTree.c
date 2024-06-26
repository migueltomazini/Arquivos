#include "bTree.h"
//#include "funcoesAuxiliares.h" -- VOLTAR

// Inicializa um nó da árvore B com valores padrão.
void criarNo(NO_BTREE *no) {
    no->alturaNo = -1;
    no->nroChaves = 0;
    for (int i =  0; i < MAX_CHAVES; i++) {
        no->chaves[i] = -1;
        no->byteOffset[i] = -1;
        no->ponteiroNo[i] = -1;
    }
    no->ponteiroNo[ORDEM - 1] = -1;
    no->rrn = -1;
}

// Escreve o cabeçalho da árvore B no início do arquivo.
void inserirCabecalhoArvB (FILE *arquivo, BTREE *cabecalho) {
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&(cabecalho->status), sizeof(char), 1, arquivo);
    fwrite(&(cabecalho->noRaiz), sizeof(int), 1, arquivo);
    fwrite(&(cabecalho->proxRRN), sizeof(int), 1, arquivo);
    fwrite(&(cabecalho->nroChaves), sizeof(int), 1, arquivo);
    for (int i = 0; i < 47; i++)
        fputc('$', arquivo);
}

// Lê o cabeçalho da árvore B a partir do arquivo e retorna um ponteiro para ele.
void recuperarCabecalhoArvB (FILE *arquivo, BTREE *cabecalho) {
    fseek(arquivo, 0, SEEK_SET);
    fread(&(cabecalho->status), sizeof(char), 1, arquivo);
    fread(&(cabecalho->noRaiz), sizeof(int), 1, arquivo);
    fread(&(cabecalho->proxRRN), sizeof(int), 1, arquivo);
    fread(&(cabecalho->nroChaves), sizeof(int), 1, arquivo);
}

// Escreve um nó da árvore B em uma posição específica no arquivo. 
void escreverNo(FILE * arquivo, long rrn, NO_BTREE *no) {
    fseek(arquivo, rrn * TAM_PAG + TAM_PAG, SEEK_SET);
    fwrite(&(no->alturaNo), sizeof(int), 1, arquivo);
    fwrite(&(no->nroChaves), sizeof(int), 1, arquivo);
    for (int i = 0; i < MAX_CHAVES; i++) {
        fwrite(&(no->chaves[i]), sizeof(int), 1, arquivo);
        fwrite(&(no->byteOffset[i]), sizeof(long), 1, arquivo);
    }
    for (int i = 0; i < ORDEM; i++)
        fwrite(&(no->ponteiroNo[i]), sizeof(int), 1, arquivo);
}

// Lê um nó da árvore B a partir de uma posição específica no arquivo.
void recuperarNo(FILE * arquivo, long rrn, NO_BTREE *no) {
    if(rrn == -1)
        return;

    fseek(arquivo, rrn * TAM_PAG + TAM_PAG, SEEK_SET);
    fread(&(no->alturaNo), sizeof(int), 1, arquivo);
    fread(&(no->nroChaves), sizeof(int), 1, arquivo);
    for (int i = 0; i < MAX_CHAVES; i++) {
        fread(&(no->chaves[i]), sizeof(int), 1, arquivo);
        fread(&(no->byteOffset[i]), sizeof(long), 1, arquivo);
    }
    fread(no->ponteiroNo, sizeof(int), ORDEM, arquivo);
}

// Inicializa uma árvore B, criando um novo arquivo para armazená-la.
void inicializarArvoreB(BTREE *arvore, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "wb");
    if (arquivo == NULL) {
        perror("Erro ao abrir arquivo");
        exit(1);
    }
    arvore->status = '1'; // Consistente
    arvore->noRaiz = -1;
    arvore->proxRRN = 0;
    arvore->nroChaves = 0;
    // for (int i = 0; i < 47; i++)
    //     arvore->lixo[i] = '$';

    inserirCabecalhoArvB(arquivo, arvore);
    fclose(arquivo);
}

// Divide um nó da árvore B quando ele está cheio.
void dividirNo(int chave, long byteOffset, NO_BTREE *no, int *chavePromovida, long *byteOffsetPromovido, int *descendenteDireita, NO_BTREE *novoNo) {
    int i;
    
    // Vetores para armazenar os valores contidos no nó de forma temporária, para permitir a modificação
    int chavesAux[MAX_CHAVES + 1];
    long byteOffsetAux[MAX_CHAVES + 1];
    int ponteirosAux[ORDEM + 1];

    // Copia as chaves e os byte offsets para arrays temporários
    for (int i = 0; i < MAX_CHAVES; i++) {
        chavesAux[i] = no->chaves[i];
        byteOffsetAux[i] = no->byteOffset[i];
        ponteirosAux[i] = no->ponteiroNo[i];
    }
    // ponteirosAux[no->nroChaves] = no->ponteiroNo[MAX_CHAVES - 1];
    // ponteirosAux[MAX_CHAVES] = *descendenteDireita;
    ponteirosAux[ORDEM - 1] = no->ponteiroNo[ORDEM - 1];

    // for (int i = 0; i < ORDEM + 1; i++) {
    //     printf("%d ", ponteirosAux[i]);
    // }
    // printf("\n");

    // Insere a nova chave na posição correta nos arrays temporários
    for (i = no->nroChaves - 1; i >= 0 && chave < no->chaves[i]; i--) {
        chavesAux[i + 1] = chavesAux[i];
        byteOffsetAux[i + 1] = byteOffsetAux[i];
        ponteirosAux[i + 2] = ponteirosAux[i + 1];
    }
    chavesAux[i + 1] = chave;
    byteOffsetAux[i + 1] = byteOffset;
    ponteirosAux[i + 2] = *descendenteDireita;

    // for (int i = 0; i < ORDEM + 1; i++) {
    //     printf("%d ", ponteirosAux[i]);
    // }
    // printf("\n");

    // Definir a chave que será promovida
    int meio = (ORDEM / 2) - 1;
    *chavePromovida = chavesAux[meio];
    *byteOffsetPromovido = byteOffsetAux[meio];

    // Alocação dos devidos valores no novo nó
    novoNo->alturaNo = no->alturaNo;
    novoNo->nroChaves = meio + 1;
    for (int j = 0; j < novoNo->nroChaves; j++) {
        novoNo->chaves[j] = chavesAux[meio + 1 + j];
        novoNo->byteOffset[j] = byteOffsetAux[meio + 1 + j];
        novoNo->ponteiroNo[j] = ponteirosAux[meio + 1 + j];
    }
    novoNo->ponteiroNo[novoNo->nroChaves] = ponteirosAux[ORDEM];

    // for (int i = 0; i < ORDEM; i++) {
    //     printf("%d ", novoNo->ponteiroNo[i]);
    // }
    // printf("\n");

    // Limpa o restante do nó original
    for (int j = meio; j < MAX_CHAVES; j++) {
        no->chaves[j] = -1;
        no->byteOffset[j] = -1;
        no->ponteiroNo[j] = -1;
    }
    no->ponteiroNo[ORDEM - 1] = -1;

    // Alocação dos devidos valores no nó original
    no->nroChaves = meio;
    for (int j = 0; j < meio; j++) {
        no->chaves[j] = chavesAux[j];
        no->byteOffset[j] = byteOffsetAux[j];
        no->ponteiroNo[j] = ponteirosAux[j];
    }
    no->ponteiroNo[meio] = ponteirosAux[meio];
}

// Função responsável por inserir chaves de forma recursiva
int inserirChaveRecursivo(FILE *arquivo, BTREE *arvore, int rrnAtual, int chave, long byteOffset, int *chavePromovida, long *byteOffsetPromovido, int *descendenteDireita) {
    // Caso base: Nó inexistente, indicando necessidade de promoção do anterior
    if (rrnAtual == -1) {
        *chavePromovida = chave;
        *byteOffsetPromovido = byteOffset;
        *descendenteDireita = -1;
        return 1;
    }

    NO_BTREE paginaAtual;
    recuperarNo(arquivo, rrnAtual, &paginaAtual);

    // Localiza a posição correta da nova chave dentro do nó
    int posicao;
    for (posicao = 0; posicao < paginaAtual.nroChaves && chave > paginaAtual.chaves[posicao]; posicao++);

    // Verifica se chave já existe
    if (posicao < paginaAtual.nroChaves && chave == paginaAtual.chaves[posicao]) {
        printf("Erro: chave duplicada\n");
        return -1;
    }

    // Chamada recursiva para inserir a chave na subárvore correta
    int retorno = inserirChaveRecursivo(arquivo, arvore, paginaAtual.ponteiroNo[posicao], chave, byteOffset, chavePromovida, byteOffsetPromovido, descendenteDireita);
    if (retorno == 0 || retorno == -1) {
        return retorno;
    }

    // Trata o caso no qual há espaço no nó atual
    if (paginaAtual.nroChaves < MAX_CHAVES) {
        for (int j = paginaAtual.nroChaves; j > posicao; j--) {
            paginaAtual.chaves[j] = paginaAtual.chaves[j - 1];
            paginaAtual.byteOffset[j] = paginaAtual.byteOffset[j - 1];
            paginaAtual.ponteiroNo[j + 1] = paginaAtual.ponteiroNo[j];
        }
        paginaAtual.chaves[posicao] = *chavePromovida;
        paginaAtual.byteOffset[posicao] = *byteOffsetPromovido;
        paginaAtual.ponteiroNo[posicao + 1] = *descendenteDireita;
        paginaAtual.nroChaves++;

        escreverNo(arquivo, rrnAtual, &paginaAtual);
        return 0;
    } 
    // Trata o caso no qual é necessário criar um novo nó, realizando , posteriormente, a divisão
    else {
        NO_BTREE novoNo;
        criarNo(&novoNo);

        dividirNo(*chavePromovida, *byteOffsetPromovido, &paginaAtual, chavePromovida, byteOffsetPromovido, descendenteDireita, &novoNo);

        novoNo.rrn = arvore->proxRRN++;
        *descendenteDireita = novoNo.rrn;

        escreverNo(arquivo, rrnAtual, &paginaAtual);
        escreverNo(arquivo, novoNo.rrn, &novoNo);

        return 1;
    }
}

// Função responsável por inserir uma chave na árvore B.
void inserirChave(BTREE *arvore, const char *nomeArquivo, int chave, long byteOffset) {
    int chavePromovida;         // Chave a ser promovida caso um split seja necessário
    long byteOffsetPromovido;   // ByteOffset da chave promovida
    int descendenteDireita;
    
    FILE *arquivo;
    if ((arquivo = fopen(nomeArquivo, "r+b")) == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // if (testarArquivo(arquivo) == 1)
    //     exit(1); 

    recuperarCabecalhoArvB(arquivo, arvore);
    arvore->status = '0';
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&arvore->status, sizeof(char), 1, arquivo);

    printf("A chave que vou tentar inserir: %d\n", chave);

    int retorno = inserirChaveRecursivo(arquivo, arvore, arvore->noRaiz, chave, byteOffset, &chavePromovida, &byteOffsetPromovido, &descendenteDireita);

    // Caso no qual a árvore está vazia, sendo necessário criar um nó raiz
    if (retorno == 2) {
        NO_BTREE novoNo;
        criarNo(&novoNo);
        novoNo.alturaNo = 0;
        novoNo.nroChaves = 1;
        novoNo.chaves[0] = chavePromovida;
        novoNo.byteOffset[0] = byteOffsetPromovido;
        novoNo.rrn = arvore->proxRRN++;
        arvore->noRaiz = novoNo.rrn;
        escreverNo(arquivo, novoNo.rrn, &novoNo);
    } 
    // Caso no qual é necessário criar uma nova raiz, devido a promoção
    else if (retorno == 1) {
        NO_BTREE no;
        NO_BTREE novoNo;
        criarNo(&novoNo);
        recuperarNo(arquivo, arvore->noRaiz, &no);
        novoNo.alturaNo = arvore->noRaiz == -1 ? 0 : (no.alturaNo + 1);
        novoNo.nroChaves = 1;
        novoNo.chaves[0] = chavePromovida;
        novoNo.byteOffset[0] = byteOffsetPromovido;
        novoNo.ponteiroNo[0] = arvore->noRaiz;
        novoNo.ponteiroNo[1] = descendenteDireita;
        novoNo.rrn = arvore->proxRRN++;
        arvore->noRaiz = novoNo.rrn;
        escreverNo(arquivo, novoNo.rrn, &novoNo);
        fseek(arquivo, 0, SEEK_END);
    }

    arvore->nroChaves++;
    arvore->status = '1';
    inserirCabecalhoArvB(arquivo, arvore);
    printRaiz(arvore, arquivo);
    fclose(arquivo);
}

// Função responsável pela impressão dos descendentes -- APAGAR DEPOIS
void printDescendente(BTREE * arvore, int rrn, FILE * arquivo) {
    NO_BTREE no;
    criarNo(&no);

    recuperarNo(arquivo, rrn, &no);
    printf("-------------------------\n");

    printf("altura no:%d\nnrochaves:%d\n", no.alturaNo, no.nroChaves);
    for (int i = 0; i < MAX_CHAVES; i++)
    {
        printf("|| %d | %d | %ld ", no.ponteiroNo[i], no.chaves[i], no.byteOffset[i]);
    }
    printf("%d\n", no.ponteiroNo[MAX_CHAVES - 1]);
    printf("-------------------------\n");
}

// Função responsável pela impressão da raiz -- APAGAR DEPOIS
void printRaiz(BTREE * arvore, FILE * arquivo) {
    NO_BTREE no;
    criarNo(&no);

    recuperarNo(arquivo, arvore->noRaiz, &no);
    printf("-------------------------\n");
    printf("altura no:%d\nnrochaves:%d\n", no.alturaNo, no.nroChaves);
    for (int i = 0; i < MAX_CHAVES; i++)
    {
        printf("|| %d | %d | %ld ", no.ponteiroNo[i], no.chaves[i], no.byteOffset[i]);
    }
    printf("%d\n", no.ponteiroNo[MAX_CHAVES - 1]);
    printf("-------------------------\n");

    
    if(no.ponteiroNo[0] == 0){
        printDescendente(arvore, 0, arquivo);
    }
    if(no.ponteiroNo[1] == 1){
        printDescendente(arvore, 1, arquivo);
    }  
}

// Ajeitar
// Função responsável por buscar uma determinada chave na árvore B.
long buscarChave(BTREE *arvore, FILE *arquivo, long rrn, int chave) {
    int i = 0;
    NO_BTREE no;

    recuperarNo(arquivo, rrn, &no);

    for (; i < no.nroChaves && chave > no.chaves[i]; i++);
    
    if (i < no.nroChaves && chave == no.chaves[i]) {
        return no.byteOffset[i]; // Chave encontrada
    } else if (no.alturaNo == 0) {
        return -1; // Não há ponteiroNo para procurar
    } else {
        // Desce para o próximo nível da árvore
        return buscarChave(arvore, arquivo, no.ponteiroNo[i], chave);
    }
}
