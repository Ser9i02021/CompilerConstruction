# g++ lexical_analyzer.cpp -o lexical_analyzer
# /usr/bin/env ./lexical_analyzer
# def f (int a) {
# x = x + a;
# return;
# }

#include <iostream>
#include <cctype>
#include <string>
#include <limits>

using std::cin;
using std::cout;
using std::endl;

// Utilitários para ler/devolver caractere
static inline int nextc(std::istream& in) {
    return in.get(); // retorna EOF (traits::eof) em fim
}

static inline void putback(int c, std::istream& in) {
    if (c != std::char_traits<char>::eof())
        in.putback(static_cast<char>(c));
}

// DFA: IDENT = letra|_ ( letra|digito|_ )*
static bool getIdent(std::istream& in) {
    int state = 0;
    int c = nextc(in);

    // q0
    if (c == std::char_traits<char>::eof()) return false;
    if (std::isalpha((unsigned char)c) || c == '_') {
        state = 1;
    } else {
        putback(c, in);
        return false; // falha em q0
    }

    // q1: loop
    while (true) {
        c = nextc(in);
        if (c != std::char_traits<char>::eof()
            && (std::isalnum((unsigned char)c) || c == '_')) {
            continue; // permanece em q1
        } else {
            // aceita: devolve lookahead
            putback(c, in);
            return true;
        }
    }
}

// DFA: OUTRO = um único caractere que não é branco e não inicia IDENT
// (i.e., não é letra nem '_'). Dígitos contam como OUTRO.
static bool getOutro(std::istream& in) {
    int state = 0;
    int c = nextc(in);
    if (c == std::char_traits<char>::eof()) return false;

    // q0
    if (!std::isspace((unsigned char)c) && !(std::isalpha((unsigned char)c) || c == '_')) {
        // aceita consumindo UM caractere
        return true;
    } else {
        putback(c, in);
        return false;
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int c;
    bool start_of_line = true;

    while (true) {
        c = cin.get();
        if (c == std::char_traits<char>::eof()) break;

        if (c == '\n') {
            cout << '\n';
            start_of_line = true;
            continue;
        }

        // BRANCO (exceto newline já tratado) é ignorado
        if (c == ' ' || c == '\t' || c == '\r' || c == '\f' || c == '\v') {
            continue;
        }

        // precisamos tentar tokens a partir deste caractere: devolve ao fluxo
        cin.putback(static_cast<char>(c));

        if (getIdent(cin)) {
            if (!start_of_line) cout << ' ';
            cout << "IDENT";
            start_of_line = false;
            continue;
        }

        if (getOutro(cin)) {
            if (!start_of_line) cout << ' ';
            cout << "OUTRO";
            start_of_line = false;
            continue;
        }

        // fallback: consome 1 caractere para evitar loop infinito
        cin.get();
    }

    if (!start_of_line) cout << '\n';
    return 0;
}
