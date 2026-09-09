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
  int nível;     // AA - precisa o nível do nó
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
  nova->nível = 1;   // AA - nível de nó folha é 1
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

// AA - funções a chamar a cada alteração da árvore, para verificar
//      o desequilíbrio e corrigir se for o caso. No caso de remoção,
//      a correção é mais complicada, por isso duas funções
static ABB* aa_equilibra_inserção(ABB *a);
static ABB* aa_equilibra_remoção(ABB *a);

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
  // AA - a árvore foi alterada, verifica o equilíbrio deste nó
  a = aa_equilibra_inserção(a);
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
  // AA - a árvore foi alterada, verifica o equilíbrio deste nó
  a = aa_equilibra_remoção(a);
  return a;
}


// AA - funções para equilíbrio da árvore

// retorna o maior entre dois inteiros
static int max(int a, int b)
{
  return a > b ? a : b;
}

// retorna o menor entre dois inteiros
static int min(int a, int b)
{
  return a < b ? a : b;
}

// retorna o nível do nó a
static int aa_nível(ABB *a)
{
  if (abb_é_vazia(a)) return 0;
  return a->nível;
}

// primeira função de correção (skew)
// se o nó a e seu filho esquerdo estão no mesmo nível,
//   faz o nó a ficar à direita de seu filho esquerdo,
//   que vira a nova raiz
//   E <- [A]        [E] -> A
//  X Y     Z        X     Y Z
static ABB *skew(ABB *a)
{
  int nível = aa_nível(a);
  if (nível == 0) return a;
  if (nível != aa_nível(a->esq)) return a;
  // rotação
  ABB *e = a->esq;
  ABB *y = e->dir;
  a->esq = y;
  e->dir = a;
  return e;
}

// segunda função de correção (skip)
// se tem mais dois nós no mesmo nível do nó a na sua direita,
//   sobe o primeiro deles, que vira a nova raiz
//                          [D]
//   [A] -> D -> Z         A   Z
//   X     Y              X Y
static ABB *split(ABB *a)
{
  int nível = aa_nível(a);
  if (nível == 0) return a;
  if (nível != aa_nível(a->dir)) return a;
  if (nível != aa_nível(a->dir->dir)) return a;
  // rotação
  ABB *d = a->dir;
  ABB *y = d->esq;
  a->dir = y;
  d->esq = a;
  d->nível++;
  return d;
}

// realiza o equilíbrio após uma inserção
// as funções skew e spit verificam se a rotação é necessária
static ABB *aa_equilibra_inserção(ABB *a)
{
  a = skew(a);
  a = split(a);
  return a;
}

// função auxiliar para o reequilíbrio na remoção
// diminui o nível do nó a (e de seu filho direito), se necessário
// diminui se o nível de a está mais de um nível acima de algum filho
// retorna true se o nível do nó a foi alterado
static bool aa_diminui_nível(ABB *a)
{
  int nível = aa_nível(a);
  if (nível == 0) return false;
  int nível_esq = aa_nível(a->esq);
  int nível_dir = aa_nível(a->dir);
  if (nível - nível_esq <= 1 && nível - nível_dir <= 1) return false;
  a->nível = nível - 1;
  if (nível_dir > a->nível) a->dir->nível = nível - 1;
  return true;
}

// realiza o equilíbrio após uma remoção
static ABB *aa_equilibra_remoção(ABB *a)
{
  if (!aa_diminui_nível(a)) return a;

  // para garantir, tem que fazer o skew do nó, do filho e do neto à direita
  //   e mais o split do nó e do filho á direita
  // alguns cuidados a tomar:
  //   - só tem neto se houver filho
  //   - uma operação pode alterar o nó que está sendo considerado.
  a = skew(a);
  if (!abb_é_vazia(a->dir)) {
    a->dir = skew(a->dir);
    a->dir->dir = skew(a->dir->dir);
  }
  a = split(a);
  a->dir = split(a->dir);
  return a;
}
