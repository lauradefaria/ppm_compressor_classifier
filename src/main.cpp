#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>
#include <iomanip>
#include <cstring>
#include <set>
#include <iomanip>
#include <filesystem>
#include <regex>
#include <climits>
#include <map>

#include "ReadData.hpp"
#include "Config.hpp"
#include "gf/generalFunctions.hpp"

#include "arithmetic/FrequencyTable.hpp"
#include "arithmetic/BitIoStream.hpp"
#include "arithmetic/ArithmeticCoder.hpp"

#include "ppm/PatriciaTree.hpp"

// Uso: ./main -encode <arquivo a ser comprimido>
//          ou -create_table <arquivo a ser usado para treinamento>
//          ou -generate_text <arquivo da arvore>
//          ou -train <diretório com os arquivos de treinamento>
//          ou -validate <arquivo .csv de conjunto validação>
//          ou -test <arquivo .csv de conjunto teste>

int main(int argc, char *argv[]) {

    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo>" << std::endl;
        return 1;
    }

    if (strcmp(argv[1], "-encode") == 0) {
        std::cout << "Comprimindo..." << std::endl;

        std::string message;
        ReadData data;
        data.readtxt(message, argv[2]);
    
        auto start_time_encode = std::chrono::steady_clock::now();
        encode_master(message);
        int compressed_size = std::ifstream("output.bin", std::ios::binary | std::ios::ate).tellg();
        auto end_time_encode = std::chrono::steady_clock::now();   
    
        auto start_time_decode = std::chrono::steady_clock::now();
        decode_master(message);
        auto end_time_decode = std::chrono::steady_clock::now();
    
        auto encoding_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_encode - start_time_encode);
        std::cout << std::endl << "Tempo de codificacao: " << std::fixed << std::setprecision(3) << (double)(encoding_duration.count() / 1000.0) << " segundos" << std::endl;
        auto decoding_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_decode - start_time_decode);
        std::cout << std::endl << "Tempo de decodificacao: " << std::fixed << std::setprecision(3) << (double)(decoding_duration.count() / 1000.0) << " segundos" << std::endl;
        auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_decode - start_time_encode);
        std::cout << std::endl << "Tempo total: " << std::fixed << std::setprecision(3) << (double)(total_duration.count() / 1000.0) << " segundos" << std::endl;

        compressed_size = compressed_size - 4;

        std::cout << std::endl << "=== ANALISE DE COMPRESSAO ===" << std::endl;
        std::cout << "Tamanho original: " << message.length() << " bytes" << std::endl;
        std::cout << "Tamanho comprimido: " << compressed_size << " bytes" << std::endl;
        double compression_ratio = static_cast<double>(compressed_size) / message.length();
        std::cout << "Taxa de compressao: " << compression_ratio << std::endl;
        std::cout << "Reducao: " << (1.0 - compression_ratio) * 100 << " por centagem" << std::endl;

        ///--=-=-=-=-=-=-
        // std::cout << std::fixed << std::setprecision(6) << numero << std::endl;
        double avg_code_length = (compressed_size * 8.0) / message.length();

        std::cout << std::fixed << std::setprecision(6) << "\nComprimento medio da mensagem codificada: " << avg_code_length << " bits/simbolo" << std::endl;

    } else if (strcmp(argv[1], "-generate_text") == 0) {
        std::cout << "gerando texto a partir da tabela..." << std::endl;

        PatriciaTree arvore;
        arvore.carregarArvore("arvore.txt");
        // arvore.imprimirTabela();

        generate_text(arvore);

    } else if (strcmp(argv[1], "-create_table") == 0) {
        std::string message;
        ReadData data;
        data.readtxt(message, argv[2]);

        std::cout << "Criando tabela para treinamento da geração de texto..." << std::endl;
        std::string nomeArvore = "arvore.txt";
        create_tree(message, nomeArvore, KMAX);
    }else if (strcmp(argv[1], "-train") == 0) {

        std::vector<std::string> arquivos;
        std::string diretorio = argv[2]; // caminho do diretório

        // coleta todos os arquivos do diretório
        for (const auto& entry : std::filesystem::directory_iterator(diretorio)) {
            if (entry.is_regular_file()) {
                arquivos.push_back(entry.path().string());
            }
        }
        
        // processa cada arquivo
        for (const auto& arquivo : arquivos) {
            // cria para vários k
            for (int k = 0; k <= 6; k++){
                std::string message;
                ReadData data;
                data.readtxt(message, arquivo.c_str());
                
                std::cout << "Treinando com arquivo: " << arquivo << " e k: " << k << std::endl;
                // Extrai apenas o nome do arquivo sem o caminho completo
                std::string nomeArquivo = std::filesystem::path(arquivo).filename().string();
                std::string nomeArvore = "arvore_k" + std::to_string(k) + "_" + nomeArquivo;
                create_tree(message, nomeArvore, k);
            }
        }
        
    }else if (strcmp(argv[1], "-validate") == 0) {
        
        std::vector<std::string> messages;
        std::vector<std::string> label;
        
        std::cout << "Lendo CSV de validação..." << std::endl;
        ReadData data;
        data.readCSV(argv[2], messages, label);

        // vetor: [k][mensagem] = classificação
        std::vector <std::vector <std::string>> classifications;
        classifications.resize(7);
        // preenche matriz com string vazia
        for (int k = 0; k <= 6; k++) {
            classifications[k].resize(messages.size());
            for (int i = 0; i < messages.size(); i++) {
                classifications[k][i] = "";
            }
        }
        
        // Exemplo de uso dos vetores
        // std::cout << "\n=== EXEMPLO DOS PRIMEIROS REGISTROS ===" << std::endl;
        // for (size_t i = 0; i < std::min(size_t(5), messages.size()); i++) {
        //     std::cout << "Registro " << (i+1) << ":" << std::endl;
        //     std::cout << "  Mensagem: " << messages[i].substr(0, 100) << "..." << std::endl;
        //     std::cout << "  Classificação: " << label[i] << std::endl;
        //     std::cout << std::endl;
        // }
        // std::cout << "Total de registros processados: " << label.size() << std::endl;

        // carrega todas arvores de todos os ks
        std::map<std::pair<int, std::string>, std::unique_ptr<PatriciaTree>> arvores;

        for (int k = 0; k <= 6; k++) {
            std::string k_model = "k" + std::to_string(k);
            for (const auto& entry : std::filesystem::directory_iterator("modelos/" + k_model)) {
                std::string filename = entry.path().filename().string();
                std::regex pattern("_([^_.]+)\\.txt");
                std::smatch matches;
                std::string classe;
                if (std::regex_search(filename, matches, pattern)) {
                    classe = matches[1].str();
                }
        
                auto arvore = std::make_unique<PatriciaTree>();
                arvore->carregarArvore(entry.path().string());
        
                arvores[{k, classe}] = std::move(arvore);
            }
        }

        // percorre cada mensagem e classifica usando k de 0 até 6
        for (int k = 0; k <= 6; k++) {
            std::cout << "Classificando mensagens com k = " << k << "..." << std::endl;
            for (int i = 0; i < messages.size(); i++) {
            // for (int i = 0; i < 30; i++) {

                int best_compressed_size = INT_MAX;
                for (auto& [key, arvorePtr] : arvores) {
                    auto [k_tree, classe] = key;
                    if (k_tree != k) continue;
                
                    static_ppm(messages[i], *arvorePtr); // passa a árvore por referência
        
                    int compressed_size = std::ifstream("output.bin", std::ios::binary | std::ios::ate).tellg();
                    if (compressed_size < best_compressed_size) {
                        best_compressed_size = compressed_size;
                        classifications[k][i] = classe; // guarda a classe vencedora
                    }
                }
            }
        }

        // // imprime classificações
        // std::cout << std::endl;
        // // for (int i = 0; i < messages.size(); i++) {
        // for (int i = 0; i < messages.size(); i++) {
        // // for (int i = 0; i < 30; i++) {
        //     std::cout << "Mensagem: " << messages[i].substr(0, 100) << "..." << std::endl;
        //     for (int k = 0; k <= 6; k++) {
        //         std::cout << "Classificação para k=" << k << " e mensagem " << i << ": " << classifications[k][i] << std::endl;
        //     }
        //     std::cout << "Verdadeira classificação: " << label[i] << std::endl;
        //     std::cout << std::endl;
        // }

        // escreve um .txt para cada resultado de k
        for (int k = 0; k <= 6; k++) {
            std::cout << "Escrevendo arquivo de resultados para k = " << k << "..." << std::endl;
            std::string filename = "resultados_k" + std::to_string(k) + ".txt";
            std::ofstream arquivo(filename);
            
            if (arquivo.is_open()) {
                // Primeira linha: classificações obtidas separadas por vírgula
                for (int i = 0; i < messages.size(); i++) {
                // for (int i = 0; i < 30; i++) {
                    arquivo << classifications[k][i];
                    if (i < messages.size() - 1) {
                        arquivo << ",";
                    }
                }
                arquivo << std::endl;
                
                // Segunda linha: labels verdadeiros separados por vírgula
                for (int i = 0; i < label.size(); i++) {
                // for (int i = 0; i < 30; i++) {
                    arquivo << label[i];
                    if (i < label.size() - 1) {
                        arquivo << ",";
                    }
                }
                arquivo << std::endl;
                
                arquivo.close();
                std::cout << "Arquivo criado: " << filename << std::endl;
            } else {
                std::cerr << "Erro ao criar arquivo: " << filename << std::endl;
            }
        }

    }else if (strcmp(argv[1], "-test") == 0) {
     // melhor k encontrado na validação
        int k = 6;
        
        std::vector<std::string> messages;
        std::vector<std::string> label;
        
        std::cout << "Lendo CSV de testes..." << std::endl;
        ReadData data;
        data.readCSV(argv[2], messages, label);

        // vetor: [mensagem] = classificação
        std::vector <std::string> classifications;
        // preenche matriz com string vazia
        for (int i = 0; i < messages.size(); i++) {
            classifications.push_back("");
        }
        
        // carrega arvores de k
        std::map<std::pair<int, std::string>, std::unique_ptr<PatriciaTree>> arvores;
        std::string k_model = "k" + std::to_string(k);
        for (const auto& entry : std::filesystem::directory_iterator("modelos/" + k_model)) {
            std::string filename = entry.path().filename().string();
            // Regex para pegar apenas o nome da classe antes do .txt
            // Exemplo: arvore_k6_depression.txt -> depression
            std::regex pattern("arvore_k\\d+_([^.]+)\\.txt");
            std::smatch matches;
            std::string classe;
            if (std::regex_search(filename, matches, pattern)) {
                classe = matches[1].str();
            } else {
                // fallback: pega tudo antes do .txt
                size_t pos1 = filename.find_last_of('_');
                size_t pos2 = filename.find_last_of('.');
                if (pos1 != std::string::npos && pos2 != std::string::npos && pos2 > pos1) {
                    classe = filename.substr(pos1 + 1, pos2 - pos1 - 1);
                } else {
                    classe = filename;
                }
            }

            auto arvore = std::make_unique<PatriciaTree>();
            arvore->carregarArvore(entry.path().string());

            arvores[{k, classe}] = std::move(arvore);
        }
        

        // percorre cada mensagem e classifica usando k
        std::cout << "Classificando mensagens com k=" << k << std::endl;
        for (int i = 0; i < messages.size(); i++) {
            int best_compressed_size = INT_MAX;
            for (auto& [key, arvorePtr] : arvores) {
                auto [k_tree, classe] = key;

                //std::cout << "Classe: " << classe << std::endl;
            
                static_ppm(messages[i], *arvorePtr); // passa a árvore por referência
    
                int compressed_size = std::ifstream("output.bin", std::ios::binary | std::ios::ate).tellg();
                if (compressed_size < best_compressed_size) {
                    best_compressed_size = compressed_size;
                    classifications[i] = classe; // guarda a classe vencedora
                }
            }
        }

        // escreve um .txt para cada resultado de k
        std::cout << "Escrevendo resultados em arquivos..." << std::endl;
        std::string filename = "resultados_teste_k" + std::to_string(k) + ".txt";
        std::ofstream arquivo(filename);
        
        if (arquivo.is_open()) {
            // Primeira linha: classificações obtidas separadas por vírgula
            for (int i = 0; i < messages.size(); i++) {
                arquivo << classifications[i];
                if (i < messages.size() - 1) {
                    arquivo << ",";
                }
            }
            arquivo << std::endl;
            
            // Segunda linha: labels verdadeiros separados por vírgula
            for (int i = 0; i < label.size(); i++) {
                arquivo << label[i];
                if (i < label.size() - 1) {
                    arquivo << ",";
                }
            }
            arquivo << std::endl;
            
            arquivo.close();
            std::cout << "Arquivo criado: " << filename << std::endl;
        } else {
            std::cerr << "Erro ao criar arquivo: " << filename << std::endl;
        }   
    }
    else {
        std::cerr << "Entrada inválida!" << std::endl;
        return 1;
    }

    return 0;
}