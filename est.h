/* Arquivo com coisa que todos precisamos */
#ifndef _LIVROS_
#define _LIVROS_
#define MAXTIT 100
#define MAXAUT 50
#include <stdio.h>

typedef struct {
    char titulo[MAXTIT + 1];
    int cod;
    char autor[MAXAUT + 1];
} LIVRO;

int imp(LIVRO * l, int n)
{
    int i;
    for (i = 0; i < n; i++) {
        printf("%s\n%d\n%s\n\n", l[i].titulo, l[i].cod, l[i].autor);
    }
}

/* uma função para ler uma string não vazia de um arquivo.
 * Vantagens sobre o fgets: não armazena o ENTER no final e
 * não le string vazia (com 0 cars). Se não conseguiu ler uma 
 * string, retorna ZERO. Se conseguiu retorna o tamanho dela 
 * */
int lestringARQ(char s[], int max, FILE * onde)
{
    int i = 0;
    char letra;
    static int l = 0;
    static FILE *estou = NULL;

    /* Se passar NULL como 'arquivo' retorna a quantidade de linhas
     * que já foram lidas
     * */
    if (onde == NULL)
        return (l);

    /* Resetar o contator de linhas quando receber um outro arquivo */
    if (onde != estou) {
        l = 0;
        estou = onde;
    }
    for (i = 0; i < (max - 1); i++) {
        letra = fgetc(onde);
        if (feof(onde)) {
            s[i] = 0;
            return (i);
        }
        if ((letra == '\n') && (i == 0)) {
            i = i - 1;
            l++;
            continue;
        }

        if (letra == '\n') {
            l++;
            break;
        }

        s[i] = letra;
    }
    s[i] = 0;
    return (i);
}
#endif
