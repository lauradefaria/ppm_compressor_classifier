from sklearn.metrics import confusion_matrix
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np

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
print(f"\nPrimeiros 10 preditos: {y_pred[:10]}")
print(f"Primeiros 10 reais: {y_real[:10]}")

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
acuracia = np.trace(cm) / np.sum(cm)
plt.text(0.5, -0.1, f'Acurácia: {acuracia:.3f}', 
         transform=plt.gca().transAxes, fontsize=12,
         ha='center', bbox=dict(boxstyle="round,pad=0.3", facecolor="lightgray"))

plt.tight_layout()
plt.show()

# Métricas por classe
print("\n" + "="*50)
print("ANÁLISE DETALHADA:")
print("="*50)

for i, classe in enumerate(classes):
    tp = cm[i, i]  # True Positive
    fn = np.sum(cm[i, :]) - tp  # False Negative
    fp = np.sum(cm[:, i]) - tp  # False Positive
    
    precisao = tp / (tp + fp) if (tp + fp) > 0 else 0
    recall = tp / (tp + fn) if (tp + fn) > 0 else 0
    f1 = 2 * (precisao * recall) / (precisao + recall) if (precisao + recall) > 0 else 0
    
    print(f"\n{classe.upper():<20}")
    print(f"  True Positives: {tp:>4}")
    print(f"  False Negatives: {fn:>3}")
    print(f"  False Positives: {fp:>3}")
    print(f"  Precisão: {precisao:.3f}")
    print(f"  Recall: {recall:.3f}")
    print(f"  F1-Score: {f1:.3f}")