
$inputDir = ".\silesia"
$outputFile = "resultados.txt"

"" | Out-File $outputFile

Get-ChildItem $inputDir | ForEach-Object {
    $file = $_.FullName
    Write-Host "Processando $file ..."

    # Executa o programa e captura a saída
    $output = & .\main -encode $file

    # Extrai as linhas de interesse
    $tempoCod  = ($output | Select-String "Tempo de codificacao").ToString().Split()[3..4] -join " "
    $tempoDec  = ($output | Select-String "Tempo de decodificacao").ToString().Split()[3..4] -join " "
    $tempoTot  = ($output | Select-String "Tempo total").ToString().Split()[2..3] -join " "
    $tamOrig   = ($output | Select-String "Tamanho original" | Select-Object -Last 1).ToString().Split()[2..3] -join " "
    $tamComp   = ($output | Select-String "Tamanho comprimido").ToString().Split()[2..3] -join " "
    $taxaComp  = ($output | Select-String "Taxa de compressao").ToString().Split()[3]
    $reducao   = ($output | Select-String "Reducao").ToString().Split()[1]
    $comprMed  = ($output | Select-String "Comprimento medio").ToString().Split()[5..6] -join " "
    $entropia  = ($output | Select-String "Entropia").ToString().Split()[1..2] -join " "

    # Escreve no arquivo de saída
    Add-Content $outputFile "$(Split-Path $file -Leaf):"
    Add-Content $outputFile "Tempo de codificacao: $tempoCod"
    Add-Content $outputFile "Tempo de decodificacao: $tempoDec"
    Add-Content $outputFile "Tempo total: $tempoTot"
    Add-Content $outputFile "Tamanho original: $tamOrig"
    Add-Content $outputFile "Tamanho comprimido: $tamComp"
    Add-Content $outputFile "Taxa de compressao: $taxaComp"
    Add-Content $outputFile "Reducao: $reducao`%"
    Add-Content $outputFile "Comprimento medio da mensagem codificada: $comprMed"
    Add-Content $outputFile "Entropia: $entropia"
    Add-Content $outputFile ""
}
Write-Host "Resultados salvos em $outputFile"