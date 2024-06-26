#include "funcoes.h" // Inclui o cabeçalho com as funções principais.
#include "funcoesAuxiliares.h" // Inclui o cabeçalho com as funções auxiliares.

void insertbtree(char *nomeArquivo, char *nomeIndice, int nroAdicoes) {
    // Abrir o arquivo de índice no modo de leitura e escrita binária.
    FILE *arquivoIndice = fopen(nomeIndice, "rb+");
    if (arquivoIndice == NULL) {
        // Caso o arquivo de índice não consiga ser aberto, imprime mensagem de erro e encerra o programa.
        printf("Falha no processamento do arquivo.\n");
        exit(0);
    }
    
    // Abrir o arquivo de dados no modo de leitura e escrita binária.
    FILE *arquivo = fopen(nomeArquivo, "rb+");
    if (arquivo == NULL) {
        // Caso o arquivo de dados não consiga ser aberto, imprime mensagem de erro, fecha o arquivo de índice e encerra o programa.
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivoIndice);
        exit(0);
    }

    // Variáveis para armazenar registros e nós da árvore-B.
    REGISTRO registro; // Estrutura para armazenar um registro.
    BTREE arvoreB; // Estrutura para armazenar a árvore-B.
    NO_BTREE novoNo; // Estrutura para armazenar um novo nó da árvore-B.

    // Inicializa o novo nó da árvore-B.
    criarNo(&novoNo);  // Passa o endereço do novo nó para a função de criação.

    // Loop para inserir múltiplos registros.
    for (int i = 0; i < nroAdicoes; i++) {
        // Variável para armazenar o byte offset do registro.
        long int byteOffset;

        // Inserir o registro no arquivo de dados usando a função auxiliar.
        insertIntoAux(nomeArquivo, &byteOffset);

        // Posicionar o ponteiro do arquivo na posição do byte offset para ler o registro.
        fseek(arquivo, byteOffset, SEEK_SET);
        ler_registro(&registro, arquivo); // Lê o registro do arquivo de dados.

        // Inserir a chave (ID do registro) no arquivo de índice árvore-B.
        inserirChave(&arvoreB, nomeIndice, registro.id, byteOffset);
    }

    //não foi necessário fechar o arquivo por que nas função auxiliar ao finalizar a inserção ele é fechado 
    //e caso ocorra qualquer queda durante a função ela esta preparada para fechar o arquivo, 
    //da mesma forma ocorreu para verificações e mudaças no cabeçalho.
}
