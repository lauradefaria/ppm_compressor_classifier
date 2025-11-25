#ifndef READ_DATA_HPP
#define READ_DATA_HPP

#include <fstream> 
#include <sstream>
#include <string>  
#include <vector>

class ReadData
{
    public:
        void readtxt(std::string &message, const std::string &filename);
        
        // Função para ler CSV com duas colunas
        void readCSV(const std::string &filename, 
                           std::vector<std::string> &messages, 
                           std::vector<std::string> &classifications);
};

#endif // READ_DATA_HPP