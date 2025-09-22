# g++ simbol_table.cpp -o simbol_table 
# ./simbol_table << 'EOF'  
# 1. def f (int x) {
# 2.   if x < 0
# 3.      x = -x;
# 4.   return;
# 5. }
# EOF

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
using namespace std;

/* ---- Leitura char-a-char ---- */
static int nextc(istream &in) {
    return in.get();
}
static void putback(int c, istream &in) {
    if (c != EOF) in.putback(static_cast<char>(c));
}

/* ---- DFA para IDENT ---- */
static bool getIdent(istream &in, string &lexeme) {
    lexeme.clear();
    int c = nextc(in);
    if (c == EOF) return false;

    if (isalpha(c) || c == '_') {
        lexeme.push_back(static_cast<char>(c));
    } else {
        putback(c, in);
        return false; // falha no q0
    }

    while (true) {
        c = nextc(in);
        if (isalnum(c) || c == '_') {
            lexeme.push_back(static_cast<char>(c));
        } else {
            putback(c, in);
            return true; // aceita
        }
    }
}

/* ---- Programa principal ---- */
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<string> order;                           // ordem de primeira ocorrência
    unordered_map<string, vector<int>> occ;         // ident -> linhas
    occ.reserve(256);

    int line = 1;
    while (true) {
        int c = nextc(cin);
        if (c == EOF) break;

        if (c == '\n') { line++; continue; }
        if (isspace(c)) continue; // ignora branco (menos newline já tratado)

        putback(c, cin);
        string ident;
        if (getIdent(cin, ident)) {
            if (!occ.count(ident)) {
                order.push_back(ident);
                occ[ident] = {};
            }
            auto &v = occ[ident];
            if (v.empty() || v.back() != line) v.push_back(line);
            continue;
        }
        (void) nextc(cin); // consome OUTRO
    }

    // imprime tabela
    for (const auto &id : order) {
        cout << id << '\t' << '[';
        const auto &v = occ[id];
        for (size_t i = 0; i < v.size(); ++i) {
            if (i) cout << ", ";
            cout << v[i];
        }
        cout << "]\n";
    }
    return 0;
}
