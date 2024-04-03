/* Programa que contêm as operações de busca*/

#ifndef OPERACOES_H
    #define OPERACOES_H
    
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    
    // Lista o arquivo de saída no modo binário usando a função fornecida binarioNaTela
    void createTable(FILE *entrada, FILE *saida);
    // Lista n dados de um arquivo de entrada de forma organizada
    void selectFrom(int nroRegObt, FILE *entrada);
    // Realiza m buscas que retornam n registros de acordo com a entrada do usuário
    void selectFromWhere(int nroRegObt, FILE *entrada, int nroBuscas);
#endif