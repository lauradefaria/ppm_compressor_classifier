<a href='https://isocpp.org/' target="_blank"><img alt='C++' src='https://img.shields.io/badge/C++-100000?style=for-the-badge&logo=c%2B%2B&logoColor=white&labelColor=00599C&color=00599C'/></a>
<a href='https://gcc.gnu.org/' target="_blank"><img alt='GCC' src='https://img.shields.io/badge/GCC-100000?style=for-the-badge&logo=gnu&logoColor=white&labelColor=A42E2B&color=A42E2B'/></a>
<a href='https://www.python.org/' target="_blank"><img alt='Python' src='https://img.shields.io/badge/Python-100000?style=for-the-badge&logo=python&logoColor=white&labelColor=3776AB&color=3776AB'/></a>
<a href='https://jupyter.org/' target="_blank"><img alt='Jupyter' src='https://img.shields.io/badge/Jupyter-100000?style=for-the-badge&logo=jupyter&logoColor=white&labelColor=F37626&color=F37626'/></a>
<a href='https://pandas.pydata.org/' target="_blank"><img alt='Pandas' src='https://img.shields.io/badge/Pandas-100000?style=for-the-badge&logo=pandas&logoColor=white&labelColor=150458&color=150458'/></a>
<a href='https://numpy.org/' target="_blank"><img alt='NumPy' src='https://img.shields.io/badge/NumPy-100000?style=for-the-badge&logo=numpy&logoColor=white&labelColor=013243&color=013243'/></a>
<a href='https://scikit-learn.org/' target="_blank"><img alt='scikit-learn' src='https://img.shields.io/badge/scikit--learn-100000?style=for-the-badge&logo=scikit-learn&logoColor=white&labelColor=F7931E&color=F7931E'/></a>
<a href='https://huggingface.co/' target="_blank"><img alt='HuggingFace' src='https://img.shields.io/badge/HuggingFace-100000?style=for-the-badge&logo=huggingface&logoColor=white&labelColor=FFD21E&color=FFD21E'/></a>
<a href='https://code.visualstudio.com/' target="_blank"><img alt='VS Code' src='https://img.shields.io/badge/VS_Code-100000?style=for-the-badge&logo=visual-studio-code&logoColor=white&labelColor=007ACC&color=007ACC'/></a>
<a href='https://www.reddit.com/' target="_blank"><img alt='Reddit' src='https://img.shields.io/badge/Reddit-100000?style=for-the-badge&logo=reddit&logoColor=white&labelColor=FF4500&color=FF4500'/></a>
</div>

# Projeto da implementação de um PPM-C

Implementação completa do algoritmo **PPM-C (Prediction by Partial Matching)** desenvolvido para a disciplina de Introdução à Teoria da Informação  na UFPB. O projeto apresenta duas aplicações principais:

### Compressor-Descompressor
Sistema de compressão de dados usando PPM-C com codificação aritmética de 32 bits e árvore Patricia para gerenciamento eficiente de contextos. Testado no Corpus Silesia, alcançando resultados competitivos com compressores comerciais como WinRAR e 7zip.

### Classificador de Saúde Mental
Aplicação inovadora do método PPM para identificação de condições de saúde mental a partir de textos do Reddit. O sistema analisa padrões linguísticos para classificar 6 categorias: ansiedade, depressão, bipolar, estresse, transtorno de personalidade e tendências suicidas.

---

## 📂 Estrutura do Projeto
```
.
├── src/                      # Código fonte principal
│   ├── main.cpp             # Ponto de entrada do programa
│   └── ReadData.cpp         # Leitura de arquivos
├── ppm/                      # Implementação da Árvore Patricia
├── arithmetic/               # Codificador Aritmético
├── gf/                       # Funções de encode/decode
├── PreProcessing/            # Scripts de pré-processamento
│   └── HuggingFace.ipynb    # Pipeline de classificação
├── matrizConfusao/           # Matrizes de confusão (K=0 a K=6)
├── modelos.zip              # Modelos treinados para classificação
├── resultados_k*.txt        # Resultados de validação
├── comandoParaRodarPPM      # Exemplo de uso do compressor
├── Makefile                 # Automação de compilação
└── README.md
```

---

## 🌳 Árvore Patricia no PPM-C

A *Patricia tree* (ou *trie comprimida*) é uma variação da árvore trie usada para armazenar strings (ou sequências de símbolos) de forma mais compacta.

Em uma trie “pura”, cada aresta corresponde a um único caractere. Isso gera árvores muito grandes, especialmente se houver poucos dados em comum entre as palavras. Na Patricia tree, várias arestas consecutivas sem bifurcação são comprimidas em uma única aresta que guarda um prefixo inteiro.

**Destaques:**
- Alfabeto completo de bytes (0-255)
- Suporte para K_max variável (testado de 0 a 6)
- Melhor resultado: 0.881 bits/símbolo (arquivo XML, K=6)

### Por que Patricia Tree?

O PPM-C necessita:
1. Manter **tabelas de contexto** com frequências de símbolos
2. **Busca rápida** de contextos já observados
3. **Economia de memória** para milhares de contextos

A Patricia tree resolve isso através de:
- **Compressão**: Prefixos comuns compartilhados em nós únicos
- **Eficiência**: Inserção e busca em O(|k|)
- **Adaptabilidade**: Cresce dinamicamente conforme novos contextos aparecem

### Exemplo Visual

Palavras: `casa`, `caso`, `canto`

**Trie Normal:**
```
c → a → s → a
    ↓
    s → o
    ↓
    n → t → o
```

**Patricia Tree:**
```
c → a → s → [a, o]
    ↓
    n → to
```

---

### Vantagem da Patricia Tree nesse cenário

* **Compacta**: evita desperdiçar memória repetindo cadeias longas sem bifurcações.
* **Adaptativa**: cresce conforme novos contextos são vistos.
* **Eficiente**: busca/inserção são rápidas, essenciais para compressão.
* **Flexível**: cada nó já armazena frequências, número de símbolos distintos e até o símbolo especial ESC (*escape*), usado quando o símbolo não foi visto no contexto.

Exemplo grafico da Árvore Patricia:

<img src="https://prod.ferndocs.com/_next/image?url=https%3A%2F%2Ffiles.buildwithfern.com%2Fhttps%3A%2F%2Falchemy.docs.buildwithfern.com%2Fdocs%2F2025-08-30T01%3A44%3A18.308Z%2Ftutorials%2Falchemy-university%2Fcryptography-basics%2F1920px-Patricia_trie.svg-1-.png&w=1920&q=75" 
     alt="Árvore Redimensionada" width="50%">

Resumindo:
A árvore Patricia serve no PPM-C como uma **estrutura compacta e eficiente para armazenar e acessar as tabelas de contexto**, permitindo inserir novos contextos, atualizar frequências e buscar probabilidades para a compressão e descompressão.

---

## 🧠 Sistema de Classificação

### Base de Dados
- **Fonte**: HuggingFace (textos do Reddit)
- **Total**: 87.396 amostras após pré-processamento
- **Divisão**: 60% treino, 20% validação, 20% teste

### Metodologia
1. **Treinamento**: PPM adaptativo para cada classe
2. **Validação**: Teste de K=0 a K=6 para seleção do melhor modelo
3. **Teste**: Avaliação final com métricas detalhadas

### Resultados por K

| K | Acurácia | Observação |
|---|----------|------------|
| 0 | 41.4% | Ordem zero (frequências globais) |
| 3 | 79.5% | Estabilização inicial |
| **6** | **81.2%** | **Melhor resultado** |

### Matrizes de Confusão

O diretório `matrizConfusao/` contém visualizações completas do desempenho do classificador para cada valor de K, demonstrando a evolução da precisão e os padrões de confusão entre classes.

**Análise**: A confusão mais comum ocorre entre Depressão ↔ Suicídio, devido à sobreposição semântica natural entre essas condições.

**Resultados:**
- **Acurácia: 81.2%** (K=6)
- Base: 87.396 textos processados
- Melhor classe: Transtorno de Personalidade (F1-Score: 0.957)

--- 

## 💻 Execução Local

### Pré-requisitos
Para o Compressor (C++)

- Compilador: GCC 9.0+ ou compatível com C++17
Sistema Operacional:
- Windows 10/11
Linux (Ubuntu 22.04+ recomendado)
- Memória RAM: Mínimo 8GB (16GB recomendado para K > 3)

Para o Classificador (Python)

- Python: 3.8 ou superior
- Jupyter Notebook: Última versão
- Bibliotecas Python: pandas, numpy, scikit-learn, matplotlib, seaborn

### Compilação: Manualmente

#### WINDOWS:  
```bash
g++ -std=c++17 -Wall -Wextra src/main.cpp src/ReadData.cpp ppm/PatriciaTree.cpp gf/generalFunctions.cpp arithmetic/ArithmeticCoder.cpp arithmetic/BitIoStream.cpp arithmetic/FrequencyTable.cpp -I. -I./src -I./ppm -I./gf -I./arithmetic -o main.exe
```

#### LINUX: 
```bash
g++ -std=c++17 -Wall -Wextra src/main.cpp ppm/PatriciaTree.cpp -I. -I./src -I./ppm -o main
```

#### Compilação: MakeFile
```bash
make
./main seu_arquivo_entrada.txt
```

### Passos
1. Clone o repositório:
   ```bash
   git clone https://github.com/lauradefaria/Teoria_da_Informacao.git
   ```
2. Realize o processo de `Compilação` por meio do MakeFile ou manualmente *(verifique o comando acima)*
3. Execute com o comando:
   ```bash
   ./main seu_arquivo_entrada.txt
   ```
4. Para utilizar o classificador de saúde mental
   ```bash
   pip install datasets pandas numpy scikit-learn matplotlib seaborn jupyter
   ```
5. No notebook `PreProcessing/HuggingFace` execute as células sequencialmente.
6. Execute os notebooks da pasta `matrizConfusao/`

---

## 📖 Autores 

| [<img loading="lazy" src="https://avatars.githubusercontent.com/u/45434515?v=4" width=115><br><sub>Laura de Faria</sub>](https://github.com/lauradefaria) | [<img loading="lazy" src="https://avatars.githubusercontent.com/u/72822997?v=4" width=115><br><sub>Renata Mendes</sub>](https://github.com/renatamendesc) | [<img loading="lazy" src="https://avatars.githubusercontent.com/u/64603683?v=4" width=115><br><sub>Rodrigo Pereira</sub>](https://github.com/Rodrigo-P-Nascimento) |
| :---: | :---: | :---: |

---

## 📄 License

[![License](http://img.shields.io/:license-mit-blue.svg?style=flat-square)](http://badges.mit-license.org)

- **[MIT license](http://opensource.org/licenses/mit-license.php)**

---

## 📚 Referências

1. CLEARY, John; WITTEN, Ian. Data Compression Using Adaptive Coding and Partial String Matching. IEEE Transactions on Communications, 1984.

2. NAYUKI. [Reference Arithmetic Coding](https://github.com/nayuki/Reference-arithmetic-coding). GitHub, 2025.

3. SALOMON, David. Data Compression: The Complete Reference. 3ª ed. Springer, 2004.
