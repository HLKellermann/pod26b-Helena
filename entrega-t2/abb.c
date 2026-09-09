#include "abb.h"

#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

// implementação da função de comparação de chaves
// ficaria melhor em um arquivo do usuário
comparação compara_chaves(chave_t a, chave_t b)
{
  if (a == b) return IGUAIS;
  if (a < b) return MENOR;
  return MAIOR;
}

// declaração do registro que contém um nó da árvore
struct abb {
  chave_t chave;
  dado_t dado;
  ABB *esq;
  ABB *dir;
};

// funções auxiliares (não exportadas aos clientes)

// preenche um nó de uma árvore vazia com chave e dado
// retorna o nó preenchido
// o novo nó é uma folha
static ABB *abb_preenche(ABB *a, chave_t chave, dado_t dado)
{
  assert(abb_é_vazia(a));
  ABB *nova = malloc(sizeof(*nova));
  assert(nova != NULL);
  nova->chave = chave;
  nova->dado = dado;
  nova->esq = abb_cria();
  nova->dir = abb_cria();
  return nova;
}

// libera a memória alocada por chave e dado no nó a
static void abb_libera_dados(ABB *a)
{
  // se algum dado fosse alocado, seria liberado aqui
}

static bool abb_é_folha(ABB *a)
{
  return abb_é_vazia(a->esq) && abb_é_vazia(a->dir);
}

// esvazia o nó a
static ABB *abb_libera_nó(ABB *a)
{
  abb_libera_dados(a);
  free(a);
  return NULL; // a árvore vazia é representada por NULL
}

// retorna o nó que contém a menor chave em a (o nó mais à esquerda)
static ABB *abb_menor(ABB *a)
{
  if (abb_é_vazia(a->esq)) return a;
  return abb_menor(a->esq);
}

// retorna o nó que contém a maior chave em a (o nó mais à direita)
static ABB *abb_maior(ABB *a)
{
  if (abb_é_vazia(a->dir)) return a;
  return abb_maior(a->dir);
}

// troca as chaves e dados entre os nós a e b
static void abb_troca_dados(ABB *a, ABB *b)
{
  chave_t ct = a->chave;
  a->chave = b->chave;
  b->chave = ct;
  dado_t dt = a->dado;
  a->dado = b->dado;
  b->dado = dt;
}

// implementação das funções da interface
ABB *abb_cria()
{
  return NULL; // a árvore vazia é representada por NULL
}

bool abb_é_vazia(ABB *a)
{
  return a == NULL;
}

dado_t *abb_busca(ABB *a, chave_t chave)
{
  if (abb_é_vazia(a)) return DADO_NAO_EXISTE;
  switch (compara_chaves(chave, a->chave)) {
    case IGUAIS: return &a->dado;
    case MENOR:  return abb_busca(a->esq, chave);
    case MAIOR: return abb_busca(a->dir, chave);
  }
}

ABB *abb_insere(ABB *a, chave_t chave, dado_t dado)
{
  if (abb_é_vazia(a)) return abb_preenche(a, chave, dado);
  switch (compara_chaves(chave, a->chave)) {
    case IGUAIS:
      break; // ignora em silêncio!
    case MENOR:
      a->esq = abb_insere(a->esq, chave, dado);
      break;
    case MAIOR:
      a->dir = abb_insere(a->dir, chave, dado);
      break;
  }
  return a;
}

ABB *abb_remove(ABB *a, chave_t chave)
{
  if (abb_é_vazia(a)) return a; // não existe
  ABB *a_na_entrada = a;
  switch(compara_chaves(chave, a->chave)) {
    case IGUAIS:
      if (abb_é_vazia(a->dir)) {
        a = a->esq;
      } else if (abb_é_vazia(a->esq)) {
        a = a->dir;
      } else {
        ABB *vitima = abb_menor(a->dir);
        abb_troca_dados(a, vitima);
        a->dir = abb_remove(a->dir, chave);
      }
      break;
    case MENOR:
      a->esq = abb_remove(a->esq, chave);
      break;
    case MAIOR:
      a->dir = abb_remove(a->dir, chave);
      break;
  }
  if (a != a_na_entrada) abb_libera_nó(a_na_entrada);
  return a;
}

