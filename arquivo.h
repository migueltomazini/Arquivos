/* Utilizado na manipulação dos dados, tendo um arquivo .csv como entrada
e um arquivo binário como saída*/

#ifndef ARQUIVO_H
    #define ARQUIVO_H
    
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>

    typedef struct cabecalho_ CABECALHO;
    typedef struct registro_ REGISTRO;

    FILE *inserirDados(char *nomeArquivoCsv);
    
#endif