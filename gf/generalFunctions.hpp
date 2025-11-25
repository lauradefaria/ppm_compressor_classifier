#ifndef GENERAL_FUNCTIONS_HPP
#define GENERAL_FUNCTIONS_HPP

#include <string>

#include "../ppm/PatriciaTree.hpp"

void create_tree(std::string& message, std::string& nomeArvore, int kmax);
void generate_text(PatriciaTree &arvore);
void decode_master(std::string& original_message);
void encode_master(std::string& message);
void static_ppm(std::string& message, const PatriciaTree &arvore);

#endif // GENERAL_FUNCTIONS_HPP