#ifndef FUNCOESAUXILARES_H
#define FUNCOESAUXILARES_H

#include "funcoes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define TAM_CABECALHO 25

// Função para reinserir o cabeçalho no arquivo
void reinserir_cabecalho(FILE *arquivo, CABECALHO *cabecalho, char status);

// Função para ler o cabeçalho do arquivo
void ler_cabecalho(FILE *arquivo, CABECALHO *cabecalho);

// Função para ler um registro do arquivo
void ler_registro(REGISTRO *registro, FILE *arquivo);

// Função para preencher espaços vazios no arquivo
void preeche_vazio(FILE *arquivo, int tamanho);

// Função para ler lixo do arquivo
int ler_lixo(FILE *file);

// Função para atualizar a lista encadeada de registros removidos
void updateList(FILE *arquivo, long int novo, int tamanhoRegistroNovo);

// Função para atualizar o cabeçalho do arquivo
void updateCabecalho(FILE *arquivo, int nroRemocoes);

// Função para recuperar um registro do arquivo binário
void recuperarRegistro(REGISTRO **registro, FILE *arquivo, int byteOffset, int *maxNomeJog, int *maxNacionalidade, int *maxNomeClube);

// Função auxiliar para abrir e testar o arquivo
int testarArquivo(FILE *arquivo);

// Função para processar os comandos de busca
int comandoBusca(int *nroComandos, char (*comando)[5][20], char (*palavraChave)[5][100]);

// Função de busca nos registros
int busca(REGISTRO *registro, REGISTRO_IND *vetorInd, int nroComandos, char comando[5][20], char palavraChave[5][100]);

// Função para buscar um registro pelo ID
long int buscaId(REGISTRO *registro, REGISTRO_IND *vetorInd, int nroRegistros, int nroComandos, char comando[5][20], char palavraChave[5][100]);

// Função para imprimir os campos de um registro
void impressaoRegistro(REGISTRO *registro);

// Função para recuperar o índice do arquivo
REGISTRO_IND *recoverIndex(FILE *arquivo, FILE *indice);

// Função para criar um novo registro
REGISTRO *criar_registro(int id, int idade, const char *nomeJogador, const char *nacionalidade, const char *clube);

// Função responsável por inserir o registro no arquivo binário
void inserirRegistro(REGISTRO *registro, FILE *arquivo);

// Função auxiliar para alocar memória dinamicamente para o registro
void alocarRegistro(REGISTRO **registro, int maxNomeJog, int maxNacionalidade, int maxNomeClube);

// Função auxiliar responsável por desalocar a memória do registro
void desalocarRegistro(REGISTRO **registro);

void reinserir_cabecalho_ind(FILE *arquivo, CABECALHO_IND *cabecalho, char status);
void ler_cabecalho_ind(FILE *arquivo, CABECALHO_IND *cabecalho);
void insertIntoAux(char *nomeArquivo, long int *byteOffset);
#endif // FUNCOESAUXILARES_H
