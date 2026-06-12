# Caminhos no grid 7x7 (Assessment Task)

Solução em C++ do problema de contar caminhos num tabuleiro **7x7** que vão do
canto **superior-esquerdo** ao **inferior-esquerdo** passando por **todas as 49
casas exatamente uma vez** (caminho Hamiltoniano), casando com um **padrão** de
48 caracteres que pode conter o coringa `?`.

> Disciplina: APA — Backtracking and Dynamic Programming (PPGPCA).

## Como compilar

```bash
g++ -std=c++17 -O2 -static -o caminhos_simples caminhos_simples.cpp
```

## Como usar

```bash
# padrao direto
caminhos_simples "??????R??????U??????????????????????????LD????D?"

# lendo de um arquivo .txt (a 1a linha nao vazia)
caminhos_simples entrada.txt

# pela entrada padrao (pipe)
echo "????????????????????????????????????????????????" | caminhos_simples

# autoteste (confere 88418 e 201)
caminhos_simples teste
```

- **Entrada:** uma linha com 48 caracteres em `{?, D, U, L, R}`.
- **Saída:** um inteiro = quantos caminhos válidos casam com o padrão.
- Exemplo: `??????R??????U??????????????????????????LD????D?` → **201**.
- Padrão só com `?` → **88418** (total de caminhos).

## A solução em resumo

- **Backtracking** (busca em profundidade com marca / desce / desmarca).
- **3 podas** que mantêm o tempo abaixo de 1 segundo:
  1. **Filtro do padrão** — só tenta a direção que o padrão permite.
  2. **Chegada antecipada** — se chega ao destino antes da hora, abandona.
  3. **Beco sem saída** — se um movimento isola alguma casa, abandona o ramo
     (cada casa vazia precisa de ≥2 saídas; o destino, ≥1).
- **Atalho:** se o padrão fixa `U` ou `R` no último passo, a resposta é `0`
  direto (impossível chegar ao destino).

Detalhes da lógica em [`DOCUMENTACAO.md`](DOCUMENTACAO.md).

## Arquivos

| Arquivo | Conteúdo |
|---|---|
| `caminhos_simples.cpp` | Código-fonte comentado. |
| `entrada.txt` | Exemplo de padrão de entrada. |
| `DOCUMENTACAO.md` | Diagramas e explicação por função. |
| `beco_sem_saida.png` | Figura ilustrando a poda de beco sem saída. |
