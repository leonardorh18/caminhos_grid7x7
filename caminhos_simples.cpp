

#include <iostream>  // cout (saida), cerr (mensagens), cin (entrada padrao)
#include <string>    // tipo string, usado para o padrao e nomes de arquivo
#include <fstream>   // ifstream: ler o padrao de um arquivo .txt
#include <cctype>    // tolower: reconhecer extensao .TXT, .Txt, etc.
#include <chrono>    // medir o tempo de execucao
using namespace std; // evita escrever "std::" antes de cout, string, etc.

// ----------------------------------------------------------------------------
//  Constantes e variaveis globais (globais para a recursao ficar simples)
// ----------------------------------------------------------------------------
const int N = 7;            // lado do tabuleiro: grid 7x7
const int PASSOS = 48;      // um caminho tem 48 movimentos (49 casas - 1)

bool   visitado[N][N];      // visitado[l][c] = true se ja passamos pela casa (l,c)
string padrao;              // os 48 caracteres da entrada (?, D, U, L, R)
long long total = 0;        // contador: quantos caminhos validos achamos

// As quatro direcoes, alinhadas por indice com o vetor "letra" abaixo.
// Indice d:   0 -> D,   1 -> U,   2 -> L,   3 -> R
int  dLinha[4]  = { +1, -1,  0,  0 };   // quanto a LINHA muda em cada direcao
int  dColuna[4] = {  0,  0, -1, +1 };   // quanto a COLUNA muda em cada direcao
char letra[4]   = { 'D','U','L','R' };  // a letra correspondente a cada direcao

// Casa de destino: canto inferior-esquerdo (linha 6, coluna 0).
const int L_DESTINO = 6;    // linha do destino
const int C_DESTINO = 0;    // coluna do destino

// ----------------------------------------------------------------------------
//  Auxiliares de leitura de entrada
// ----------------------------------------------------------------------------
//  Remove espacos, tabs e quebras de linha das pontas de uma string. Assim a
//  entrada funciona mesmo que o arquivo tenha um "enter" no final, por exemplo.
void limparPontas(string& s) {
    // Enquanto o ULTIMO caractere for espaco/tab/quebra de linha, remove-o.
    while (!s.empty() &&                                   // string nao vazia E
           (s.back() == '\n' || s.back() == '\r' ||        // ultimo char e enter
            s.back() == ' '  || s.back() == '\t'))         //   ou espaco/tab
        s.pop_back();                                      // remove o ultimo char
    // Enquanto o PRIMEIRO caractere for espaco/tab, remove-o.
    while (!s.empty() &&                                   // string nao vazia E
           (s.front() == ' ' || s.front() == '\t'))        // primeiro char e espaco/tab
        s.erase(s.begin());                                // remove o primeiro char
}

//  Diz se um nome de arquivo termina em ".txt" (para o programa decidir se deve
//  ler de arquivo ou usar o texto recebido diretamente como padrao).
bool terminaEmTxt(const string& nome) {
    if (nome.size() < 4) return false;          // nome curto demais p/ ter ".txt"
    string fim = nome.substr(nome.size() - 4);  // pega os 4 ultimos caracteres
    for (char& c : fim) c = (char)tolower(c);   // poe em minusculo (aceita .TXT, .Txt)
    return fim == ".txt";                       // verdadeiro se os 4 finais sao ".txt"
}

//  Le a PRIMEIRA linha (nao vazia) de um arquivo de texto e devolve em `destino`.
//  Retorna true se conseguiu abrir o arquivo; false se o arquivo nao existe.
bool lerPadraoDeArquivo(const string& caminhoArquivo, string& destino) {
    ifstream arquivo(caminhoArquivo);           // tenta abrir o arquivo para leitura
    if (!arquivo.is_open()) return false;       // nao abriu (arquivo nao existe) -> falha

    string linha;                               // guarda cada linha lida
    while (getline(arquivo, linha)) {           // le linha por linha ate o fim
        limparPontas(linha);                    // tira espacos/enter das pontas
        if (!linha.empty()) {                   // achou a 1a linha com conteudo
            destino = linha;                    // devolve essa linha como padrao
            return true;                        // sucesso
        }
    }
    destino = "";                               // arquivo vazio ou so com linhas em branco
    return true;                                // abriu, mas nao havia conteudo util
}


int contarSaidas(int l, int c, int headL, int headC) {
    int saidas = 0;                             // quantas portas livres a casa (l,c) tem
    for (int d = 0; d < 4; d++) {               // olha as 4 direcoes vizinhas
        int nl = l + dLinha[d];                 // linha do vizinho na direcao d
        int nc = c + dColuna[d];                // coluna do vizinho na direcao d
        if (nl < 0 || nl >= N || nc < 0 || nc >= N) continue;   // vizinho fora do tabuleiro -> ignora
        bool ehHead = (nl == headL && nc == headC);             // o vizinho e a casa atual (head)?
        if (!visitado[nl][nc] || ehHead) {      // vizinho livre OU e o head -> conta como porta
            saidas++;                           // achou mais uma porta
            if (saidas >= 2) return 2;          // ja temos >=2: nao precisa contar o resto
        }
    }
    return saidas;                              // devolve 0, 1 (ou 2, tratado acima)
}

// ----------------------------------------------------------------------------
//  PODA 3 - "beco sem saida" 
// ----------------------------------------------------------------------------

bool criouBecoSemSaida(int deixadaL, int deixadaC, int headL, int headC) {
    for (int d = 0; d < 4; d++) {               // percorre as vizinhas da casa que deixamos
        int nl = deixadaL + dLinha[d];          // linha da vizinha na direcao d
        int nc = deixadaC + dColuna[d];         // coluna da vizinha na direcao d
        if (nl < 0 || nl >= N || nc < 0 || nc >= N) continue;   // vizinha fora do tabuleiro -> ignora
        if (visitado[nl][nc]) continue;         // so analisamos casas VAZIAS (visitadas nao viram beco)
        int saidas = contarSaidas(nl, nc, headL, headC);   // quantas portas essa casa vazia ainda tem
        if (nl == L_DESTINO && nc == C_DESTINO) {          // se a casa vazia e o DESTINO...
            if (saidas < 1) return true;        // ...precisa de >=1 porta; senao esta isolado -> beco
        } else {                                // se e uma casa comum...
            if (saidas < 2) return true;        // ...precisa de >=2 (entrar e sair); senao -> beco
        }
    }
    return false;                               // nenhuma vizinha virou beco: movimento e seguro
}

// ----------------------------------------------------------------------------
//  A BUSCA (backtracking): tenta, casa a casa, todos os caminhos possiveis.
//    linha, coluna = onde estamos agora (head)
//    passo         = quantos movimentos ja demos (0 ate 48)
// ----------------------------------------------------------------------------
void buscar(int linha, int coluna, int passo) {
    // ---- Caso base: ja demos os 48 passos (caminho completo) ----
    if (passo == PASSOS) {                       // chegamos ao fim do caminho?
        // Como nunca repetimos casa, visitamos as 49; so falta terminar no destino.
        if (linha == L_DESTINO && coluna == C_DESTINO) total++;  // terminou no destino -> conta +1
        return;                                  // volta (fim deste caminho)
    }

    // ---- PODA 2 (chegada antecipada) = otimizacao 2 
    // Se ja estamos no destino mas ainda faltam passos, este caminho nao serve
    // (teriamos de sair do destino e voltar, repetindo casa) Abandona o ramo.
    if (linha == L_DESTINO && coluna == C_DESTINO) return;

    char exigido = padrao[passo];                // letra que o padrao exige NESTE passo

    // ---- Tenta as quatro direcoes possiveis (D, U, L, R) ----
    for (int d = 0; d < 4; d++) {

        if (exigido != '?' && exigido != letra[d]) continue;   // direcao proibida pelo padrao -> pula

        int nl = linha + dLinha[d];              // linha da casa de destino do movimento
        int nc = coluna + dColuna[d];            // coluna da casa de destino do movimento

        if (nl < 0 || nl >= N || nc < 0 || nc >= N) continue;  // saiu do tabuleiro -> pula
        if (visitado[nl][nc]) continue;          // casa ja visitada (repetiria) -> pula

        // Marca a casa nova; aplica a PODA 3 beco sem saida
        //  olhando as vizinhas da casa que estamos deixando (linha,
        // coluna); so desce na recursao se o movimento nao criou um beco.
        visitado[nl][nc] = true;                 // MARCA: ocupa a casa nova
        if (!criouBecoSemSaida(linha, coluna, nl, nc))  // PODA 3: o movimento e seguro?
            buscar(nl, nc, passo + 1);           // DESCE: explora a partir da casa nova
        visitado[nl][nc] = false;                // DESMARCA: desfaz (este e o "backtracking")
    }
}

// ----------------------------------------------------------------------------
//  Resolve para um padrao dado e devolve a contagem.
// ----------------------------------------------------------------------------
long long resolver(const string& p) {
    padrao = p;                                 // guarda o padrao na variavel global
    total = 0;                                  // zera o contador de caminhos
    for (int l = 0; l < N; l++)                 // para cada linha do tabuleiro
        for (int c = 0; c < N; c++)             //   para cada coluna
            visitado[l][c] = false;             //     marca a casa como NAO visitada

    visitado[0][0] = true;   // comecamos no canto superior-esquerdo -> ja visitada
    buscar(0, 0, 0);         // dispara a busca: casa (0,0), passo 0
    return total;            // devolve quantos caminhos validos foram contados
}

// ----------------------------------------------------------------------------
//  Modo "teste": confere os dois resultados conhecidos e mostra o tempo
// ----------------------------------------------------------------------------
void rodarTeste() {
    string todos(PASSOS, '?');                  // padrao com 48 '?' (conta TODOS os caminhos)
    string exemplo = "??????R??????U??????????????????????????LD????D?";  // exemplo do enunciado

    auto t0 = chrono::high_resolution_clock::now();   // marca o tempo antes do caso 1
    long long a = resolver(todos);                    // resolve o caso "tudo ?"
    auto t1 = chrono::high_resolution_clock::now();   // marca o tempo entre os casos
    long long b = resolver(exemplo);                  // resolve o caso do exemplo
    auto t2 = chrono::high_resolution_clock::now();   // marca o tempo depois do caso 2

    double msTodos   = chrono::duration<double, milli>(t1 - t0).count();  // tempo do caso 1 (ms)
    double msExemplo = chrono::duration<double, milli>(t2 - t1).count();  // tempo do caso 2 (ms)

    // Mostra cada resultado ao lado do valor esperado e do tempo medido
    cout << "Tudo '?'  -> " << a << "  (esperado 88418)   [" << msTodos   << " ms]\n";
    cout << "Exemplo   -> " << b << "  (esperado 201)     [" << msExemplo << " ms]\n";
    // Confirma se os dois resultados batem com os valores de referencia
    cout << (a == 88418 && b == 201 ? "TODOS OS TESTES PASSARAM" : "FALHOU") << "\n";
}

// ----------------------------------------------------------------------------
//  Programa principal le o padrao e imprime a contagem
// ----------------------------------------------------------------------------
int main(int argc, char** argv) {
    // ---- Modo teste: "caminhos_simples teste" ----
    if (argc >= 2 && string(argv[1]) == "teste") {  // 1o argumento e "teste"?
        rodarTeste();                               // roda os testes de referencia
        return 0;                                   // encerra o programa
    }

    // ---- Le o padrao. O argumento pode ser: ----
    //   - um arquivo .txt   -> le o padrao da primeira linha do arquivo
    //   - o proprio padrao  -> usa o texto recebido diretamente
    //   - nada              -> le da entrada padrao (ex.: por pipe "echo ... |")
    string entrada;                                 // aqui guardamos o padrao a usar
    if (argc >= 2) {                                // o usuario passou um argumento?
        string arg = argv[1];                       // o argumento (padrao ou nome de arquivo)
        if (terminaEmTxt(arg)) {                    // termina em ".txt"? -> e um arquivo
            if (!lerPadraoDeArquivo(arg, entrada)) {// tenta ler o padrao do arquivo
                cerr << "Erro: nao consegui abrir o arquivo '" << arg << "'.\n";  // falhou
                return 1;                           // encerra com codigo de erro
            }
        } else {                                    // nao e arquivo...
            entrada = arg;                          // ...entao o argumento e o proprio padrao
        }
    } else {                                        // nenhum argumento...
        getline(cin, entrada);                      // ...le o padrao da entrada padrao (teclado/pipe)
    }

    limparPontas(entrada);                          // remove espacos/quebras das pontas

    if ((int)entrada.size() != PASSOS) {            // o padrao tem exatamente 48 caracteres?
        cerr << "Erro: o padrao deve ter exatamente 48 caracteres (recebi "
             << entrada.size() << ").\n";           // avisa o tamanho recebido
        return 1;                                   // encerra com codigo de erro
    }

    // ---- ATALHO: ultimo passo impossivel de chegar ao destino ----
    // padrao FIXA o ultimo passo em 'U' ou 'R', nenhum caminho pode terminar no destino 
    char ultimo = entrada[PASSOS - 1];
    if (ultimo == 'U' || ultimo == 'R') {
        cout << 0 << "\n";                          // resposta imediata
        cerr << "Tempo de execucao: 0 ms (atalho: ultimo passo nunca chega ao destino)\n";
        return 0;
    }

    // ---- Mede quanto tempo a busca leva para achar/contar todos os caminhos ----
    auto t0 = chrono::high_resolution_clock::now(); // tempo antes de resolver
    long long resultado = resolver(entrada);        // executa a contagem (backtracking)
    auto t1 = chrono::high_resolution_clock::now(); // tempo depois de resolver
    double ms = chrono::duration<double, milli>(t1 - t0).count();  // duracao em milissegundos

    // A SAIDA oficial (o inteiro) vai para stdout, como o enunciado pede
    cout << resultado << "\n";                       // imprime o numero de caminhos
   
   
    cerr << "Tempo de execucao: " << ms << " ms\n"; // imprime o tempo medido
    return 0;                                        // encerra com sucesso
}
