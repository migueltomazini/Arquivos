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

#include "arquivo.h"
#include "funcoes_fornecidas.h"

int main() {
    int comando = -1;
    char nomeArquivoCsv[50];
    char nomeArquivoBin[50];

    scanf("%d", &comando);

    switch (comando) {
    case 1:
        scanf(" %s", nomeArquivoCsv);
        scanf(" %s", nomeArquivoBin);
        csvToBin(nomeArquivoCsv, nomeArquivoBin);
        break;
    default:
        break;
    }
}