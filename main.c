#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 53

typedef struct carro {
    char placa[8];
    char marca[15];
    char modelo[15];
    char cor[15];
    int status; // 1 - ativo ou 0 - removido
} CARRO;

typedef struct noTabela {
    char placa[8];
    int posicao;
    struct noTabela *prox;
} NO;

FILE *prepararArquivo(char nome[]);
void fecharArquivo(FILE *arq);
void criarIndice(FILE *arq, NO *tabelaHashing[]);
void desalocarIndice(NO *tabelaHashing[]);
void exibirOpcoes();
int buscar(NO *tabelaHashing[], char placa[]);
void inserirTabelaHash(NO *tabelaHashing[], char placa[], int pos);
void removerTabelaHash(NO *tabelaHashing[], char placa[], int posTabela);
int hashing(char placa[]);
void cadastrar(FILE *arq, NO *tabelaHashing[]);
void consultar(FILE *arq, NO *tabelaHashing[]);
void alterar(FILE *arq, NO *tabelaHashing[]);
void remover(FILE *arq, NO *tabelaHashing[]);
void exibirCadastro(FILE *arq);
void limparBuffer();

int main() {
    char nomeArq[] = "carros.dat";
    int op;
    FILE *cadastro;
    NO *tabelaHashing[N] = {NULL}; // Inicializar a tabela hash com NULLs
    cadastro = prepararArquivo(nomeArq);
    if (cadastro == NULL)
        printf("Erro na abertura do arquivo. Programa encerrado \n");
    else {
        criarIndice(cadastro, tabelaHashing);
        do {
            exibirOpcoes();
            scanf("%d", &op);
            limparBuffer(); // Limpar buffer após leitura do menu
            switch (op) {
            case 1:
                cadastrar(cadastro, tabelaHashing);
                break;
            case 2:
                consultar(cadastro, tabelaHashing);
                break;
            case 3:
                alterar(cadastro, tabelaHashing);
                break;
            case 4:
                remover(cadastro, tabelaHashing);
                break;
            case 5:
                exibirCadastro(cadastro);
                break;
            case 0:
                fecharArquivo(cadastro);
                desalocarIndice(tabelaHashing);
                break;
            default:
                printf("Opcao invalida \n");
            }
        } while (op != 0);
    }
    return 0;
}

FILE *prepararArquivo(char nome[]) {
    FILE *aux;
    aux = fopen(nome, "r+b");
    if (aux == NULL)
        aux = fopen(nome, "w+b");
    return aux;
}

void fecharArquivo(FILE *arq) {
    fclose(arq);
}

void criarIndice(FILE *arq, NO *tabelaHashing[]) {
    CARRO c;
    int pos = 0;
    fseek(arq, 0, SEEK_SET);
    while (fread(&c, sizeof(CARRO), 1, arq) == 1) {
        if (c.status == 1) {
            inserirTabelaHash(tabelaHashing, c.placa, pos);
        }
        pos++;
    }
}

void desalocarIndice(NO *tabelaHashing[]) {
    for (int i = 0; i < N; i++) {
        NO *temp = tabelaHashing[i];
        while (temp != NULL) {
            NO *aux = temp;
            temp = temp->prox;
            free(aux);
        }
    }
}

void exibirOpcoes() {
    printf("Opções \n");
    printf("1 - Cadastrar um carro \n");
    printf("2 - Consultar carro \n");
    printf("3 - Alterar dados do carro \n");
    printf("4 - Remover carro \n");
    printf("5 - Exibir carros cadastrados \n");
    printf("0 - Encerrar programa \n");
    printf("Informe a opcao: ");
}

int buscar(NO *tabelaHashing[], char placa[]) {
    int pos = hashing(placa);
    NO *temp = tabelaHashing[pos];
    while (temp != NULL) {
        if (strcmp(temp->placa, placa) == 0)
            return temp->posicao;
        temp = temp->prox;
    }
    return -1;
}

void inserirTabelaHash(NO *tabelaHashing[], char placa[], int pos) {
    int h = hashing(placa);
    NO *novo = (NO *)malloc(sizeof(NO));
    strcpy(novo->placa, placa);
    novo->posicao = pos;
    novo->prox = tabelaHashing[h];
    tabelaHashing[h] = novo;
}

void removerTabelaHash(NO *tabelaHashing[], char placa[], int posTabela) {
    int pos = hashing(placa);
    NO *prev = NULL;
    NO *temp = tabelaHashing[pos];
    while (temp != NULL) {
        if (strcmp(temp->placa, placa) == 0) {
            if (prev == NULL)
                tabelaHashing[pos] = temp->prox;
            else
                prev->prox = temp->prox;
            free(temp);
            break;
        }
        prev = temp;
        temp = temp->prox;
    }
}

int hashing(char placa[]) {
    int h = 0;
    for (int i = 0; placa[i] != '\0'; i++) {
        h = (h * 256 + placa[i]) % N;
    }
    return h;
}

void cadastrar(FILE *arq, NO *tabelaHashing[]) {
    CARRO c;
    printf("Informe a placa do carro: ");
    scanf("%s", c.placa);
    limparBuffer();
    if (buscar(tabelaHashing, c.placa) != -1) {
        printf("Carro já cadastrado.\n");
        return;
    }
    printf("Informe a marca do carro: ");
    scanf("%s", c.marca);
    limparBuffer();
    printf("Informe o modelo do carro: ");
    scanf("%s", c.modelo);
    limparBuffer();
    printf("Informe a cor do carro: ");
    scanf("%s", c.cor);
    limparBuffer();
    c.status = 1;
    fseek(arq, 0, SEEK_END);
    fwrite(&c, sizeof(CARRO), 1, arq);
    inserirTabelaHash(tabelaHashing, c.placa, ftell(arq) / sizeof(CARRO));
    printf("Carro cadastrado com sucesso.\n");
}

void consultar(FILE *arq, NO *tabelaHashing[]) {
    char placa[8];
    printf("Informe a placa do carro a ser consultado: ");
    scanf("%s", placa);
    limparBuffer();
    int pos = buscar(tabelaHashing, placa);
    if (pos != -1) {
        fseek(arq, pos * sizeof(CARRO), SEEK_SET);
        CARRO c;
        fread(&c, sizeof(CARRO), 1, arq);
        printf("Placa: %s\n", c.placa);
        printf("Marca: %s\n", c.marca);
        printf("Modelo: %s\n", c.modelo);
        printf("Cor: %s\n", c.cor);
        printf("Status: %s\n", c.status ? "Ativo" : "Removido");
    } else {
        printf("Carro não encontrado.\n");
    }
}

void alterar(FILE *arq, NO *tabelaHashing[]) {
    char placa[8];
    printf("Informe a placa do carro a ser alterado: ");
    scanf("%s", placa);
    limparBuffer();
    int pos = buscar(tabelaHashing, placa);
    if (pos != -1) {
        fseek(arq, pos * sizeof(CARRO), SEEK_SET);
        CARRO c;
        fread(&c, sizeof(CARRO), 1, arq);
        printf("Dados atuais:\n");
        printf("Marca: %s\n", c.marca);
        printf("Modelo: %s\n", c.modelo);
        printf("Cor: %s\n", c.cor);
        printf("Informe a nova marca do carro: ");
        scanf("%s", c.marca);
        limparBuffer();
        printf("Informe o novo modelo do carro: ");
        scanf("%s", c.modelo);
        limparBuffer();
        printf("Informe a nova cor do carro: ");
        scanf("%s", c.cor);
        limparBuffer();
        fseek(arq, -sizeof(CARRO), SEEK_CUR);
        fwrite(&c, sizeof(CARRO), 1, arq);
        printf("Carro alterado com sucesso.\n");
    } else {
        printf("Carro não encontrado.\n");
    }
}

void remover(FILE *arq, NO *tabelaHashing[]) {
    char placa[8];
    printf("Informe a placa do carro a ser removido: ");
    scanf("%s", placa);
    limparBuffer();
    int pos = buscar(tabelaHashing, placa);
    if (pos != -1) {
        fseek(arq, pos * sizeof(CARRO), SEEK_SET);
        CARRO c;
        fread(&c, sizeof(CARRO), 1, arq);
        printf("Dados do carro:\n");
        printf("Placa: %s\n", c.placa);
        printf("Marca: %s\n", c.marca);
        printf("Modelo: %s\n", c.modelo);
        printf("Cor: %s\n", c.cor);
        printf("Deseja realmente remover este carro? (S/N): ");
        char resposta;
        scanf(" %c", &resposta);
        limparBuffer();
        if (resposta == 'S' || resposta == 's') {
            c.status = 0;
            fseek(arq, -sizeof(CARRO), SEEK_CUR);
            fwrite(&c, sizeof(CARRO), 1, arq);
            removerTabelaHash(tabelaHashing, placa, pos);
            printf("Carro removido com sucesso.\n");
        } else {
            printf("Operação cancelada.\n");
        }
    } else {
        printf("Carro não encontrado.\n");
    }
}

void exibirCadastro(FILE *arq) {
    fseek(arq, 0, SEEK_SET);
    CARRO c;
    printf("Carros cadastrados:\n");
    while (fread(&c, sizeof(CARRO), 1, arq) == 1) {
        printf("Placa: %s\n", c.placa);
        printf("Marca: %s\n", c.marca);
        printf("Modelo: %s\n", c.modelo);
        printf("Cor: %s\n", c.cor);
        printf("Status: %s\n", c.status ? "Ativo" : "Removido");
        printf("\n");
    }
}

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // Consome todos os caracteres até encontrar uma nova linha ou fim de arquivo
    }
}
