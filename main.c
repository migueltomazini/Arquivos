/*
Esse programa foi desenvolvido com o objetivo de permitir ao usuário a obtenção 
de dados de um arquivo de entrada (.csv) e gerar um arquivo binário com os mesmos,
assim como realizar operações de busca. Programa desenvolvido a partir de uma 
atividade proposta na disciplina de Organização de Arquivos (SCC0215)
ministrada por Cristina Dutra de Aguiar.

Autores:
Isabela Beatriz Sousa Nunes Farias - 13823833
Miguel Rodrigues Tomazini - 14599300
(2024)
*/

#include "funcoes.h"

int main() {
    int comando = -1;
    int nroBuscas;
    char nomeArquivoCsv[50];
    char nomeArquivoBin[50];

    scanf("%d", &comando);

    switch (comando) {
    case 1:
        scanf(" %s", nomeArquivoCsv);
        scanf(" %s", nomeArquivoBin);
        createTable(nomeArquivoCsv, nomeArquivoBin);
        binarioNaTela(nomeArquivoBin);
        break;
    case 2:
        scanf (" %s", nomeArquivoBin);
        selectFrom(nomeArquivoBin);
        break;
    case 3:
        scanf(" %s", nomeArquivoBin);
        scanf("%d", &nroBuscas);
        selectFromWhere(nomeArquivoBin, nroBuscas);
    default:
        break;
    }
}