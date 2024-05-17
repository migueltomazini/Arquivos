#include "funcoes.h"

int testarArquivo(FILE *arquivo, char *nomeArquivo) {
    char status;
    fread(&status, sizeof(char), 1, arquivo);
    // Verifica se o arquivo ou a leitura do status falharam
    if (arquivo == NULL || status =='0') {
        printf("Falha no processamento do arquivo.\n");
        return 1; // Retorna 1 se a abertura ou a leitura do status falharam
    }

    return 0; // Retorna 0 se a abertura e a leitura do status foram bem-sucedidas
}



void createIndex(char *nomeArquivo, char *nomeIndice) {
    
    FILE* ptrArquivo = fopen(nomeArquivo, "rb");
    if (ptrArquivo == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return;
    }
   
    char status;
    fread(&status, sizeof(char), 1, ptrArquivo);
    if (testarArquivo == 1)
    {
        printf("Falha no processamento do arquivo.");
        return;
    }
    

    FILE* ptrIndice = fwrite(nomeArquivo, 1 ,nomeIndice, "wb");
    if (ptrIndice == NULL)
    {
        printf("Falha no processamento do arquivo");
        fclose(ptrArquivo);
        return;
    }
  
    
    
    
    
}
