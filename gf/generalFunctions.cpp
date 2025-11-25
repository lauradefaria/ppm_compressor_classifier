#include "generalFunctions.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>
#include <iomanip>
#include <random>
#include <cmath>
#include <set>

#include "../src/ReadData.hpp"
#include "../src/Config.hpp"

#include "../arithmetic/FrequencyTable.hpp"
#include "../arithmetic/BitIoStream.hpp"
#include "../arithmetic/ArithmeticCoder.hpp"

void generate_text(PatriciaTree &arvore) { 
    std::string message;
    std::vector<unsigned char> historico;

    std::random_device rd;
    std::mt19937 gen(rd());

    int max_size = 0;
    while (true) {
        unsigned char simbolo = 0;

        // tenta contextos de KMAX até 0
        int hsize = static_cast<int>(historico.size());
        int maxLen = std::min(KMAX, hsize);

        // explora do maior para o menor contexto
        for (int len = maxLen; len >= 0; len--) {

            // constroi contexto
            std::string contexto;
            contexto.reserve(len);
            for (int j = hsize - len; j < hsize; j++) {
                contexto.push_back(static_cast<char>(historico[j]));
            }
            
            // procura contexto na árvore
            PatriciaNode* no = arvore.buscarContexto(contexto);
            
            if (no != nullptr) {
                // cria distribuição de probabilidades baseada nas frequências
                std::vector<std::uint32_t> freqs = no->freq;
                
                // se há símbolos não vistos, inclui o ESC
                if (no->distintos < ALFABETO) {
                    freqs.push_back(no->distintos); // frequencia de ESC equivale ao número de símbolos existentes
                }

                // cria distribuição de probabilidades com pesos
                std::discrete_distribution<> dist(freqs.begin(), freqs.end());

                // gera símbolo aleatório
                uint32_t simbolo_temp = dist(gen);

                // std::cout << "Simbolo gerado: " << (char)simbolo_temp << std::endl;

                if (simbolo_temp == ESC_SYMBOL) {
                    // ESC gerado, continua para contexto menor
                    continue;
                } else {
                    simbolo = static_cast<unsigned char>(simbolo_temp);
                    break;
                }
            }
        }

        message.push_back(static_cast<char>(simbolo));
        historico.push_back(simbolo);

        // Mantém histórico limitado
        if (static_cast<int>(historico.size()) > KMAX) {
            historico.erase(historico.begin());
        }

        max_size++;

        if (max_size > 2000 /*&& simbolo == (int)(' ')*/)
            break;
    }
    
    // salvar mensagem em arquivo
    std::ofstream output_file("generated_message.txt");
    output_file.write(message.c_str(), message.size());
    output_file.close();
    
    std::cout << "\n=== TEXTO GERADO ===" << std::endl;
    std::cout << message << std::endl;
    std::cout << "===================" << std::endl;
}

void create_tree(std::string& message, std::string& nomeArvore, int kmax) {
    
    // código que apenas cria a tabela, sem comprimir
    PatriciaTree arvore;
    std::vector<unsigned char> historico;

    unsigned char simbolo;
    for (size_t i = 0; i < message.size(); i++) {

        simbolo = message[i];

        int hsize = static_cast<int>(historico.size());
        int maxLen = std::min(kmax, hsize);

        // insere contexto na árvore
        for (int len = 0; len <= maxLen; ++len) {
            std::string contexto;
            contexto.reserve(len);
            for (int i = hsize - len; i < hsize; ++i)
                contexto.push_back(static_cast<char>(historico[i]));
            arvore.inserirContexto(contexto, simbolo);
        }

        historico.push_back(simbolo);
        if (static_cast<int>(historico.size()) > kmax)
            historico.erase(historico.begin());
    }

    //arvore.imprimirTabela();
    arvore.salvarArvore(nomeArvore);
}

void decode_master(std::string& original_message) {

    std::cout << "Decodificando mensagem..." << std::endl;

    PatriciaTree arvore;
    std::vector<unsigned char> historico; 

    // cria set de símbolos vistos (mesmo do codificador)
    std::set<unsigned char> simbolosVistos;

    // inicializa arquivo de entrada
    std::ifstream input_file("output.bin", std::ios::binary);

    // lê o cabeçalho que ocupa 4 bytes para obter o tamanho original do arquivo
    uint32_t original_size;
    input_file.read(reinterpret_cast<char*>(&original_size), sizeof(uint32_t));
    BitInputStream bit_input(input_file);

    // inicializa decodificador
    ArithmeticDecoder decoder(32, bit_input);
    std::string decoded_message;
    
    for (uint32_t i = 0; i < original_size; i++) {

        // registra símbolos a serem excluídos, que já sabemos que não irão aparecer
        std::set<unsigned char> simbolosExcluidos;

        // variáveis para limitar o tamanho do contexto (mesma lógica do codificador)
        int hsize = static_cast<int>(historico.size()); // tamanho do historico
        int maxLen = std::min(KMAX, hsize);             // tamanho maximo do contexto atual

        unsigned char simbolo = 0;
        bool decodificado = false;

        // tentar contextos de KMAX até 0 (percorre os contextos em ordem decrescente)
        for (int len = maxLen; len >= 0; len--)
        {
            std::string contexto;
            contexto.reserve(len);
            for (int j = hsize - len; j < hsize; j++)
            {
                contexto.push_back(static_cast<char>(historico[j]));
            }

            // procura contexto na árvore
            PatriciaNode* no = arvore.buscarContexto(contexto);

            if (no != nullptr)
            {   
                std::vector<std::uint32_t> freqs;
                if (i == 0) {
                    freqs.resize(ALFABETO, 0);
                    freqs.push_back(1);
                } else {
                    freqs = no->freq;
                }

                // remove símbolos que foram excluídos em contextos maiores
                for (unsigned char s : simbolosExcluidos) {
                    if (freqs[s] > 0) {
                        freqs[s] = 0;
                    }
                }

                // se há símbolos não vistos, inclui o ESC (mesma lógica do codificador)
                if (no->distintos < ALFABETO)
                    freqs.push_back(no->distintos);          // frequencia de ESC equivale ao número de símbolos existentes
                SimpleFrequencyTable freq_table(freqs);      // cria atual frequency table

                // decodifica símbolo ou ESC
                std::uint32_t decoded_symbol = decoder.read(freq_table);
                
                if (decoded_symbol == ESC_SYMBOL)
                {
                    // ESC decodificado, continua para contexto menor
                    // adiciona todos os símbolos deste contexto ao conjunto de excluídos
                    for (int s = 0; s < ALFABETO; s++) {
                        if (no->freq[s] > 0) {
                            simbolosExcluidos.insert(static_cast<unsigned char>(s));
                        }
                    }

                    continue;
                }
                else
                {
                    simbolo = static_cast<unsigned char>(decoded_symbol);
                    decodificado = true;
                    break;
                }
            }
        }

        // se não foi possível decodificar, usa equiprobabilidades (mesma lógica do codificador)
        if (!decodificado)
        {
            // Cria equiprobabilidade apenas para símbolos não vistos (MESMA LÓGICA DO CODIFICADOR)
            std::vector<std::uint32_t> freqs_equal(ALFABETO, 0);
            int simbolosNaoVistos = 0;
            
            for (int s = 0; s < ALFABETO; s++) {
                if (simbolosVistos.find(static_cast<unsigned char>(s)) == simbolosVistos.end()) {
                    freqs_equal[s] = 1;
                    simbolosNaoVistos++;
                }
            }

            SimpleFrequencyTable freq_table(freqs_equal);
            std::uint32_t decoded_symbol = decoder.read(freq_table);
            simbolo = static_cast<unsigned char>(decoded_symbol);
        }

        // Atualiza estruturas (mesma lógica do codificador)
        simbolosVistos.insert(simbolo);
        decoded_message += static_cast<char>(simbolo);

        // atualizar a árvore (mesma lógica do codificador)
        for (int len = 0; len <= maxLen; len++)
        {
            std::string contexto;
            contexto.reserve(len);
            for (int j = hsize - len; j < hsize; j++)
            {
                contexto.push_back(static_cast<char>(historico[j]));
            }
            arvore.inserirContexto(contexto, simbolo);
        }

        // administra tamanho do histórico para não estourar a memória (mesma lógica do codificador)
        historico.push_back(simbolo);
        if (static_cast<int>(historico.size()) > KMAX)
            historico.erase(historico.begin());
    }

    input_file.close();

    // verificar se a mensagem decodificada é igual à original
    if (original_message == decoded_message) {
        std::cout << "SUCESSO: Mensagem decodificada eh identica a original!" << std::endl;
        std::cout << "Tamanho original: " << original_message.length() << std::endl;
        std::cout << "Tamanho decodificado: " << decoded_message.length() << std::endl;
    } else {
        std::cout << "ERRO: Mensagem decodificada e diferente da original!" << std::endl;
        std::cout << "Tamanho original: " << original_message.length() << std::endl;
        std::cout << "Tamanho decodificado: " << decoded_message.length() << std::endl;
        
        // mostrar as primeiras diferenças
        size_t min_len = std::min(original_message.length(), decoded_message.length());
        for (size_t i = 0; i < min_len && i < 100; i++) {
            if (original_message[i] != decoded_message[i]) {
                std::cout << "Primeira diferença na posicao " << i << ": original=" << (int)original_message[i] 
                         << " decodificado=" << (int)decoded_message[i] << std::endl;
                break;
            }
        }
    }
    
    // mostrar inicio das mensagens
    std::cout << std::endl << "Mensagem original: " << original_message.substr(0, 100) << "..." << std::endl;
    std::cout << "Mensagem decodificada: " << decoded_message.substr(0, 100) << "..." << std::endl;

    // salva mensagem decodificada em arquivo
    std::ofstream output_file("decoded_message", std::ios::binary);
    output_file.write(decoded_message.c_str(), decoded_message.size());
    output_file.close();
}

void encode_master(std::string& message)
{
    std::cout << "Codificando mensagem..." << std::endl;
    
    PatriciaTree arvore;
    std::vector<unsigned char> historico;

    // cria set de símbolos vistos
    std::set<unsigned char> simbolosVistos;

    // inicializa arquivo de saída
    std::ofstream output_file("output.bin", std::ios::binary);

    // grava tamanho original do arquivo no cabeçalho
    uint32_t original_size = static_cast<uint32_t>(message.size());
    output_file.write(reinterpret_cast<const char*>(&original_size), sizeof(uint32_t));

    BitOutputStream bit_output(output_file);

    // inicializa codificador
    ArithmeticEncoder encoder(32, bit_output);

    double soma_informacao = 0.0;

    // percorre cada símbolo da mensagem
    for (size_t i = 0; i < message.length(); i++)
    {
        unsigned char simbolo = message[i];
        // std::cout << "Simbolo: " << (char)simbolo << std::endl;

        // registra símbolos a serem excluídos, que já sabemos que não irão aparecer
        std::set<unsigned char> simbolosExcluidos;

        // variáveis para limitar o tamanho do contexto
        int hsize = static_cast<int>(historico.size()); // tamanho do historico
        int maxLen = std::min(KMAX, hsize);             // tamanho maximo do contexto atual

        // tentar contextos de KMAX até 0 (percorre os contextos em ordem decrescente)
        bool codificado = false;
        for (int len = maxLen; len >= 0; len--)
        {
            std::string contexto;
            contexto.reserve(len);
            for (int j = hsize - len; j < hsize; j++)
            {
                contexto.push_back(static_cast<char>(historico[j]));
            }

            // procura contexto na árvore
            PatriciaNode* no = arvore.buscarContexto(contexto);

            // se contexto foi encontrado
            if (no != nullptr)
            {   
                std::vector<std::uint32_t> freqs;
                if (i == 0) {
                    freqs.resize(ALFABETO, 0);
                    freqs.push_back(1);
                } else {
                    freqs = no->freq;
                }

                // remove símbolos que foram excluídos em contextos maiores
                for (unsigned char s : simbolosExcluidos) {
                    if (freqs[s] > 0) {
                        freqs[s] = 0;
                    }
                }

                // se há símbolos não vistos, inclui o ESC
                if (no->distintos < ALFABETO)
                    freqs.push_back(no->distintos);          // frequencia de ESC equivale ao número de símbolos existentes
                SimpleFrequencyTable freq_table(freqs);      // cria atual frequency table

                // se símbolo está no contexto
                if (no->freq[simbolo] > 0)
                {   
                    // std::cout << "Codificando simbolo no contexto K=" << len << std::endl;        
                    // codifica o símbolo
                    // *** Informação se símbolo está no contexto ***
                    double p = static_cast<double>(freq_table.get(simbolo)) / freq_table.getTotal();
                    soma_informacao += -std::log2(p);

                    encoder.write(freq_table, static_cast<std::uint32_t>(simbolo));
                    codificado = true;
                    break;
                }
                else
                {
                    // *** Informação do ESC ***
                    double pEsc = static_cast<double>(freq_table.get(ESC_SYMBOL)) / freq_table.getTotal();
                    soma_informacao += -std::log2(pEsc);

                    encoder.write(freq_table, ESC_SYMBOL);

                    // adiciona todos os símbolos deste contexto ao conjunto de excluídos
                    for (int s = 0; s < ALFABETO; s++) {
                        if (no->freq[s] > 0) {
                            simbolosExcluidos.insert(static_cast<unsigned char>(s));
                        }
                    }
                }
            }
        }

        // se não foi possível codificar, usa equiprobabilidades
        if (!codificado)
        {
            // std::cout << "Codificando com equiprobabilidades" << std::endl;

            // Cria equiprobabilidade apenas para símbolos não vistos
            std::vector<std::uint32_t> freqs_equal(ALFABETO, 0);
            int simbolosNaoVistos = 0;
            
            for (int s = 0; s < ALFABETO; s++) {
                if (simbolosVistos.find(static_cast<unsigned char>(s)) == simbolosVistos.end()) {
                    freqs_equal[s] = 1;
                    simbolosNaoVistos++;
                }
            }

            SimpleFrequencyTable freq_table(freqs_equal);
            
            double p = static_cast<double>(freq_table.get(simbolo)) / freq_table.getTotal();
            soma_informacao += -std::log2(p);

            encoder.write(freq_table, static_cast<std::uint32_t>(simbolo));
            // std::cout << std::endl;
        }

        simbolosVistos.insert(simbolo);

        // atualizar a árvore
        for (int len = 0; len <= maxLen; len++)
        {
            std::string contexto;
            contexto.reserve(len);
            for (int j = hsize - len; j < hsize; j++)
            {
                contexto.push_back(static_cast<char>(historico[j]));
            }
            arvore.inserirContexto(contexto, simbolo);
        }

        // arvore.imprimirTabela();

        // administra tamanho do histórico para não estourar a memória
        historico.push_back(simbolo);
        if (static_cast<int>(historico.size()) > KMAX)
            historico.erase(historico.begin());

    }

    // arvore.imprimirTabela();

    double informacao_media = soma_informacao / message.size();
    std::cout << std::fixed << std::setprecision(6) << "Entropia: " << informacao_media << " bits/simbolo" << std::endl;

    encoder.finish();
    output_file.close();
}

void static_ppm(std::string& message, const PatriciaTree &arvore) {

    // std::cout << "Codificando mensagem (PPM estático)..." << std::endl;
    
    std::vector<unsigned char> historico;
    std::set<unsigned char> simbolosVistos;
    
    std::ofstream output_file("output.bin", std::ios::binary);
    
    // grava tamanho original do arquivo no cabeçalho
    uint32_t original_size = static_cast<uint32_t>(message.size());
    output_file.write(reinterpret_cast<const char*>(&original_size), sizeof(uint32_t));
    
    BitOutputStream bit_output(output_file);
    ArithmeticEncoder encoder(32, bit_output);
    
    double soma_informacao = 0.0;
    
    // percorre cada símbolo da mensagem
    for (size_t i = 0; i < message.length(); i++) {
        unsigned char simbolo = message[i];
    
        std::set<unsigned char> simbolosExcluidos;
    
        int hsize = static_cast<int>(historico.size());
        int maxLen = std::min(KMAX, hsize);
    
        bool codificado = false;
        for (int len = maxLen; len >= 0; len--) {
            std::string contexto;
            contexto.reserve(len);
            for (int j = hsize - len; j < hsize; j++) {
                contexto.push_back(static_cast<char>(historico[j]));
            }
    
            // busca contexto na árvore já treinada
            PatriciaNode* no = arvore.buscarContexto(contexto);
    
            if (no != nullptr) {
                std::vector<std::uint32_t> freqs = no->freq;
    
                // remove símbolos já excluídos em contextos maiores
                for (unsigned char s : simbolosExcluidos) {
                    if (freqs[s] > 0) freqs[s] = 0;
                }
    
                // se ainda há símbolos não vistos, inclui o ESC
                if (no->distintos < ALFABETO)
                    freqs.push_back(no->distintos);
    
                SimpleFrequencyTable freq_table(freqs);
    
                if (no->freq[simbolo] > 0) {
                    // símbolo encontrado neste contexto
                    double p = static_cast<double>(freq_table.get(simbolo)) / freq_table.getTotal();
                    soma_informacao += -std::log2(p);
    
                    encoder.write(freq_table, static_cast<std::uint32_t>(simbolo));
                    codificado = true;
                    break;
                } else {
                    // precisa emitir ESC
                    double pEsc = static_cast<double>(freq_table.get(ESC_SYMBOL)) / freq_table.getTotal();
                    soma_informacao += -std::log2(pEsc);
    
                    encoder.write(freq_table, ESC_SYMBOL);
    
                    // adiciona símbolos deste contexto à lista de excluídos
                    for (int s = 0; s < ALFABETO; s++) {
                        if (no->freq[s] > 0) {
                            simbolosExcluidos.insert(static_cast<unsigned char>(s));
                        }
                    }
                }
            }
        }
    
        // fallback: equiprobabilidade se não codificou
        if (!codificado) {
            std::vector<std::uint32_t> freqs_equal(ALFABETO, 0);
            int simbolosNaoVistos = 0;
            for (int s = 0; s < ALFABETO; s++) {
                if (simbolosVistos.find(static_cast<unsigned char>(s)) == simbolosVistos.end()) {
                    freqs_equal[s] = 1;
                    simbolosNaoVistos++;
                }
            }
    
            SimpleFrequencyTable freq_table(freqs_equal);
    
            double p = static_cast<double>(freq_table.get(simbolo)) / freq_table.getTotal();
            soma_informacao += -std::log2(p);
    
            encoder.write(freq_table, static_cast<std::uint32_t>(simbolo));
        }
    
        simbolosVistos.insert(simbolo);
    
        // apenas atualiza o histórico para manter o contexto
        historico.push_back(simbolo);
        if ((int)historico.size() > KMAX)
            historico.erase(historico.begin());
    }
    
    // double informacao_media = soma_informacao / message.size();
    // std::cout << std::fixed << std::setprecision(6)
    //           << "Entropia: " << informacao_media << " bits/simbolo" << std::endl;
    
    encoder.finish();
    output_file.close();
}