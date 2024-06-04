#include "funcoes.h" // Inclui o cabeçalho com as funções principais
#include "funcoesAuxiliares.h" // Inclui o cabeçalho com as funções auxiliares

// Função auxiliar para inserir um registro no arquivo
void insertIntoAux(char *nomeArquivo) {
    int id, idade; 
    char nomeJogador[30], nacionalidade[30], nomeClube[30], auxIdade[4];

    // Lê os dados do registro a partir da entrada padrão
    scanf("%d", &id);
    scan_quote_string(auxIdade);
    scan_quote_string(nomeJogador);
    scan_quote_string(nacionalidade);
    scan_quote_string(nomeClube);
    idade = atoi(auxIdade); // Converte a idade de string para inteiro
    if (idade == 0) { // Se a idade for 0, define como -1 (indefinida)
        idade = -1;
    }

    // Abre o arquivo binário para leitura e escrita
    FILE* nomearq = fopen(nomeArquivo, "rb+");
    if (nomearq == NULL) { // Verifica se a abertura do arquivo falhou
        printf("Falha no processamento do arquivo\n");
        exit(1);
    }

    CABECALHO cabecalho; // Declaração do cabeçalho do arquivo
    ler_cabecalho(nomearq, &cabecalho); // Lê o cabeçalho do arquivo

    if (cabecalho.status == '0') { // Verifica se o status do cabeçalho é inválido
        printf("Falha no processamento do arquivo\n");
        fclose(nomearq); // Fecha o arquivo
        exit(1);
    }

    reinserir_cabecalho(nomearq, &cabecalho, '0'); // Atualiza o status do cabeçalho para '0' (inválido)

    REGISTRO* registro = criar_registro(id, idade, nomeJogador, nacionalidade, nomeClube); // Cria um novo registro
    if (cabecalho.topo == -1) { // Se não há registros removidos
        fseek(nomearq, 0, SEEK_END); // Posiciona o ponteiro no final do arquivo
        inserirRegistro(registro, nomearq); // Insere o novo registro no final do arquivo
        cabecalho.prosByteOffset += registro->tamanhoRegistro; // Atualiza o próximo byte offset
        cabecalho.nroRegArq++; // Incrementa o número de registros
        desalocarRegistro(&registro); // Desaloca a memória do registro
        reinserir_cabecalho(nomearq, &cabecalho, '1'); // Atualiza o cabeçalho com o status '1' (válido)
        fclose(nomearq); // Fecha o arquivo
        return;
    }

    REGISTRO* anterior = (REGISTRO*) malloc(sizeof(REGISTRO)); // Aloca memória para o registro anterior
    if (!anterior) { // Verifica se a alocação de memória falhou
        printf("Falha no processamento do arquivo.\n");
        exit(1);
    }
    fseek(nomearq, cabecalho.topo, SEEK_SET); // Posiciona o ponteiro no topo da lista de registros removidos
    ler_registro(anterior, nomearq); // Lê o registro anterior
    ler_lixo(nomearq); // Lê os espaços vazios após o registro anterior

    if (anterior->prox == -1) { // Se só existe um registro removido
        if (registro->tamanhoRegistro <= anterior->tamanhoRegistro) { // Se o tamanho do novo registro é menor ou igual ao registro removido
            fseek(nomearq, (-1) * anterior->tamanhoRegistro, SEEK_CUR); // Posiciona o ponteiro no início do registro removido
            int tamaux = registro->tamanhoRegistro;
            registro->tamanhoRegistro = anterior->tamanhoRegistro; // Atualiza o tamanho do novo registro para ocupar o espaço do registro removido
            inserirRegistro(registro, nomearq); // Insere o novo registro no lugar do registro removido
            preeche_vazio(nomearq, anterior->tamanhoRegistro - tamaux); // Preenche o espaço vazio, se houver
            cabecalho.topo = -1; // Atualiza o topo para indicar que não há mais registros removidos
            cabecalho.nroRegRem--; // Decrementa o número de registros removidos
            cabecalho.nroRegArq++; // Incrementa o número de registros
            reinserir_cabecalho(nomearq, &cabecalho, '1'); // Atualiza o cabeçalho com o status '1' (válido)
            desalocarRegistro(&anterior); // Desaloca a memória do registro anterior
            desalocarRegistro(&registro); // Desaloca a memória do novo registro
            fclose(nomearq); // Fecha o arquivo
            return;
        } else { // Se o tamanho do novo registro é maior que o registro removido
            fseek(nomearq, 0, SEEK_END); // Posiciona o ponteiro no final do arquivo
            inserirRegistro(registro, nomearq); // Insere o novo registro no final do arquivo
            cabecalho.prosByteOffset += registro->tamanhoRegistro; // Atualiza o próximo byte offset
            cabecalho.nroRegArq++; // Incrementa o número de registros
            desalocarRegistro(&registro); // Desaloca a memória do novo registro
            reinserir_cabecalho(nomearq, &cabecalho, '1'); // Atualiza o cabeçalho com o status '1' (válido)
            fclose(nomearq); // Fecha o arquivo
            return;
        }
    } else if (registro->tamanhoRegistro <= anterior->tamanhoRegistro) { // Se o novo registro deve ser inserido no topo
        fseek(nomearq, (-1) * anterior->tamanhoRegistro, SEEK_CUR); // Posiciona o ponteiro no início do registro removido
        int tamaux = registro->tamanhoRegistro;
        registro->tamanhoRegistro = anterior->tamanhoRegistro; // Atualiza o tamanho do novo registro para ocupar o espaço do registro removido
        inserirRegistro(registro, nomearq); // Insere o novo registro no lugar do registro removido
        preeche_vazio(nomearq, anterior->tamanhoRegistro - tamaux); // Preenche o espaço vazio, se houver
        cabecalho.topo = anterior->prox; // Atualiza o topo para o próximo registro removido
        cabecalho.nroRegRem--; // Decrementa o número de registros removidos
        cabecalho.nroRegArq++; // Incrementa o número de registros
        reinserir_cabecalho(nomearq, &cabecalho, '1'); // Atualiza o cabeçalho com o status '1' (válido)
        desalocarRegistro(&anterior); // Desaloca a memória do registro anterior
        desalocarRegistro(&registro); // Desaloca a memória do novo registro
        fclose(nomearq); // Fecha o arquivo
        return;
    }

    long int aux_anterior = cabecalho.topo; // Byte offset do registro anterior ao atual
    long int aux_atual = anterior->prox; // Byte offset do próximo registro a ser processado
    while (aux_atual != -1) { // Percorre a lista de registros removidos
        fseek(nomearq, aux_atual, SEEK_SET); // Posiciona o ponteiro no próximo registro removido
        REGISTRO* atual = (REGISTRO*) malloc(sizeof(REGISTRO)); // Aloca memória para o registro atual
        ler_registro(atual, nomearq); // Lê o registro atual
        ler_lixo(nomearq); // Lê os espaços vazios após o registro atual

        if (registro->tamanhoRegistro <= atual->tamanhoRegistro) { // Se o novo registro deve ser inserido no lugar do registro atual
            anterior->prox = atual->prox; // Atualiza o ponteiro do registro anterior para pular o registro atual
            fseek(nomearq, (-1) * atual->tamanhoRegistro, SEEK_CUR); // Posiciona o ponteiro no início do registro atual
            int tamaux = registro->tamanhoRegistro;
            registro->tamanhoRegistro = atual->tamanhoRegistro; // Atualiza o tamanho do novo registro para ocupar o espaço do registro atual
            inserirRegistro(registro, nomearq); // Insere o novo registro no lugar do registro atual
            preeche_vazio(nomearq, atual->tamanhoRegistro - tamaux); // Preenche o espaço vazio, se houver
            cabecalho.nroRegRem--; // Decrementa o número de registros removidos
            cabecalho.nroRegArq++; // Incrementa o número de registros
            fseek(nomearq, aux_anterior, SEEK_SET); // Posiciona o ponteiro no registro anterior
            inserirRegistro(anterior, nomearq); // Atualiza o registro anterior no arquivo
            reinserir_cabecalho(nomearq, &cabecalho, '1'); // Atualiza o cabeçalho com o status '1' (válido)
            desalocarRegistro(&atual); // Desaloca a memória do registro atual
            desalocarRegistro(&anterior); // Desaloca a memória do registro anterior
            desalocarRegistro(&registro); // Desaloca a memória do novo registro
            fclose(nomearq); // Fecha o arquivo
            return;
        }

        anterior = atual; // Atualiza o registro anterior para o próximo
        aux_anterior = aux_atual; // Atualiza o byte offset do registro anterior
        aux_atual = atual->prox; // Atualiza o byte offset do próximo registro a ser processado
    }

    // Insere o novo registro no final do arquivo
    fseek(nomearq, 0, SEEK_END);
    inserirRegistro(registro, nomearq);
    cabecalho.nroRegArq++; // Incrementa o número de registros
    cabecalho.prosByteOffset += registro->tamanhoRegistro; // Atualiza o próximo byte offset
    reinserir_cabecalho(nomearq, &cabecalho, '1'); // Atualiza o cabeçalho com o status '1' (válido)
    desalocarRegistro(&registro); // Desaloca a memória do novo registro
    fclose(nomearq); // Fecha o arquivo
}

// Função principal para inserir múltiplos registros no arquivo
void insertInto(char *nomeArquivo, char *nomeIndice, int nroAdicoes) {
    for (int i = 0; i < nroAdicoes; i++) {
        insertIntoAux(nomeArquivo); // Insere cada registro utilizando a função auxiliar
    }
    createIndex(nomeArquivo, nomeIndice); // Recria o índice após as inserções
}



