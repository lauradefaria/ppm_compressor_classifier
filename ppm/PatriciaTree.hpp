#ifndef PATRICIA_TREE_HPP
#define PATRICIA_TREE_HPP

#include <string>
#include <fstream>
#include "PatriciaNode.hpp"

class PatriciaTree {
private:
    PatriciaNode* raiz;

    void imprimirNo(PatriciaNode* no, const std::string& contextoAcum = "") const;

public:
    PatriciaTree();
    ~PatriciaTree();

    PatriciaNode* buscarContexto(const std::string& contexto) const;

    void inserirContexto(const std::string& contexto, unsigned char simbolo);
    void imprimirTabela() const;

    static void cabecalhoKmenos1();
    static std::string renderSimbolo(int code);

    void liberarMemoria(PatriciaNode* no);

    void salvarArvore(const std::string& nomeArquivo) const;
    void salvarNo(std::ofstream& arquivo, PatriciaNode* no) const;

    void carregarArvore(const std::string& filename);
    PatriciaNode* carregarNo(std::ifstream& file);
};

#endif
