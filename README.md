# Caminhos no grid 7x7 (Assessment Task)

Solução em C++ do problema de contar caminhos num tabuleiro **7x7** que vão do
canto **superior-esquerdo** ao **inferior-esquerdo** passando por **todas as 49
casas exatamente uma vez** (caminho Hamiltoniano), casando com um **padrão** de
48 caracteres que pode conter o coringa `?`.

> Disciplina: APA — Backtracking and Dynamic Programming (PPGPCA).

## Otimizações novas (além das que estão no slide)

O material da disciplina (slide *"Paths in a Grid - Optimizations"*) lista as
otimizações para a versão do problema **sem padrão**: chegada antecipada e a
detecção de divisão do grid (implementadas aqui como Podas 2 e 3). Além delas,
esta solução implementa otimizações **que não estão no slide**:

### 1. Filtro do padrão (Poda 1) — explora o coringa `?` da tarefa

```cpp
if (exigido != '?' && exigido != letra[d]) continue;
```

O slide trata o problema sem padrão; a tarefa introduz o padrão com `?` e
letras fixas. Esta poda usa as letras fixas do próprio padrão para cortar a
busca: em cada passo com letra fixa, a ramificação cai de **4 direções para 1**
— e, como a árvore de busca é multiplicativa, cada letra elimina a subárvore
inteira das outras 3 direções **antes** de explorá-la.

**Impacto medido:** o exemplo do enunciado (com letras fixas) roda em ~14 ms,
contra ~320 ms do pior caso sem nenhuma letra (tudo `?`).

### 2. Atalho do último passo — detecta entradas impossíveis sem buscar

```cpp
if (ultimo == 'U' || ultimo == 'R') { cout << 0; return; }
```

O destino `(6,0)` só pode ser alcançado no 48º movimento vindo de cima (`D`)
ou da direita (`L`). Se o padrão **fixa** `U` ou `R` na última posição, nenhum
caminho pode terminar no destino — a resposta é `0` imediata, **sem executar a
busca** (resposta em ~0 ms).

### 3. Forma incremental da poda de beco — refinamento da implementação

O slide descreve a ideia conceitualmente ("o movimento parte o grid em duas
regiões"). A implementação aqui refina isso em dois pontos próprios:

- **Checagem por grau de saídas** — em vez de detectar "parede + virada",
  verifica diretamente a consequência: alguma casa vazia ficou com menos de
  2 saídas (ou o destino com menos de 1)? Isso cobre os casos 3 e 4 do slide
  com um único teste simples.
- **Verificação incremental** — a cada movimento, só as **vizinhas da casa
  que acabou de ser deixada** podem ter perdido uma saída; então a checagem
  olha no máximo **4 casas** por passo, em vez de varrer as 49.

**Impacto medido:** a versão incremental derrubou o pior caso de ~970 ms para
~220 ms — sem ela, o limite de 1 segundo ficaria apertado.

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

## Arquivos

| Arquivo | Conteúdo |
|---|---|
| `caminhos_simples.cpp` | Código-fonte comentado. |
| `entrada.txt` | Exemplo de padrão de entrada. |
| `DOCUMENTACAO.md` | Diagramas e explicação por função. |
| `beco_sem_saida.png` | Figura ilustrando a poda de beco sem saída. |
