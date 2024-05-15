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

#include "funcoes.h" // Inclui o arquivo de cabeçalho que contém as definições das funções

int main() {
    int comando = -1; // Variável para armazenar o comando escolhido pelo usuário
    int nroRepeticoes; // Variável para armazenar o número de buscas, usado na opção 3
    char nomeArquivoCsv[50]; // String para armazenar o nome do arquivo CSV
    char nomeArquivoBin[50]; // String para armazenar o nome do arquivo binário
    char nomeIndice[50];

    // Recebe o comando escolhido pelo usuário
    scanf("%d", &comando);

    // Executa a ação correspondente ao comando escolhido
    switch (comando) {
    case 1: // Caso o comando seja 1 (criar tabela)
        scanf(" %s", nomeArquivoCsv); // Recebe o nome do arquivo CSV
        scanf(" %s", nomeArquivoBin); // Recebe o nome do arquivo binário
        createTable(nomeArquivoCsv, nomeArquivoBin); // Chama a função para criar a tabela
        binarioNaTela(nomeArquivoBin); // Exibe o conteúdo do arquivo binário na tela
        break;
    case 2: // Caso o comando seja 2 (imprimir todos os registros)
        scanf(" %s", nomeArquivoBin); // Recebe o nome do arquivo
        selectFrom(nomeArquivoBin); // Chama a função para imprimir todos os registros
        break;
    case 3: // Caso o comando seja 3 (realizar buscas)
        scanf(" %s", nomeArquivoBin); // Recebe o nome do arquivo
        scanf("%d", &nroRepeticoes); // Recebe o número de buscas
        selectFromWhere(nomeArquivoBin, nroRepeticoes); // Chama a função para realizar buscas
    case 4:
        scanf(" %s", nomeArquivoBin); // Recebe o nome do arquivo
        scanf(" %s", nomeIndice); // Recebe o nome do índice
        createIndex(nomeArquivoBin, nomeIndice); // Chama a função para criar o índice
        binarioNaTela(nomeArquivoBin); // Exibe o conteúdo do índice na tela
    case 5:
        scanf(" %s", nomeArquivoBin); // Recebe o nome do arquivo
        scanf(" %s", nomeIndice); // Recebe o nome do índice
        scanf("%d", &nroRepeticoes); // Recebe o numero de remoções
        deleteFromWhere(nomeArquivoBin, nomeIndice, nroRepeticoes); // Chama a função para realizar remoções

    case 6:
        scanf(" %s", nomeArquivoBin); // Recebe o nome do arquivo
        scanf(" %s", nomeIndice); // Recebe o nome do índice
        scanf("%d", &nroRepeticoes); // Recebe o numero de inserções
        insertInto(nomeArquivoBin, nomeIndice, nroRepeticoes); // Chama a função para realizar inserções
    default:
        break;
    }
}
