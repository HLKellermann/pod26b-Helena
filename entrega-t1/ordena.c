// algoritmos de ordenação de dados
// disciplina de pesquisa e ordenação

// {{{1 includes
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

// {{{1 tipos de dados

typedef int chave_t;   // tipo da chave
#define TAM_DADOS 20   // número de bytes além da chave em cada dado

typedef struct {
  chave_t chave;
  char dados[TAM_DADOS];
} dado_t;

// contadores globais
long n_compara;
long n_copia;


// {{{1 funções auxiliares

// copia o dado apontado por origem para o local apontado por destino
void copia(dado_t *destino, dado_t *origem)
{
  n_copia++;
  *destino = *origem;
}

// troca o dado na posição i1 do vetor v
//   pelo dado na posição i2
void troca(dado_t v[], int i1, int i2)
{
  dado_t salvo;
  copia(&salvo, &v[i1]);
  copia(&v[i1], &v[i2]);
  copia(&v[i2], &salvo);
}

// diz se está ok o dado apontado por p1
//   estar antes do dado apontado por p2
bool em_ordem(dado_t *p1, dado_t *p2)
{
  n_compara++;
  return p1->chave <= p2->chave;
}

// diz se está ok o dado na posição i1 do vetor v
//   estar antes do dado na posição i2
bool em_ordem_v(dado_t v[], int i1, int i2)
{
  return em_ordem(&v[i1], &v[i2]);
}

// diz se um vetor está ordenado ou não
bool ordenado(int n, dado_t v[n])
{
  for (int i = 0; i < n-1; i++) {
    if (!em_ordem_v(v, i, i + 1)) return false;
  }
  return true;
}

typedef enum {
  crescente,
  aleatorio,
  decrescente,
  n_ordem
} ordem_t;

// preenche o vetor v com n dados, segundo a ordem dada
void preenche_vetor(int n, dado_t v[n], ordem_t ordem)
{
  switch (ordem) {
    case crescente:
      for (int i = 0; i < n; i++) {
        v[i].chave = i;
      }
      break;
    case decrescente:
      for (int i = 0; i < n; i++) {
        v[i].chave = n - i;
      }
      break;
    case aleatorio:
      for (int i = 0; i < n; i++) {
        v[i].chave = i;
      }
      srand(0); // para ficar igual entre os algoritmos
      for (int i = 0; i < n; i++) {
        troca(v, rand() % n, rand() % n);
      }
      break;
    default:
      break;
  }
}

// {{{1 algoritmos de ordenação

// {{{2 ordenação por bolha
//
// compara cada elemento com seu próximo vizinho, e se não estiverem
//   na ordem certa, troca um pelo outro
// quando terminar, é certo que o maior elemento está no final do vetor,
//   mas como cada elemento só volta no máximo uma posição, tem que repetir.
// repetindo n-1 vezes se tem certeza que está ordenado.

void ordena_bolha(int n, dado_t v[n])
{
  for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < n - 1; j++) {
      if (!em_ordem_v(v, j, j + 1)) troca(v, j, j + 1);
    }
  }
}

// como em uma passagem se garante que o maior elemento foi para o final,
//   dá para otimizar, não testando esse último elemento na próxima vez
void ordena_bolha2(int n, dado_t v[n])
{
  for (int i = n; i > 1; i--) {
    for (int j = 0; j < i - 1; j++) {
      if (!em_ordem_v(v, j, j + 1)) troca(v, j, j + 1);
    }
  }
}

// se fizer uma passagem inteira e não precisar realizar nenhuma troca,
//   já está em ordem, não precisa continuar
void ordena_bolha3(int n, dado_t v[n])
{
  for (int i = n; i > 1; i--) {
    bool trocou = false;
    for (int j = 0; j < i - 1; j++) {
      if (!em_ordem_v(v, j, j + 1)) {
        troca(v, j, j + 1);
        trocou = true;
      }
    }
    if (!trocou) break;
  }
}

// a posição onde foi feita a última troca é a que contém o maior elemento
//   de uma passada, não precisa olhar os elementos além dessa posição
//   na passada seguinte
void ordena_bolha4(int n, dado_t v[n])
{
  int pos_troca;
  for (int i = n; i > 1; i = pos_troca) {
    // antes de i não estão ordenados
    pos_troca = 0;
    for (int j = 0; j < i - 1; j++) {
      if (!em_ordem_v(v, j, j + 1)) {
        troca(v, j, j + 1);
        pos_troca = j + 1;
      }
    }
  }
}

// {{{2 ordenação por seleção
//
// remove cada dado do vetor não ordenado para o final de um vetor
//   inicialmente vazio. A cada vez, retira o menor dado do vetor não
//   ordenado. Usa o início do próprio vetor para manter os dados
//   ordenados.
void ordena_selecao(int n, dado_t v[n])
{
  // elementos antes de i já estão ordenados,
  //   de i em diante não estão ordenados e são
  //   todos maiores que os ordenados
  for (int i = 0; i < n - 1; i++) {
    // procura onde está o menor elemento à partir da posição i
    int pos_menor = i;
    for (int j = i + 1; j < n; j++) {
      if (!em_ordem_v(v, pos_menor, j)) {
        pos_menor = j;
      }
    }
    // troca o menor elemento com o que está na posição i,
    //   se já não estiver aí
    if (pos_menor != i) troca(v, pos_menor, i);
  }
}

// {{{2 ordenação por inserção
//
// insere cada elemento do vetor a ordenar em um vetor inicialmente
//   vazio, que é mantido sempre ordenado, fazendo a inserção a cada
//   vez na posição correta.
// usa as posições iniciais do vetor para conter o vetor ordenado e
//   as posições finais para os dados ainda não ordenados.

void ordena_insercao(int n, dado_t v[n])
{
  // dados antes de i estão ordenados; de i em diante não
  for (int i = 1; i < n; i++) {
    // o dado na posição i é o próximo a ser inserido, salva ele
    dado_t salvo;
    copia(&salvo, &v[i]);
    // avança os dados antes de i que são maiores que o dado a inserir
    int pos = i;
    while (pos > 0 && !em_ordem(&v[pos - 1], &salvo)) {
      copia(&v[pos], &v[pos - 1]);
      pos--;
    }
    // copia o dado salvo para sua posição (se já não tiver lá)
    if (pos != i) copia(&v[pos], &salvo);
  }
}

// {{{2 ordenação shell
//
// Uma variação da ordenação por inserção.
// Aplica a ordenação por inserção em dados que estão a
//   uma certa distância entre si, e usa distâncias cada
//   vez menores, até 1.
void ordena_shell(int n, dado_t v[n])
{
  // as distâncias a considerar (devem ter trabalhado bastante
  //   para chegar a esses números...)
  int dists[] = { 701, 301, 132, 57, 23, 10, 4, 1, -1 };
  for (int idist = 0; dists[idist] > 0; idist++) {
    int dist = dists[idist];
    for (int i = dist; i < n; i++) {
      // o dado na posição i é o próximo a ser inserido, salva ele
      dado_t salvo;
      copia(&salvo, &v[i]);
      // avança os dados antes de i que são maiores que o dado a inserir
      int pos = i;
      while (pos > dist - 1 && !em_ordem(&v[pos - dist], &salvo)) {
        copia(&v[pos], &v[pos - dist]);
        pos -= dist;
      }
      // copia o dado salvo para sua posição (se já não tiver lá)
      if (pos != i) copia(&v[pos], &salvo);
    }
  }
}

// {{{2 ordenação heap
//
// como ordenação por seleção, divide o vetor em duas partes, no início
//   os dados ainda não ordenados e depois os dados já ordenados.
// no início a primeira parte é o vetor inteiro e a segunda é vazia.
// a cada passo, escolhe o maior elemento da primeira parte e coloca
//   no final da parte não ordenada do vetor.
// esse elemento que passa a não mais pertencer à parte não ordenada,
//   que é diminuída de um elemento e passa a ser o primeiro da parte
//   ordenada, que é acrescida de um elemento.
// ao final de n passos, a parte não ordenada fica vazia e a parte
//   ordenada toma todo o vetor.
// o que muda em relação ao algoritmo de seleção é a forma como o
//   maior elemento é encontrado: em vez de uma busca linear, a parte
//   não ordenada do vetor é organizada como uma lista de prioridade,
//   implementada como uma árvore heap.


// funções auxiliares
void heap_ajeita(int n, dado_t v[n], int i);
void heap_constroi(int n, dado_t v[n]);

void ordena_heap(int n, dado_t v[n])
{
  // organiza os dados em v na ordem de uma árvore heap
  heap_constroi(n, v);

  // v contém uma árvore heap, com o maior elemento em v[0].
  // A cada passo do laço, v[p] contém o último elemento da heap,
  //   e a partir da posição p+1, os valores em v são todos não maiores
  //   que os valores na heap, e estão em ordem crescente.
  // Esse elemento em v[0] é o maior da heap, e deve ser o próximo a
  //   ser colocado na parte ordenada do vetor (no seu início, em v[p]).
  // O valor que está no final do heap (em v[p]) é colocado em v[0]
  //   (danificando a heap).
  // A heap é então rearranjada para que volte a ser uma heap e contenha
  //   seu maior elemento em v[0].
  // Prossegue assim até passar todos os elementos da heap para o final
  //   do vetor.
  for (int p = n-1; p > 0; p--) {
    // a heap tem p+1 elementos, troca v[0] (o maior elemento)
    //   por v[p] (o final do vetor com a heap / início do vetor ordenado)
    troca(v, 0, p);
    // a heap agora tem p elementos, as subárvores são heap, mas
    //   a raiz talvez não -> ajeita a raiz
    heap_ajeita(p, v, 0);
  }
}

// no vetor n, os nós abaixo de i contêm árvores heap, mas talvez o nó i
//   não seja maior que seus filhos.
// reorganiza a árvore à partir de i para que seja uma heap.
void heap_ajeita(int n, dado_t v[n], int i)
{
  // posição dos filhos de i
  int esq = 2 * i + 1;
  int dir = esq + 1;
  // se o nó i não tem filho esquerdo, também não tem filho direito,
  //   e o nó i é uma árvore heap
  if (esq >= n) return;

  // encontra o filho que tem o maior valor
  int maior_filho = esq;
  if (dir < n && !em_ordem_v(v, dir, esq)) maior_filho = dir;

  // se o maior valor dos filhos for maior que o valor em i,
  //   a árvore deve ser corrigida. Coloca o maior valor em i
  //   e desce o valor em i para o filho. Isso pode fazer com
  //   que a árvore desse filho não seja mais heap e deve ser
  //   corrigida.
  if (!em_ordem_v(v, maior_filho, i)) {
    troca(v, i, maior_filho);
    heap_ajeita(n, v, maior_filho);
  }
}

// o vetor v contém dados quaisquer.
// organiza para que contenha uma árvore heap.
void heap_constroi(int n, dado_t v[n])
{
  // os nós à partir de n/2 não têm filhos, logo são heaps.
  // os nós antes disso podem não ser -- ajeita eles.
  // tem que fazer isso de trás para diante, para que os
  //   filhos do nó a ser ajeitado já sejam heaps
  for (int i = n / 2 - 1; i >= 0; i--) {
    heap_ajeita(n, v, i);
  }
}

// {{{2 ordenação merge
//
// a ideia é ordenar à partir de uma operação (merge) que junta
//   dois vetores ordenados em um só. Para isso, move um dado por
//   vez, do vetor que contém o menor valor para o vetor destino.
// usa essa operação repetidas vezes, inicialmente juntando vetores
//   de tamanho 1 (que estão obviamente ordenados), gerando vetores
//   de tamanho 2. Depois, junta esses vetores de tamanho 2 em
//   vetores de tamanho 4 e assim sucessivamente até que só reste
//   um vetor, de tamanho n, com todos os dados ordenados.

// função auxiliar
void merge(dado_t *v, int p1, int p2, int u2, dado_t *w);

void ordena_merge(int n, dado_t v[n])
{
  // aloca um vetor auxiliar de mesmo tamanho que v
  dado_t *w = malloc(n * sizeof(dado_t));
  assert(w != NULL);

  // começa com n partições de tamanho 1 (que são naturalmente ordenadas),
  //   mistura duas a duas obtendo partições ordenadas com o dobro do
  //   tamanho, até ter uma só partição ordenada, de tamanho n
  for (int t = 1; t < n; t *= 2) {
    // mistura duas partições vizinhas, ordenadas, de tamanho t
    for (int i = 0; i < n; i += 2 * t) {
      // a primeira partição inicia na posição i, a segunda na posição j
      int j = i + t;
      // no final do vetor, pode ser que não tenha 2 partições
      if (j >= n) break;
      // u é a última posição da segunda partição
      int u = j + t - 1;
      if (u > n - 1) u = n - 1;
      // faz a junção
      merge(v, i, j, u, w);
    }
  }
  free(w);
}

// no vetor v tem duas partições ordenadas, uma nas posições p1 até
//   p2-1, e outra nas posições p2 até u2.
// esta função mistura essas partições, produzindo uma única partição
//   com os mesmos dados, ordenados, nas posições p1 até u2.
// usa o vetor w como espaço auxiliar (ele tem o mesmo tamanho de v).
void merge(dado_t *v, int p1, int p2, int u2, dado_t *w)
{
  int u1 = p2 - 1;      // última posição da partição 1
  int i1 = p1;          // índice que vai varrer a partição 1
  int i2 = p2;          // índice que vai varrer a partição 2
  int n = u2 - p1 + 1;  // número total de itens nas duas partições
  int iv, iw;           // índices no vetor v e w

  // copia os n dados de v para w, em ordem
  for (iw = 0; iw < n; iw++) {
    // copia para w[iw] o menor entre v[i1] e v[i2]
    if (i1 <= u1 && (i2 > u2 || em_ordem_v(v, i1, i2))) iv = i1++;
    else iv = i2++;
    copia(&w[iw], &v[iv]);
  }

  // copia os n dados ordenados, de w para v
  iv = p1;
  for (iw = 0; iw < n; iw++) {
    copia(&v[iv], &w[iw]);
    iv++;
  }
}


// {{{2 ordenação quick
//
// escolhe um elemento para ser o pivô, coloca todos os elementos
//   do vetor que são menores (ou iguais) a ele no início do vetor
//   e todos os que são maiores no final, e o pivô entre eles.
// com isso, o pivô está no local certo, os dados menores estão do
//   lado certo e os maiores também, só falta ordenar os dados de
//   cada lado.
// usa então a mesma função, de forma recursiva, para ordenar cada
//   lado.

// funções auxiliares
int quick_particiona(int n, dado_t v[n]);

void ordena_quick(int n, dado_t v[n])
{
  // se o vetor tiver menos de 2 elementos, já está ordenado
  if (n < 2) return;

  // particiona o vetor em duas partições, com o pivô entre elas,
  //   a partição esquerda contendo dados menores que o pivô e a direita
  //   com os dados maiores
  int pos_pivô = quick_particiona(n, v);

  // ordena cada partição (o pivô tá no lugar certo)
  ordena_quick(pos_pivô, v);
  ordena_quick(n - pos_pivô - 1, v + pos_pivô + 1);
}

// escolhe um pivô e particiona o vetor v em 3 subvetores:
//   - à esquerda os dados menores que o pivô, 
//   - à direita os dados maiores que o pivô,
//   - entre eles o pivô
// retorna a posição do pivô
int quick_particiona(int n, dado_t v[n])
{
  // escolhe o primeiro elemento do vetor para ser o pivô.
  // essa escolha é simples, mas é péssima se v já estiver ordenado...
  // se escolher um dado em outra posição, ele deve ser colocado na posição 0
  int pos_pivô = 0;

  // antes de i ficam os que são <= pivô
  // depois de j ficam os que são > pivô
  // entre i e j, os que ainda não se sabe
  int i = pos_pivô + 1;
  int j = n - 1;
  while (i <= j) {
    // avança o i até achar um que não pode ficar antes do pivô
    while (i <= j && em_ordem_v(v, i, pos_pivô)) i++;
    // recua o j até achar um que não pode ficar depois do pivô
    while (i <= j && !em_ordem_v(v, j, pos_pivô)) j--;
    if (i < j) {
      // em i tem um que é maior que o pivô, em j um que é <=
      troca(v, i, j);
      i++;
      j--;
    }
  }

  // em j está o último que é <= pivô -> coloca o pivô aí
  if (j != pos_pivô) {
    troca(v, pos_pivô, j);
    pos_pivô = j;
  }

  return pos_pivô;
}

// {{{1 cronômetro

typedef struct timespec crono;

void crono_zera(crono *c)
{
  clock_gettime(CLOCK_MONOTONIC, c);
}

double crono_parcial(crono *c)
{
  crono agora;
  clock_gettime(CLOCK_MONOTONIC, &agora);
  return (agora.tv_sec - c->tv_sec) + 1e-9 * (agora.tv_nsec - c->tv_nsec);
}


// {{{1 execução e temporização

// tipo de dados para representar um algoritmo de ordenação
typedef struct {
  char nome[30];                      // nome do algoritmo
  void (*funcao)(int n, dado_t v[n]); // ptr para função que o implementa
  bool cancelado;                     // true se demorou demais
} algo_t;

// executa um algoritmo de ordenação para ordenar um vetor de tamanho n
void executa_algoritmo(algo_t *algo, int n, ordem_t ordem)
{
  crono c;
  double t;

  if (algo->cancelado) return;
  dado_t *v = malloc(n * sizeof(dado_t));
  if (v == NULL) {
    algo->cancelado = true;
    return;
  }
  preenche_vetor(n, v, ordem);

  n_compara = 0;
  n_copia = 0;

  crono_zera(&c);
  algo->funcao(n, v);
  t = crono_parcial(&c);

  printf("%s %10d %d %12.9f ", algo->nome, n, ordem, t);
  printf("%11ld %11ld ", n_compara, n_copia);
  printf("%d\n", ordenado(n, v));
  algo->cancelado = t > 5;
  free(v);
}

// {{{1 main
int main()
{
  algo_t algos[] = {
    {"bolha   ", ordena_bolha},
    {"bolha2  ", ordena_bolha2},
    {"bolha3  ", ordena_bolha3},
    {"bolha4  ", ordena_bolha4},
    {"insercao", ordena_insercao},
    {"selecao ", ordena_selecao},
    {"shell   ", ordena_shell},
    {"heap    ", ordena_heap},
    {"merge   ", ordena_merge},
    {"quick   ", ordena_quick},
  };
  int n_algo = sizeof(algos) / sizeof(algos[0]);

  for (int a = 0; a < n_algo; a++) {
    for (ordem_t ordem = 0; ordem < n_ordem; ordem++) {
      algos[a].cancelado = false;
      for (int n = 8; !algos[a].cancelado; n *= 2) {
        executa_algoritmo(&algos[a], n, ordem);
      }
    }
  }
}

// vim: foldmethod=marker
