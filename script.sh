#!/bin/bash

inputDir="./silesia"
outputFile="resultados.txt"

# Limpa o arquivo de saída
: > "$outputFile"

for file in "$inputDir"/*; do
    echo "Processando $file ..."

    # Executa o programa e captura a saída
    output=$(./main -encode "$file")

    # Extrai as linhas de interesse usando grep + awk
    tempoCod=$(echo "$output" | grep "Tempo de codificacao"  | awk '{print $4, $5}')
    tempoDec=$(echo "$output" | grep "Tempo de decodificacao" | awk '{print $4, $5}')
    tempoTot=$(echo "$output" | grep "Tempo total"           | awk '{print $3, $4}')
    tamOrig=$(echo "$output"  | grep "Tamanho original" | tail -n 1 | awk '{print $3, $4}')
    tamComp=$(echo "$output"  | grep "Tamanho comprimido" | awk '{print $3, $4}')
    taxaComp=$(echo "$output" | grep "Taxa de compressao" | awk '{print $4}')
    reducao=$(echo "$output"  | grep "Reducao"            | awk '{print $2}')
    comprMed=$(echo "$output" | grep "Comprimento medio"  | awk '{print $6, $7}')
    entropia=$(echo "$output" | grep "Entropia"           | awk '{print $2, $3}')

    # Escreve no arquivo de saída
    {
        echo "$(basename "$file"):"
        echo "Tempo de codificacao: $tempoCod"
        echo "Tempo de decodificacao: $tempoDec"
        echo "Tempo total: $tempoTot"
        echo "Tamanho original: $tamOrig"
        echo "Tamanho comprimido: $tamComp"
        echo "Taxa de compressao: $taxaComp"
        echo "Reducao: ${reducao}%"
        echo "Comprimento medio da mensagem codificada: $comprMed"
        echo "Entropia: $entropia"
        echo ""
    } >> "$outputFile"

done

echo "Resultados salvos em $outputFile"
