#include "ReadData.hpp"
#include <iostream>

using namespace std;
    
void ReadData::readtxt(string &message, const string &filename)
{
    // ler arquivo como binário
    ifstream file(filename, ios::binary);
    if (file.is_open())
    {
        // obter tamanho do arquivo, movemos o ponteiro para o final do arquivo
        file.seekg(0, ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, ios::beg);
        
        // ler bytes do arquivo
        message.resize(fileSize);
        file.read(&message[0], fileSize);
        file.close();
        cout << "Arquivo '" << filename << "' lido com sucesso (" 
             << fileSize << " bytes)" << endl;
    }
    else
    {
        cerr << "Erro: Não foi possível abrir o arquivo '" << filename << "'" << endl;
        message = "";
    }
}

void ReadData::readCSV(const string &filename, vector<string> &messages, vector<string> &classifications)
{
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Erro: Não foi possível abrir o arquivo '" << filename << "'" << endl;
        return;
    }
    
    string line;
    bool firstLine = true;
    
    while (getline(file, line)) {
        // Pular o cabeçalho
        if (firstLine) {
            firstLine = false;
            continue;
        }
        
        // Verificar se a linha não está vazia
        if (line.empty()) {
            continue;
        }
        
        // Parse da linha CSV considerando aspas
        string text, status;
        bool insideQuotes = false;
        bool foundComma = false;
        
        for (size_t i = 0; i < line.length(); i++) {
            char c = line[i];
            
            if (c == '"') {
                insideQuotes = !insideQuotes;
            } else if (c == ',' && !insideQuotes) {
                foundComma = true;
                continue;
            }
            
            if (!foundComma) {
                text += c;
            } else {
                status += c;
            }
        }
        
        // Remover aspas do início e fim se existirem
        if (!text.empty() && text.front() == '"' && text.back() == '"') {
            text = text.substr(1, text.length() - 2);
        }
        if (!status.empty() && status.front() == '"' && status.back() == '"') {
            status = status.substr(1, status.length() - 2);
        }
        
        // Adicionar aos vetores
        messages.push_back(text);
        classifications.push_back(status);
    }
    
    file.close();
    cout << "CSV lido com sucesso: " << messages.size() << " registros encontrados" << endl;
}
