from sklearn.metrics import confusion_matrix, classification_report
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# Classes (baseado na sua tabela)
classes = ['anxiety', 'depression', 'stress', 'personality disorder', 'bipolar', 'suicidal']

# Lendo os dados do arquivo
with open('resultados_teste_k6.txt', 'r', encoding='utf-8') as file:
    linhas = file.readlines()
    
    # Primeira linha: valores preditos
    y_pred = linhas[0].strip().split(',')
    
    # Segunda linha: valores reais
    y_real = linhas[1].strip().split(',')

print(f"Valores preditos: {len(y_pred)} amostras")
print(f"Valores reais: {len(y_real)} amostras")

# Matriz de confusão
cm = confusion_matrix(y_real, y_pred, labels=classes)

# Visualização
plt.figure(figsize=(16, 12))
sns.heatmap(cm, annot=True, fmt='d', cmap='Blues', 
            xticklabels=classes, yticklabels=classes,
            cbar_kws={'label': 'Quantidade'})

plt.title('Matriz de Confusão - 6 Classes - k=6', fontsize=16, pad=20)
plt.ylabel('Valor Real', fontsize=12)
plt.xlabel('Previsão', fontsize=12)
plt.xticks(rotation=0)
plt.yticks(rotation=0)

# Adicionando métricas básicas
acuracia_global = np.trace(cm) / np.sum(cm)
plt.text(0.5, -0.1, f'Acurácia Global: {acuracia_global:.3f}', 
         transform=plt.gca().transAxes, fontsize=12,
         ha='center', bbox=dict(boxstyle="round,pad=0.3", facecolor="lightgray"))

plt.tight_layout()
plt.show()

# Métricas por classe
print("\n" + "="*70)
print("ANÁLISE DETALHADA POR CLASSE:")
print("="*70)

# Cálculo do total de amostras para acurácia por classe
total_amostras = len(y_real)

dados_classes = []

for i, classe in enumerate(classes):
    tp = cm[i, i]  # True Positive
    fn = np.sum(cm[i, :]) - tp  # False Negative
    fp = np.sum(cm[:, i]) - tp  # False Positive
    tn = total_amostras - (tp + fn + fp)  # True Negative
    
    # Cálculo das métricas
    precisao = tp / (tp + fp) if (tp + fp) > 0 else 0
    recall = tp / (tp + fn) if (tp + fn) > 0 else 0
    f1 = 2 * (precisao * recall) / (precisao + recall) if (precisao + recall) > 0 else 0
    acuracia_classe = (tp + tn) / total_amostras if total_amostras > 0 else 0
    
    # Armazenar para cálculo geral
    dados_classes.append({
        'classe': classe,
        'tp': tp,
        'fp': fp,
        'fn': fn,
        'tn': tn,
        'precisao': precisao,
        'recall': recall,
        'f1': f1,
        'acuracia_classe': acuracia_classe
    })
    
    print(f"\n{classe.upper():<25}")
    print(f"  True Positives: {tp:>4}")
    print(f"  False Negatives: {fn:>3}")
    print(f"  False Positives: {fp:>3}")
    print(f"  True Negatives: {tn:>4}")
    print(f"  Precisão: {precisao:.3f}")
    print(f"  Recall: {recall:.3f}")
    print(f"  F1-Score: {f1:.3f}")
    print(f"  Acurácia: {acuracia_classe:.3f}")

# ===========================================================================
# MÉTRICAS GERAIS (NO GERAL)
# ===========================================================================

print("\n" + "="*70)
print("MÉTRICAS GERAIS (NO GERAL):")
print("="*70)

# Somatórios totais
total_tp = sum([d['tp'] for d in dados_classes])
total_fp = sum([d['fp'] for d in dados_classes])
total_fn = sum([d['fn'] for d in dados_classes])
total_tn = sum([d['tn'] for d in dados_classes])

print(f"\n📊 SOMA DE TODAS AS CLASSES:")
print(f"  Total True Positives:  {total_tp:>5}")
print(f"  Total False Positives: {total_fp:>5}")
print(f"  Total False Negatives: {total_fn:>5}")
print(f"  Total True Negatives:  {total_tn:>5}")

# 1. ACURÁCIA GLOBAL
print(f"\n🎯 1. ACURÁCIA GLOBAL:")
print(f"  {acuracia_global:.3f} ({(acuracia_global*100):.1f}%)")

# 2. PRECISÃO, RECALL e F1-Score MICRO
print(f"\n📈 2. MÉTRICAS MICRO (considera o volume de cada classe):")
precisao_micro = total_tp / (total_tp + total_fp) if (total_tp + total_fp) > 0 else 0
recall_micro = total_tp / (total_tp + total_fn) if (total_tp + total_fn) > 0 else 0
f1_micro = 2 * (precisao_micro * recall_micro) / (precisao_micro + recall_micro) if (precisao_micro + recall_micro) > 0 else 0

print(f"  Precisão Micro: {precisao_micro:.3f}")
print(f"  Recall Micro:    {recall_micro:.3f}")
print(f"  F1-Score Micro:  {f1_micro:.3f}")

# 3. PRECISÃO, RECALL e F1-Score MACRO
print(f"\n⚖️  3. MÉTRICAS MACRO (média simples entre classes):")
precisao_macro = np.mean([d['precisao'] for d in dados_classes])
recall_macro = np.mean([d['recall'] for d in dados_classes])
f1_macro = np.mean([d['f1'] for d in dados_classes])

print(f"  Precisão Macro: {precisao_macro:.3f}")
print(f"  Recall Macro:    {recall_macro:.3f}")
print(f"  F1-Score Macro:  {f1_macro:.3f}")

# 4. PRECISÃO, RECALL e F1-Score PONDERADA
print(f"\n📊 4. MÉTRICAS PONDERADA (média ponderada pelo suporte de cada classe):")

# Calcular suporte de cada classe (número de instâncias reais)
suportes = [d['tp'] + d['fn'] for d in dados_classes]
total_suporte = sum(suportes)

precisao_ponderada = sum([d['precisao'] * suporte for d, suporte in zip(dados_classes, suportes)]) / total_suporte
recall_ponderada = sum([d['recall'] * suporte for d, suporte in zip(dados_classes, suportes)]) / total_suporte
f1_ponderada = sum([d['f1'] * suporte for d, suporte in zip(dados_classes, suportes)]) / total_suporte

print(f"  Precisão Ponderada: {precisao_ponderada:.3f}")
print(f"  Recall Ponderada:    {recall_ponderada:.3f}")
print(f"  F1-Score Ponderada:  {f1_ponderada:.3f}")

# 5. RELATÓRIO COMPLETO DO SKLEARN
print(f"\n🔍 5. RELATÓRIO DE CLASSIFICAÇÃO COMPLETO:")
print(classification_report(y_real, y_pred, labels=classes, target_names=classes))

# 6. RESUMO FINAL
print(f"\n" + "="*70)
print("📋 RESUMO FINAL DO MODELO:")
print("="*70)

print(f"• Acurácia Global:     {acuracia_global:.3f}")
print(f"• F1-Score Micro:      {f1_micro:.3f} (igual à acurácia)")
print(f"• F1-Score Macro:      {f1_macro:.3f} (média balanceada)")
print(f"• F1-Score Ponderada:  {f1_ponderada:.3f} (recomendada para dados desbalanceados)")

# Identificar melhor e pior classe
melhor_classe = max(dados_classes, key=lambda x: x['f1'])
pior_classe = min(dados_classes, key=lambda x: x['f1'])

print(f"\n⭐ MELHOR CLASSE: {melhor_classe['classe'].upper()}")
print(f"  F1-Score: {melhor_classe['f1']:.3f}")

print(f"\n⚠️  CLASSE COM MAIS DIFICULDADE: {pior_classe['classe'].upper()}")
print(f"  F1-Score: {pior_classe['f1']:.3f}")

print(f"\n📈 DISTRIBUIÇÃO DE CLASSES:")
for i, classe in enumerate(classes):
    suporte = suportes[i]
    percentual = (suporte / total_suporte) * 100
    print(f"  {classe:<20}: {suporte:>4} amostras ({(percentual):.1f}%)")