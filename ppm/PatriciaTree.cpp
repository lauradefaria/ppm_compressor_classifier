#include "PatriciaTree.hpp"
#include "../src/Config.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <fstream>
#include <sstream>

PatriciaTree::PatriciaTree() {
    // cria nó vazio como raiz
    raiz = new PatriciaNode(); 
}

PatriciaTree::~PatriciaTree() {
    liberarMemoria(raiz);
}

void PatriciaTree::liberarMemoria(PatriciaNode* no) {
    // percorre todos os filhos e desaloca memória dos nós
    if (!no) return;
    for (auto& kv : no->filhos)
        liberarMemoria(kv.second);
    delete no;
}

void PatriciaTree::inserirContexto(const std::string& contexto, unsigned char simbolo) {
    PatriciaNode* atual = raiz;
    std::string restante = contexto; // contexto ainda não "consumido"

    while (!restante.empty()) {
        unsigned char c = static_cast<unsigned char>(restante[0]);

        if (atual->filhos.count(c) == 0) { // caso não exista filho iniciado com c

            // um novo nó é criado
            PatriciaNode* novo = new PatriciaNode(restante);
            atual->filhos[c] = novo;
            atual = novo;
            restante.clear();

        } else {  // caso exista 

            PatriciaNode* prox = atual->filhos[c];
            const std::string& pref = prox->prefixo;

            size_t i = 0;
            while (i < pref.size() && i < restante.size() && pref[i] == restante[i])
                i++;

            if (i == pref.size()) {
                atual = prox;
                restante = restante.substr(i);
            } else {
                PatriciaNode* mid = new PatriciaNode(pref.substr(0, i));
                atual->filhos[c] = mid;

                prox->prefixo = pref.substr(i);
                mid->filhos[static_cast<unsigned char>(prox->prefixo[0])] = prox;

                if (i < restante.size()) {
                    PatriciaNode* novoFilho = new PatriciaNode(restante.substr(i));
                    mid->filhos[static_cast<unsigned char>(novoFilho->prefixo[0])] = novoFilho;
                    atual = novoFilho;
                    restante.clear();
                } else {
                    atual = mid;
                    restante.clear();
                }
            }

        }
    }

    // atualiza frequência do símbolo
    if (atual->freq[simbolo] == 0)
        atual->distintos++;
    atual->freq[simbolo]++;

}

PatriciaNode* PatriciaTree::buscarContexto(const std::string& contexto) const {
    PatriciaNode* atual = raiz;
    std::string restante = contexto;

    while (!restante.empty()) {
        unsigned char c = static_cast<unsigned char>(restante[0]);

        if (atual->filhos.count(c) == 0) {
            return nullptr; // contexto não encontrado
        }

        PatriciaNode* prox = atual->filhos[c];
        const std::string& pref = prox->prefixo;

        size_t i = 0;
        while (i < pref.size() && i < restante.size() && pref[i] == restante[i])
            i++;

        if (i == pref.size()) {
            atual = prox;
            restante = restante.substr(i);
        } else {
            // prefixo não coincide completamente
            if (i < restante.size()) {
                return nullptr; // contexto não encontrado
            } else {
                atual = prox;
                restante.clear();
            }
        }
    }

    return atual;
}

// converte um byte para um símbolo imprimível
std::string PatriciaTree::renderSimbolo(int code) {
    std::ostringstream oss;
    unsigned char c = static_cast<unsigned char>(code);
    oss << std::setw(3) << code << "('";
    if (std::isprint(c)) oss << c; else oss << '.';
    oss << "')";
    return oss.str();
}

void PatriciaTree::cabecalhoKmenos1() {
    std::cout << "K=-1: equiprobabilidade implicita para simbolos nao vistos\n";
    std::cout << "==========================================================================\n";
}

void PatriciaTree::imprimirNo(PatriciaNode* no, const std::string& contextoAcum) const {
    if (!no) return;

    std::string ctx = contextoAcum + no->prefixo;
    int ordem = static_cast<int>(ctx.size());

    uint64_t soma = 0;
    for (int s = 0; s < ALFABETO; s++)
        soma += no->freq[s];

    if (soma == 0 && no->filhos.empty())
        return;

    // ESC só é necessário se há símbolos não vistos
    uint64_t esc = 0;
    if (no->distintos < ALFABETO) {
        esc = no->distintos; // ESC = número de símbolos distintos vistos
    }
    uint64_t total = soma + esc;

    std::cout << "Contexto (K=" << ordem << "): [" << ctx << "]\n";
    std::cout << "  distintos=" << no->distintos
              << "  somaSimbolos=" << soma
              << "  ESC=" << esc
              << "  total=" << total << "\n";

    std::cout << "  K  | CONTEXTO |     SIMBOLO     | CONTAGEM | PROB\n";

    if (esc > 0) {
        double pEsc = static_cast<double>(esc) / static_cast<double>(total);
        std::cout << "  " << std::setw(2) << ordem << " | "
                  << std::setw(8) << ctx << " | "
                  << std::setw(15) << "ESC" << " | "
                  << std::setw(8) << esc << " | "
                  << std::fixed << std::setprecision(6) << pEsc << "\n";
    }

    for (int s = 0; s < ALFABETO; s++) {
        uint32_t c = no->freq[s];
        if (c == 0) continue;

        double p = (total > 0) ? static_cast<double>(c) / static_cast<double>(total) : 0.0;
        std::cout << "  " << std::setw(2) << ordem << " | "
                  << std::setw(8) << ctx << " | "
                  << std::setw(15) << renderSimbolo(s) << " | "
                  << std::setw(8) << c << " | "
                  << std::fixed << std::setprecision(6) << p << "\n";
    }

    std::cout << "--------------------------------------------------------------------------\n";

    for (const auto& kv : no->filhos)
        imprimirNo(kv.second, ctx);
}

void PatriciaTree::imprimirTabela() const {
    cabecalhoKmenos1();
    imprimirNo(raiz, "");
}

void PatriciaTree::salvarArvore(const std::string& nomeArquivo) const {

    std::ofstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir arquivo " << nomeArquivo << std::endl;
        return;
    }

    // Formatação: prefixo | simbolo | freq[1] |...| freq[255] | num_filhos | filhos..."

    salvarNo(arquivo, raiz);
    arquivo.close();
}

void PatriciaTree::salvarNo(std::ofstream& file, PatriciaNode* no) const {
    if (!no) return;

    // prefixo (escapado para evitar problemas com caracteres especiais)
    std::string prefixo_escaped = no->prefixo;
    // Substitui caracteres problemáticos
    for (char& c : prefixo_escaped) {
        if (c == '|' || c == '\n' || c == '\r') {
            c = '_';
        }
    }
    file << prefixo_escaped << "|";

    // distintos
    file << no->distintos << "|";

    // frequências
    for (int i = 0; i < ALFABETO; i++) {
        file << no->freq[i] << "|";
    }

    // número de filhos
    file << no->filhos.size() << "|";

    // chaves dos filhos
    for (auto& kv : no->filhos) {
        file << static_cast<int>(kv.first) << "|";
    }

    file << "\n"; // nó termina aqui

    // grava recursivamente os filhos
    for (auto& kv : no->filhos) {
        salvarNo(file, kv.second);
    }
}

void PatriciaTree::carregarArvore(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir arquivo para leitura: " << filename << std::endl;
        return;
    }
    
    // libera árvore atual se existir
    if (raiz) {
        liberarMemoria(raiz);
    }
    
    // carrega a raiz
    raiz = carregarNo(file);
    file.close();
    std::cout << "Arvore Patricia carregada de: " << filename << std::endl;
}

PatriciaNode* PatriciaTree::carregarNo(std::ifstream& file) {
    std::string line;
    if (!std::getline(file, line)) return nullptr;

    std::istringstream iss(line);
    std::string token;

    PatriciaNode* no = new PatriciaNode();

    // prefixo
    if (std::getline(iss, token, '|')) {
        no->prefixo = token;
    }

    // distintos
    if (std::getline(iss, token, '|') && !token.empty()) {
        try {
            no->distintos = std::stoi(token);
        } catch (const std::exception& e) {
            std::cerr << "Erro ao converter distintos: " << token << std::endl;
            no->distintos = 0;
        }
    }

    // frequências
    for (int i = 0; i < ALFABETO; i++) {
        if (std::getline(iss, token, '|') && !token.empty()) {
            try {
                no->freq[i] = std::stoi(token);
            } catch (const std::exception& e) {
                std::cerr << "Erro ao converter frequência " << i << ": " << token << std::endl;
                no->freq[i] = 0;
            }
        } else {
            no->freq[i] = 0;
        }
    }

    // número de filhos
    int num_filhos = 0;
    if (std::getline(iss, token, '|') && !token.empty()) {
        try {
            num_filhos = std::stoi(token);
        } catch (const std::exception& e) {
            std::cerr << "Erro ao converter número de filhos: " << token << std::endl;
            num_filhos = 0;
        }
    }

    // chaves dos filhos
    std::vector<int> keys;
    for (int i = 0; i < num_filhos; i++) {
        if (std::getline(iss, token, '|') && !token.empty()) {
            try {
                keys.push_back(std::stoi(token));
            } catch (const std::exception& e) {
                std::cerr << "Erro ao converter chave do filho " << i << ": " << token << std::endl;
                // Pula este filho
            }
        }
    }

    // carregar recursivamente os filhos
    for (int k : keys) {
        PatriciaNode* filho = carregarNo(file);
        if (filho) {
            no->filhos[static_cast<unsigned char>(k)] = filho;
        }
    }

    return no;
}
