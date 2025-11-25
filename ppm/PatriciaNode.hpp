#ifndef PATRICIA_NODE_HPP
#define PATRICIA_NODE_HPP

#include <string>
#include <vector>
#include <map>
#include <cstdint>

#include "../src/Config.hpp"

struct PatriciaNode {
    std::string prefixo;                            // segmento compactado
    std::vector<uint32_t> freq;                     // contagens por símbolo
    uint32_t distintos;                             // nº de símbolos distintos
    std::map<unsigned char, PatriciaNode*> filhos;  // map de filhos do nó, indentificado pelo primeiro caractere do contexto

    // construtor - inicializa o nó atual
    PatriciaNode(std::string p = "")
        : prefixo(std::move(p)), freq(ALFABETO, 0), distintos(0) {} 
};

#endif
