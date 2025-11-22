#include "est.h"
#include "tempoV2.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define HASH_TABLE_SIZE 1481

// Parece ter muitos conflitos, mas ainda é rápido
unsigned long djbx33a(char *str) {
    unsigned long hash = 5381;
    int c;
    unsigned long maxHashSize;


    // UL é uma forma mais simples de definir o casting
    maxHashSize = (1UL << 63) - 1;
    
    while ((c = *str++)) {
        // aparentemente mais eficiente que hash * 33 + c
        hash = (((hash << 5) + hash) + c)% maxHashSize;
    }
    
    return hash;
}

typedef struct HASH_NODE {
    int index;
    struct HASH_NODE *next;
} HASH_NODE;

typedef struct {
    int inUse;
    int collisionCount;
    HASH_NODE *head;
} HASH_TABLE_ITEM;

int buscaSequencial(LIVRO *vet, int n, char *title) {
    int i;
    for (i = 0; i < n; i++) {
        if (strcmp(vet[i].titulo, title) == 0) {
            return i;
        }
    }
    return -1; // não encontrado
}

int buscaHash(HASH_TABLE_ITEM *hash_table, LIVRO *original, char *title, int qtd) {
    unsigned long hash_key = djbx33a(title) % HASH_TABLE_SIZE;
    
    if (!hash_table[hash_key].inUse) {
        return -1; // não encontrado
    }
    
    HASH_NODE *current = hash_table[hash_key].head;
    while (current != NULL) {
        if (strcmp(original[current->index].titulo, title) == 0) {
            return current->index;
        }
        current = current->next;
    }
    
    return -1; // não encontrado
}

int main(int argc, char *argv[])
{
    FILE *arqDAT;
    LIVRO *original;
    int qtd, i;
    int hightCollision = 0;
    
    if (argc < 2) {
        fprintf(stderr, "ERRO. Precisa passar nome arquivo txt\n");
        return (1);
    }
    if (!strstr(argv[1], ".txt")) {
        fprintf(stderr, "ERRO. Só aceito arquivos com extensão txt\n");
        return (1);
    }

    arqDAT = fopen(argv[1], "r");
    if (arqDAT == NULL) {
        fprintf(stderr, "ERRO. Não pude abrir %s para leitura\n", argv[1]);
        return (2);
    }
    // primeira linha do arquivo deve ter a quantidade
    if (fscanf(arqDAT, "%d", &qtd) != 1) {
        fprintf(stderr, "Não pude ler quantidade\n");
        exit(0);
    }
    // Alocando o vetor de tamanho PRECISO para colocar todos os livros
    // CUIDADO: o vetor para pesquisa HASH deverá ter outro tamanho
    original = malloc(qtd * sizeof(LIVRO));
    if (original == NULL) {
        fprintf(stderr, "Não pude alocar um vetor deste tamanho\n");
        return (1);
    }
    // parte para ler os dados do arquivo. Sim, a construção do vetor
    // original já foi feita por mim. 
    for (i = 0; i < qtd; i++) {
        if (feof(arqDAT)) {
            fprintf(stderr, "Arquivo chegou ao fim sem ter todos os livros. Arquivo bugado?");
            qtd = i;
            break;
        }
        // arquivo tem, na ordem, titulo, cod e autor
        if (lestringARQ(original[i].titulo, MAXTIT, arqDAT) < 1) {
            fprintf(stderr, "Nao tem titulo. Arquivo bugado?");
            qtd = i;
            break;
        }
        if (fscanf(arqDAT, "%d", &original[i].cod) != 1) {
            fprintf(stderr, "Nao tem codigo. Arquivo bugado?");
            return (2);         // encerra
        }
        if (lestringARQ(original[i].autor, MAXAUT, arqDAT) < 1) {
            fprintf(stderr, "Nao tem autor. Arquivo bugado?");
            return (2);
        }
    }
    printf("%d livros no arquivo %s\n", qtd, argv[1]);

    /* Todos os livros do arquivo agora estão no vetor original na ordem em que foram
     * lidos do arquivo. 
     * */
    // debug: impressao so para ver
    // imp(original, qtd);         // função imp está em est.h

    HASH_TABLE_ITEM *hash_table = malloc(qtd * sizeof(HASH_TABLE_ITEM));

    // Inserir cada livro na tabela hash
    // Estou salvando APENAS o índice do livro no vetor original aproveitando que os dados já estão salvos lá
    for (i = 0; i < qtd; i++) {
        unsigned long hash_key = djbx33a(original[i].titulo) % HASH_TABLE_SIZE;
        
        // Se a posição está vazia, criar o primeiro nó
        if (!hash_table[hash_key].inUse) {
            hash_table[hash_key].inUse = 1;
            hash_table[hash_key].head = malloc(sizeof(HASH_NODE));
            hash_table[hash_key].head->index = i;
            hash_table[hash_key].head->next = NULL;
        } else {
            // Colisão: adicionar no início da lista encadeada
            hash_table[hash_key].collisionCount++;
            HASH_NODE *new = malloc(sizeof(HASH_NODE));
            new->index = i;
            new->next = hash_table[hash_key].head;
            hash_table[hash_key].head = new;

            if (hash_table[hash_key].collisionCount > hightCollision) {
                hightCollision = hash_table[hash_key].collisionCount;
            }
        }
    }

    printf("Tabela hash criada com %d como a maior quantidade de colisões\n", hightCollision);
    // Testar as buscas e medir tempo
    printf("\n=== TESTES DE BUSCA ===\n");
    
    // Solicitar título do livro para busca
    char search_title[MAXTIT + 1];
    
    printf("Digite o título do livro: ");
    if (lestringARQ(search_title, MAXTIT, stdin) < 1) {
        fprintf(stderr, "Erro ao ler título\n");
        return (1);
    }
    
    printf("\nBuscando: %s\n\n", search_title);
    
    // 1. Busca Sequencial
    tempo();
    int result = buscaSequencial(original, qtd, search_title);
    ulong t_seq = tempo();
    printf("Busca Sequencial: ");
    if (result != -1) {
        printf("Encontrado na posição %d - Tempo: %s\n", result, formata(t_seq));
    } else {
        printf("Não encontrado - Tempo: %s\n", formata(t_seq));
    }
    
    // 2. Busca por Hash
    tempo();
    result = buscaHash(hash_table, original, search_title, qtd);
    ulong t_hash = tempo();
    printf("Busca por Hash: ");
    if (result != -1) {
        printf("Encontrado na posição %d(Array original) - Tempo: %s\n", result, formata(t_hash));
    } else {
        printf("Não encontrado - Tempo: %s\n", formata(t_hash));
    }
    
    return 0;
}