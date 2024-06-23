#include "bTree.h"

int main() {
    CABECALHO_BTREE arvore;

    // Inicializa a árvore B e cria o arquivo de dados
    inicializarArvoreB(&arvore, "arvoreB.bin");

    // Insere algumas chaves na árvore B
    inserirChave(&arvore, "arvoreB.bin", 10, 100);
    inserirChave(&arvore, "arvoreB.bin", 20, 200);
    inserirChave(&arvore, "arvoreB.bin", 5, 50);
    inserirChave(&arvore, "arvoreB.bin", 15, 150);
    inserirChave(&arvore, "arvoreB.bin", 25, 250);
    inserirChave(&arvore, "arvoreB.bin", 30, 300);

    // Busca por algumas chaves na árvore B e imprime os resultados
    long result = buscarChave(&arvore, fopen("arvoreB.bin", "rb"), arvore.noRaiz, 10);
    if (result != -1) {
        printf("Chave 10 encontrada no byte offset: %ld\n", result);
    } else {
        printf("Chave 10 não encontrada.\n");
    }

    result = buscarChave(&arvore, fopen("arvoreB.bin", "rb"), arvore.noRaiz, 20);
    if (result != -1) {
        printf("Chave 20 encontrada no byte offset: %ld\n", result);
    } else {
        printf("Chave 20 não encontrada.\n");
    }

    result = buscarChave(&arvore, fopen("arvoreB.bin", "rb"), arvore.noRaiz, 25);
    if (result != -1) {
        printf("Chave 25 encontrada no byte offset: %ld\n", result);
    } else {
        printf("Chave 25 não encontrada.\n");
    }

    result = buscarChave(&arvore, fopen("arvoreB.bin", "rb"), arvore.noRaiz, 5);
    if (result != -1) {
        printf("Chave 5 encontrada no byte offset: %ld\n", result);
    } else {
        printf("Chave 5 não encontrada.\n");
    }

    return 0;
}