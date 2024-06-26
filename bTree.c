#include "bTree.h"  // Inclui o cabeçalho que contém as definições e funções para manipular a árvore-B.

// Escreve o cabeçalho da árvore B no início do arquivo.
void inserirCabecalhoArvB (FILE *arquivo, BTREE *cabecalho) {
    fseek(arquivo, 0, SEEK_SET);  // Posiciona o ponteiro do arquivo no início.
    fwrite(&(cabecalho->status), sizeof(char), 1, arquivo);  // Escreve o status da árvore B no arquivo.
    fwrite(&(cabecalho->noRaiz), sizeof(int), 1, arquivo);  // Escreve o RRN do nó raiz no arquivo.
    fwrite(&(cabecalho->proxRRN), sizeof(int), 1, arquivo);  // Escreve o próximo RRN disponível no arquivo.
    fwrite(&(cabecalho->nroChaves), sizeof(int), 1, arquivo);  // Escreve o número de chaves na árvore B no arquivo.
    for (int i = 0; i < 47; i++)  // Preenche o restante do cabeçalho com caracteres '$' para manter o formato fixo.
        fputc('$', arquivo);
}

// Lê o cabeçalho da árvore B no arquivo, armazenando os valores obtidos
void recuperarCabecalhoArvB (FILE *arquivo, BTREE *cabecalho) {
    fseek(arquivo, 0, SEEK_SET);  // Posiciona o ponteiro do arquivo no início.
    fread(&(cabecalho->status), sizeof(char), 1, arquivo);  // Lê o status da árvore B.
    fread(&(cabecalho->noRaiz), sizeof(int), 1, arquivo);  // Lê o RRN do nó raiz.
    fread(&(cabecalho->proxRRN), sizeof(int), 1, arquivo);  // Lê o próximo RRN disponível.
    fread(&(cabecalho->nroChaves), sizeof(int), 1, arquivo);  // Lê o número de chaves na árvore B.
}

// Inicializa um nó da árvore B com valores padrão.
void criarNo(NO_BTREE *no) {
    no->alturaNo = -1;  // Inicializa a altura do nó como -1.
    no->nroChaves = 0;  // Inicializa o número de chaves no nó como 0.
    for (int i =  0; i < MAX_CHAVES; i++) {  // Inicializa as chaves e os offsets dos registros como -1.
        no->chaves[i] = -1;
        no->byteOffset[i] = -1;
        no->ponteiroNo[i] = -1;
    }
    no->ponteiroNo[ORDEM - 1] = -1;  // Inicializa o último ponteiro do nó como -1.
    no->rrn = -1;  // Inicializa o RRN do nó como -1.
}

// Escreve um nó da árvore B em uma posição específica no arquivo. 
void escreverNo(FILE * arquivo, long int rrn, NO_BTREE *no) {
    fseek(arquivo, rrn * TAM_PAG + TAM_PAG, SEEK_SET);  // Posiciona o ponteiro do arquivo na posição correta do nó.
    fwrite(&(no->alturaNo), sizeof(int), 1, arquivo);  // Escreve a altura do nó.
    fwrite(&(no->nroChaves), sizeof(int), 1, arquivo);  // Escreve o número de chaves no nó.
    for (int i = 0; i < MAX_CHAVES; i++) {  // Escreve as chaves e os offsets dos registros no nó.
        fwrite(&(no->chaves[i]), sizeof(int), 1, arquivo);
        fwrite(&(no->byteOffset[i]), sizeof(long int), 1, arquivo);
    }
    for (int i = 0; i < ORDEM; i++)  // Escreve os ponteiros para os filhos do nó.
        fwrite(&(no->ponteiroNo[i]), sizeof(int), 1, arquivo);
}

// Lê um nó da árvore B a partir de uma posição específica no arquivo.
void recuperarNo(FILE * arquivo, long int rrn, NO_BTREE *no) {
    if(rrn == -1)  // Se o RRN é inválido, não faz nada.
        return;

    fseek(arquivo, rrn * TAM_PAG + TAM_PAG, SEEK_SET);  // Posiciona o ponteiro do arquivo na posição correta do nó.
    fread(&(no->alturaNo), sizeof(int), 1, arquivo);  // Lê a altura do nó.
    fread(&(no->nroChaves), sizeof(int), 1, arquivo);  // Lê o número de chaves no nó.
    for (int i = 0; i < MAX_CHAVES; i++) {  // Lê as chaves e os offsets dos registros no nó.
        fread(&(no->chaves[i]), sizeof(int), 1, arquivo);
        fread(&(no->byteOffset[i]), sizeof(long int), 1, arquivo);
    }
    fread(no->ponteiroNo, sizeof(int), ORDEM, arquivo);  // Lê os ponteiros para os filhos do nó.
}

//Inicializa uma árvore B, criando um novo arquivo para armazená-la.
void inicializarArvoreB(BTREE *arvore, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "wb");  // Abre o arquivo no modo de escrita binária ("wb").
    if (arquivo == NULL) {  // Verifica se o arquivo foi aberto corretamente.
        printf("Falha no processamento do arquivo.\n");  // Imprime uma mensagem de erro se o arquivo não pôde ser aberto.
        exit(0);  // Encerra o programa com código de erro 0.
    }
    arvore->status = '1';  // Define o status da árvore B como consistente ('1').
    arvore->noRaiz = -1;  // Define o RRN do nó raiz como -1 (indicando que a árvore está vazia).
    arvore->proxRRN = 0;  // Inicializa o próximo RRN disponível como 0.
    arvore->nroChaves = 0;  // Inicializa o número de chaves na árvore B como 0.

    inserirCabecalhoArvB(arquivo, arvore);  // Chama a função para escrever o cabeçalho da árvore B no arquivo.
    fclose(arquivo);  // Fecha o arquivo.
}

// Divide um nó da árvore B quando ele está cheio.
void dividirNo(int chave, long int byteOffset, NO_BTREE *no, int *chavePromovida, long int *byteOffsetPromovido, int *descendente, NO_BTREE *novoNo) {
    int i;  // Variável de controle para loops.

    // Vetores para armazenar os valores contidos no nó de forma temporária, para permitir a modificação.
    int chavesAux[MAX_CHAVES + 1];  // Vetor temporário para armazenar as chaves.
    long int byteOffsetAux[MAX_CHAVES + 1];  // Vetor temporário para armazenar os byte offsets.
    int ponteirosAux[ORDEM + 1];  // Vetor temporário para armazenar os ponteiros para os filhos dos nós.

    // Copia as chaves e os byte offsets para arrays temporários.
    for (int i = 0; i < MAX_CHAVES; i++) {
        chavesAux[i] = no->chaves[i];  // Copia cada chave do nó para o vetor temporário.
        byteOffsetAux[i] = no->byteOffset[i];  // Copia cada byte offset do nó para o vetor temporário.
        ponteirosAux[i] = no->ponteiroNo[i];  // Copia cada ponteiro do nó para o vetor temporário.
    }
    ponteirosAux[ORDEM - 1] = no->ponteiroNo[ORDEM - 1];  // Copia o último ponteiro do nó.

    // Insere a nova chave na posição correta nos arrays temporários.
    for (i = no->nroChaves - 1; i >= 0 && chave < no->chaves[i]; i--) {
        chavesAux[i + 1] = chavesAux[i];  // Desloca as chaves para a direita para abrir espaço para a nova chave.
        byteOffsetAux[i + 1] = byteOffsetAux[i];  // Desloca os byte offsets para a direita.
        ponteirosAux[i + 2] = ponteirosAux[i + 1];  // Desloca os ponteiros para a direita.
    }
    chavesAux[i + 1] = chave;  // Insere a nova chave no local correto.
    byteOffsetAux[i + 1] = byteOffset;  // Insere o byte offset da nova chave no local correto.
    ponteirosAux[i + 2] = *descendente;  // Insere o ponteiro para o descendente no local correto.

    // Define a chave que será promovida (última do nó esquerdo).
    int meio = (ORDEM / 2) - 1;  // Calcula o índice do meio.
    *chavePromovida = chavesAux[meio];  // Define a chave promovida como a chave do meio.
    *byteOffsetPromovido = byteOffsetAux[meio];  // Define o byte offset promovido como o byte offset do meio.

    // Atribuição dos devidos valores no novo nó.
    novoNo->alturaNo = no->alturaNo;  // Define a altura do novo nó igual à altura do nó atual.
    novoNo->nroChaves = meio + 1;  // Define o número de chaves no novo nó.
    for (int j = 0; j < novoNo->nroChaves; j++) {
        novoNo->chaves[j] = chavesAux[meio + 1 + j];  // Copia as chaves do vetor temporário para o novo nó.
        novoNo->byteOffset[j] = byteOffsetAux[meio + 1 + j];  // Copia os byte offsets do vetor temporário para o novo nó.
        novoNo->ponteiroNo[j] = ponteirosAux[meio + 1 + j];  // Copia os ponteiros do vetor temporário para o novo nó.
    }
    novoNo->ponteiroNo[novoNo->nroChaves] = ponteirosAux[ORDEM];  // Define o último ponteiro do novo nó.

    // Limpa o restante do nó original.
    for (int j = meio; j < MAX_CHAVES; j++) {
        no->chaves[j] = -1;  // Define as chaves não utilizadas como -1.
        no->byteOffset[j] = -1;  // Define os byte offsets não utilizados como -1.
        no->ponteiroNo[j] = -1;  // Define os ponteiros não utilizados como -1.
    }
    no->ponteiroNo[ORDEM - 1] = -1;  // Define o último ponteiro do nó original como -1.

    // Atribuição dos devidos valores no nó original.
    no->nroChaves = meio;  // Define o número de chaves no nó original.
    for (int j = 0; j < meio; j++) {
        no->chaves[j] = chavesAux[j];  // Copia as chaves do vetor temporário para o nó original.
        no->byteOffset[j] = byteOffsetAux[j];  // Copia os byte offsets do vetor temporário para o nó original.
        no->ponteiroNo[j] = ponteirosAux[j];  // Copia os ponteiros do vetor temporário para o nó original.
    }
    no->ponteiroNo[meio] = ponteirosAux[meio];  // Define o ponteiro do meio do nó original.
}

// Função auxiliar na inserção de chaves, realizando o processo de forma recursiva.
// Retorno: -1 - Chave duplicada, retornando erro na inserção.
//           0 - Inserção bem-sucedida, sem necessidade de realizar a divisão.
//           1 - Inserção bem-sucedida, com necessidade de realizar a divisão.
int inserirChaveAux(FILE *arquivo, BTREE *arvore, int rrnAtual, int chave, long int byteOffset, int *chavePromovida, long int *byteOffsetPromovido, int *descendente) {
    NO_BTREE paginaAtual;  // Declara uma variável para armazenar a página atual.
    int posicao;  // Variável para armazenar a posição correta da nova chave dentro de um determinado nó.
    
    // Caso base: Nó inexistente, indicando necessidade de promoção do anterior.
    if (rrnAtual == -1) {
        *chavePromovida = chave;  // Define a chave promovida.
        *byteOffsetPromovido = byteOffset;  // Define o byte offset promovido.
        *descendente = -1;  // Define o descendente como -1.
        return 1;  // Retorna 1 indicando que é necessário promover uma chave.
    }

    // Recupera a página atual do arquivo.
    recuperarNo(arquivo, rrnAtual, &paginaAtual);

    // Localiza a posição correta da nova chave dentro do nó.
    for (posicao = 0; posicao < paginaAtual.nroChaves && chave > paginaAtual.chaves[posicao]; posicao++);

    // Verifica se a chave já existe.
    if (posicao < paginaAtual.nroChaves && chave == paginaAtual.chaves[posicao]) {
        return -1;  // Retorna -1 indicando que a chave é duplicada.
    }

    // Chamada recursiva para inserir a chave na subárvore correta.
    int retorno = inserirChaveAux(arquivo, arvore, paginaAtual.ponteiroNo[posicao], chave, byteOffset, chavePromovida, byteOffsetPromovido, descendente);
    if (retorno == 0 || retorno == -1) {
        return retorno;  // Retorna o resultado da chamada recursiva.
    }

    // Trata o caso no qual há espaço no nó atual.
    if (paginaAtual.nroChaves < MAX_CHAVES) {
        // Move as chaves e byte offsets para abrir espaço para a nova chave.
        for (int j = paginaAtual.nroChaves; j > posicao; j--) {
            paginaAtual.chaves[j] = paginaAtual.chaves[j - 1];
            paginaAtual.byteOffset[j] = paginaAtual.byteOffset[j - 1];
            paginaAtual.ponteiroNo[j + 1] = paginaAtual.ponteiroNo[j];
        }
        paginaAtual.chaves[posicao] = *chavePromovida;  // Insere a nova chave.
        paginaAtual.byteOffset[posicao] = *byteOffsetPromovido;  // Insere o byte offset da nova chave.
        paginaAtual.ponteiroNo[posicao + 1] = *descendente;  // Define o ponteiro para o descendente.
        paginaAtual.nroChaves++;  // Incrementa o número de chaves no nó.

        escreverNo(arquivo, rrnAtual, &paginaAtual);  // Escreve a página atual no arquivo.
        return 0;  // Retorna 0 indicando que a inserção foi bem-sucedida.
    } 
    // Trata o caso no qual é necessário criar um novo nó, pois não há espaço suficiente.
    else {
        NO_BTREE novoNo;  // Declara um novo nó para a árvore-B.
        criarNo(&novoNo);  // Inicializa o novo nó.

        // Divide o nó atual, promovendo uma chave para o nó pai.
        dividirNo(*chavePromovida, *byteOffsetPromovido, &paginaAtual, chavePromovida, byteOffsetPromovido, descendente, &novoNo);

        novoNo.rrn = arvore->proxRRN++;  // Atualiza o RRN do novo nó e incrementa o próximo RRN disponível.
        *descendente = novoNo.rrn;  // Define o descendente para o novo nó.

        escreverNo(arquivo, rrnAtual, &paginaAtual);  // Escreve a página atual no arquivo.
        escreverNo(arquivo, novoNo.rrn, &novoNo);  // Escreve o novo nó no arquivo.

        return 1;  // Retorna 1 indicando que foi necessário dividir o nó.
    }
}

// Função responsável por inserir uma chave na árvore B.
void inserirChave(BTREE *arvore, const char *nomeArquivo, int chave, long int byteOffset) {
    int chavePromovida;  // Variável para armazenar a chave promovida caso um split seja necessário.
    long int byteOffsetPromovido;  // Variável para armazenar o byte offset da chave promovida.
    int descendente;  // Variável para armazenar o descendente do novo nó caso seja necessário realizar a divisão.

    // Abertura e testagem do arquivo.
    FILE *arquivo;
    if ((arquivo = fopen(nomeArquivo, "r+b")) == NULL) {  // Abre o arquivo no modo de leitura e escrita binária.
        printf("Falha no processamento do arquivo.\n");  // Imprime uma mensagem de erro se o arquivo não pôde ser aberto.
        return;  // Retorna da função.
    }

    // Modificar status para inconsistente.
    arvore->status = '0';  // Define o status da árvore como inconsistente ('0').
    fseek(arquivo, 0, SEEK_SET);  // Posiciona o ponteiro do arquivo no início.
    fwrite(&arvore->status, sizeof(char), 1, arquivo);  // Escreve o status no arquivo.

    recuperarCabecalhoArvB(arquivo, arvore);  // Recupera o cabeçalho da árvore B do arquivo.

    // Chama a função auxiliar para inserir a chave, passando os parâmetros necessários.
    int retorno = inserirChaveAux(arquivo, arvore, arvore->noRaiz, chave, byteOffset, &chavePromovida, &byteOffsetPromovido, &descendente);

    // Caso no qual é necessário criar uma nova raiz, devido à promoção.
    if (retorno == 1) {
        NO_BTREE no;  // Declara uma variável para armazenar o nó atual.
        NO_BTREE novoNo;  // Declara uma variável para armazenar o novo nó.

        criarNo(&novoNo);  // Inicializa o novo nó.
        recuperarNo(arquivo, arvore->noRaiz, &no);  // Recupera o nó raiz do arquivo.

        // Define os valores do novo nó raiz.
        novoNo.alturaNo = arvore->noRaiz == -1 ? 0 : (no.alturaNo + 1);  // Define a altura do novo nó raiz.
        novoNo.nroChaves = 1;  // Define o número de chaves no novo nó raiz.
        novoNo.chaves[0] = chavePromovida;  // Define a chave promovida como a primeira chave do novo nó raiz.
        novoNo.byteOffset[0] = byteOffsetPromovido;  // Define o byte offset da chave promovida.
        novoNo.ponteiroNo[0] = arvore->noRaiz;  // Define o ponteiro para o nó raiz antigo.
        novoNo.ponteiroNo[1] = descendente;  // Define o ponteiro para o descendente.
        novoNo.rrn = arvore->proxRRN++;  // Define o RRN do novo nó raiz e incrementa o próximo RRN disponível.

        arvore->noRaiz = novoNo.rrn;  // Define o novo nó raiz na árvore B.
        escreverNo(arquivo, novoNo.rrn, &novoNo);  // Escreve o novo nó raiz no arquivo.
        fseek(arquivo, 0, SEEK_END);  // Posiciona o ponteiro do arquivo no final.
    }

    // Atualização do cabeçalho, marcando status como consistente e mudando o número de chaves.
    arvore->nroChaves++;  // Incrementa o número de chaves na árvore B.
    arvore->status = '1';  // Define o status da árvore como consistente ('1').
    inserirCabecalhoArvB(arquivo, arvore);  // Escreve o cabeçalho atualizado no arquivo.

    fclose(arquivo);  // Fecha o arquivo.
}

// Função responsável por buscar uma determinada chave na árvore B.
long int buscarChave(BTREE *arvore, FILE *arquivo, long int rrn, int chave) {
    int i = 0;  // Variável de controle para loops.
    NO_BTREE no;  // Declara uma variável para armazenar o nó atual.

    recuperarNo(arquivo, rrn, &no);  // Recupera o nó a partir do RRN fornecido.

    // Percorre as chaves do nó até encontrar a chave desejada ou uma chave maior.
    for (; i < no.nroChaves && chave > no.chaves[i]; i++);

    // Chave foi encontrada.
    if (i < no.nroChaves && chave == no.chaves[i])
        return no.byteOffset[i];  // Retorna o byte offset da chave encontrada.
    // Chave não foi encontrada e não há mais níveis para procurar.
    else if (no.alturaNo == 0)
        return -1;  // Retorna -1 indicando que a chave não foi encontrada.
    // Descer um nível para procurar a chave.
    else
        return buscarChave(arvore, arquivo, no.ponteiroNo[i], chave);  // Chama recursivamente a função para procurar a chave no nível inferior.
}
