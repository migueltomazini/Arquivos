#include "arquivo.h"

struct cabecalho_ {
    char status;
    long int topo;
    long int prosByteOffset;
    int nroRegArq;
    int nroRegRem;
};

struct registro_ {
    char removido;
    int tamanhoRegistro;
    long int prox;
    int id;
    int idade;
    int tamNomeJog;
    char *nomeJogador;
    int tamNacionalidade;
    char *nacionalidade;
    int tamNomeClube;
    char *nomeClube;
};
