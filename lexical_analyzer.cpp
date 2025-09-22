#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

/* ----------
   Scanner “char a char” com DFAs por token:

   getIdent:
     q0 --(letra|_)--> q1
     q1 --(letra|digito|_)--> q1
     q1 --(outro)--> aceita, devolve 1 char (ungetc) e retorna IDENT

   getOutro:
     q0 --(nao-branco e nao-letra)--> aceita (consome 1 char e retorna OUTRO)
     (nota: dígitos entram como OUTRO)
   ---------- */

static int nextc(FILE *in) {
    return fgetc(in);
}

static void putback(int c, FILE *in) {
    if (c != EOF) ungetc(c, in);
}

/* Tenta reconhecer um IDENT a partir da posição atual.
   Sucesso: consome todo o lexema e retorna 1.
   Falha: não consome nada relevante (recoloca o primeiro char) e retorna 0. */
static int getIdent(FILE *in) {
    int state = 0;
    int c = nextc(in);

    switch (state) {
        case 0:
            if (isalpha(c) || c == '_') {
                state = 1;
            } else {
                putback(c, in);
                return 0; // fail at q0
            }
            break;
    }

    // state 1: loop enquanto for válido para IDENT
    while (1) {
        c = nextc(in);
        if (isalnum(c) || c == '_') {
            // permanece em state 1
            continue;
        } else {
            // estado de aceitação: devolve o lookahead
            putback(c, in);
            return 1;
        }
    }
}

/* Tenta reconhecer OUTRO: um único caractere que não é espaço/tab/nl
   e não inicia IDENT (i.e., não é letra/underscore). Dígitos viram OUTRO. */
static int getOutro(FILE *in) {
    int state = 0;
    int c = nextc(in);

    switch (state) {
        case 0:
            if (c == EOF) return 0;
            if (!isspace(c) && !(isalpha(c) || c == '_')) {
                // aceita consumindo um único caractere
                return 1;
            } else {
                putback(c, in);
                return 0; // fail
            }
    }
    return 0;
}

int main(void) {
    int c;
    bool start_of_line = true;

    while ((c = fgetc(stdin)) != EOF) {
        // Trata quebras de linha: preserva linhas de saída (não é token)
        if (c == '\n') {
            putchar('\n');
            start_of_line = true;
            continue;
        }

        // Ignora brancos (exceto '\n' já tratado)
        if (c == ' ' || c == '\t' || c == '\r' || c == '\f' || c == '\v') {
            continue;
        }

        // Precisamos tentar tokens a partir deste caractere: devolve ao fluxo
        ungetc(c, stdin);

        // Tenta IDENT pela DFA correspondente
        if (getIdent(stdin)) {
            if (!start_of_line) putchar(' ');
            fputs("IDENT", stdout);
            start_of_line = false;
            continue;
        }

        // Tenta OUTRO (caractere único)
        if (getOutro(stdin)) {
            if (!start_of_line) putchar(' ');
            fputs("OUTRO", stdout);
            start_of_line = false;
            continue;
        }

        // Se chegou aqui, consumimos um char “problemático” para não travar.
        // (Opcionalmente, poderia sinalizar erro.)
        (void)fgetc(stdin);
    }

    // Garante newline final opcional
    if (!start_of_line) putchar('\n');
    return 0;
}
