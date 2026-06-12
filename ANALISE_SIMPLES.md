# Versão simples (para iniciantes) — `caminhos_simples.cpp`

Solução do **Assessment Task** (slide de rodapé "53") escrita para ser **fácil de
entender**: usa só `for`, `if`, recursão e uma matriz `bool[7][7]`. Sem bitmask,
sem `__builtin_popcount`, sem flood-fill. Mesmo assim roda **abaixo de 1 s** no
pior caso possível.

## 1. O problema, em uma frase

Contar quantos caminhos de **48 passos** (`D`/`U`/`L`/`R`) saem de `(0,0)` e
terminam em `(6,0)` passando por **todas as 49 casas** do tabuleiro 7×7, casando
com um **padrão** dado (onde `?` = qualquer direção).

- Padrão só com `?` → **88418** (total de caminhos).
- Exemplo do enunciado → **201**.

> Por que "48 passos = todas as casas"? O tabuleiro tem 7×7 = 49 casas. Começamos
> em 1 casa e damos 48 passos sem repetir nenhuma: 1 + 48 = 49 = todas.

## 2. A ideia base: backtracking

Backtracking é "tentativa e erro organizado":
1. A partir da casa atual, **tente** cada direção válida.
2. Marque a casa nova como visitada e **continue** dali (recursão).
3. Ao voltar, **desmarque** a casa (desfaz a tentativa) e tente a próxima.

Ao chegar aos 48 passos, se terminamos no destino, somamos 1 ao total.

## 3. As otimizações (cada uma explicada e **medida**)

Eu escrevi primeiro a versão mais simples possível e **cronometrei**. Sem poda
ela é inviável. Cada otimização foi adicionada porque os números pediram. Todos
os tempos são do mesmo PC, compilando com `g++ -O2`. O caso **Tudo-`?`** é o pior
possível (nenhuma letra fixa para ajudar); o **Exemplo** é uma entrada realista.

| Versão | Tudo-`?` (pior caso) | Exemplo | Resposta correta? |
|---|---|---|---|
| **Sem nenhuma poda** (backtracking puro) | **105 228 ms** (~105 s) ❌ | 6 681 ms ❌ | sim, mas lenta demais |
| **+ Otim. 3 varrendo o tabuleiro inteiro** a cada passo | **970 ms** ✓ | 60 ms ✓ | sim |
| **+ Otim. 3 incremental** (só as casas que mudaram) — **versão final** | **635 ms** ✓ | **41 ms** ✓ | sim |

(As otimizações 1 e 2 estão presentes em todas as versões com poda.)

### Otimização 1 — Filtro do padrão
No passo `i`, se o padrão fixa uma letra (ex.: `R`), **só tentamos essa
direção**; as outras 3 são ignoradas na hora. Só quando o caractere é `?`
tentamos as 4. Cada caractere fixo reduz a ramificação de 4 → 1 naquele passo.
É por isso que o **exemplo** (cheio de letras) é muito mais rápido que o tudo-`?`.

```cpp
if (exigido != '?' && exigido != letra[d]) continue;
```

### Otimização 2 — Chegada antecipada ao destino
Se chegarmos ao destino `(6,0)` **antes** do 48º passo, não adianta continuar:
para seguir teríamos de sair do destino e depois voltar, repetindo uma casa.
Então abandonamos esse ramo imediatamente.

```cpp
if (linha == L_DESTINO && coluna == C_DESTINO) return;   // e ainda faltam passos
```

### Otimização 3 — Poda de "beco sem saída" (a mais importante)
É a que derruba o tempo de **~105 s para menos de 1 s**.

**Ideia:** para o caminho cobrir todas as casas, cada casa vazia precisa ser
*atravessada* — entrar por uma porta e sair por outra. Logo **toda casa vazia
precisa de pelo menos 2 saídas** (portas livres). A única exceção é o **destino**,
que é ponta do caminho e precisa de pelo menos **1**. Se alguma casa vazia ficar
com menos que isso, virou um beco: nunca completaremos o caminho → abandonamos.

> Detalhe importante de contagem: a casa onde estamos agora (o *head*) conta como
> uma porta para as casas vazias vizinhas dela (o caminho ainda vai sair de lá).
> Sem contar isso, a poda corta caminhos válidos e a resposta fica errada.

**O que deixa rápido (a parte incremental):** quando saímos de uma casa, as únicas
casas cujo número de saídas pode ter **diminuído** são **as vizinhas da casa que
acabamos de deixar** (elas perderam aquela casa como porta). Então basta conferir
essas (no máximo 4), em vez de varrer as 49 casas do tabuleiro a cada passo. Isso
levou o pior caso de **970 ms → 635 ms**, mantendo a contagem exata.

```cpp
bool criouBecoSemSaida(int deixadaL, int deixadaC, int headL, int headC) {
    for (int d = 0; d < 4; d++) {                 // so as vizinhas da casa deixada
        int nl = deixadaL + dLinha[d], nc = deixadaC + dColuna[d];
        if (fora do tabuleiro || visitado[nl][nc]) continue;
        int saidas = contarSaidas(nl, nc, headL, headC);   // head conta como porta
        if (eh o destino) { if (saidas < 1) return true; }
        else              { if (saidas < 2) return true; }
    }
    return false;
}
```

### Bônus — `-O2` na compilação
A flag `-O2` deixa o mesmo código várias vezes mais rápido, sem mudar uma linha.
É "otimização de graça".

## 4. Por que **não** usei Programação Dinâmica (DP)?
Os slides finais do material (Backtracking vs. DP) ajudam a escolher: DP precisa
de **subproblemas que se repetem** e um **estado pequeno** que caiba numa tabela.
Aqui o "estado" teria de incluir **o conjunto de casas já visitadas** (para não
repetir), e isso tem `2^49` combinações — não cabe em tabela. Por isso a
ferramenta certa é **backtracking com podas** (a ideia de *Branch and Bound* do
material): cortar cedo os ramos que comprovadamente não levam a solução.

## 5. Resultado final (medido)

```
Tudo '?'  -> 88418  (esperado 88418)   [635 ms]
Exemplo   -> 201    (esperado 201)     [41 ms]
TODOS OS TESTES PASSARAM
```

Requisito de **< 1 segundo cumprido** mesmo no pior caso (tudo-`?`); entradas
realistas (com letras) rodam em dezenas de milissegundos.

## 6. Como compilar e rodar

```powershell
g++ -std=c++17 -O2 -static -o caminhos_simples caminhos_simples.cpp

caminhos_simples "??????R??????U??????????????????????????LD????D?"   # -> 201
caminhos_simples entrada.txt          # le o padrao de um arquivo .txt
echo SEU_PADRAO | caminhos_simples
caminhos_simples teste     # confere 88418 e 201 e mostra os tempos
```

> **Por que `-static`?** Ele embute as bibliotecas do compilador dentro do
> `.exe`. Assim o programa roda em qualquer PC Windows **sem precisar das DLLs**
> do MinGW instaladas/no PATH. Sem essa flag, o executável pode falhar ao abrir
> arquivos se as DLLs do runtime não forem encontradas.

### Entradas personalizadas (o que o enunciado pede)

O programa lê **exatamente** o formato do enunciado: uma linha com **48
caracteres** em `{?, D, U, L, R}` e imprime **um inteiro** (quantos caminhos
casam). Há **três formas** de fornecer a entrada:

```powershell
# 1) padrao direto como argumento
caminhos_simples "DD??LL??RR??????????????????????????????????????"

# 2) de um arquivo .txt (le a 1a linha nao vazia do arquivo)
caminhos_simples entrada.txt

# 3) pela entrada padrao (pipe)
echo "????????????????????????????????????????????????" | caminhos_simples
```

> **Como o programa sabe se é arquivo ou padrão?** Se o argumento termina em
> `.txt`, ele **lê o arquivo**; caso contrário, trata o texto como o próprio
> padrão.

O arquivo de exemplo `entrada.txt` (um padrão com `?`) acompanha a solução.

- Se o padrão tem letras fixas, o número sai menor (mais restrito).
- Se uma sequência **concreta** (sem `?`) não leva a um caminho válido, a
  resposta é `0` (nenhum caminho casa).
- Se a entrada não tiver 48 caracteres, o programa avisa e não executa.
- Se o arquivo `.txt` não existir, o programa avisa: `Erro: nao consegui abrir
  o arquivo '...'`.

> Observação: um caminho tem **48 passos** (não 49). São 49 *casas* e 48
> *movimentos* entre elas.

## 7. Os arquivos de solução nesta pasta

| Arquivo | Para quê |
|---|---|
| `caminhos_simples.cpp` | Código-fonte comentado (este documento). |
| `caminhos_simples.exe` | Binário estático já compilado. |
| `entrada.txt` | Exemplo de padrão (com `?`). |
| `DOCUMENTACAO.md` | Diagramas e explicação por função. |
| `ANALISE_SIMPLES.md` | Este documento (otimizações e medições). |

O programa dá exatamente **88418** (tudo `?`) e **201** (exemplo do enunciado).
