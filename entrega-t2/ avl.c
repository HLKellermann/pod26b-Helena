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
  int altura;   // AVL - precisa a altura (ou o fator de eq)
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
  nova->altura = 0;   // AVL - nó folha tem altura 0
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

// AVL - a função a chamar a cada alteração da árvore, para verificar
//       o desequilíbrio e corrigir se for o caso
static ABB* avl_equilibra(ABB *a);

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
  // AVL - a árvore foi alterada, verifica o equilíbrio deste nó
  a = avl_equilibra(a);
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
  // AVL - a árvore foi alterada, verifica o equilíbrio deste nó
  a = avl_equilibra(a);
  return a;
}


// AVL - funções para equilíbrio da árvore

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

// retorna a altura do nó a
static int avl_altura(ABB *a)
{
  if (abb_é_vazia(a)) return -1;
  return a->altura;
}

// recalcula a altura do nó a
// deve ser chamada de baixo para cima na árvore (das folhas para a raiz)
static void avl_calcula_altura(ABB *a)
{
  int alt_esq = avl_altura(a->esq);
  int alt_dir = avl_altura(a->dir);
  a->altura = max(alt_esq, alt_dir) + 1;
}

// retorna o fator de equilíbrio do nó a
// as alturas devem estar atualizadas
static int avl_fator_de_equilíbrio(ABB *a)
{
  return avl_altura(a->esq) - avl_altura(a->dir);
}

// rotação -- o filho esquerdo de a vira raiz
//   [A]        [B]
//  B   X      Y   A
// Y Z            Z X
static ABB *avl_rotação_simples_sobe_esquerdo(ABB *a)
{
  // cria variáveis auxiliares com as subárvores envolvidas (com nomes como na figura)
  ABB *b = a->esq;
  ABB *z = b->dir;

  // faz a rotação
  a->esq = z;
  b->dir = a;

  // os dois nós envolvidos na rotação alteraram a altura -- recalcula
  // da forma como calcula_altura() funciona, tem que chamar de baixo para cima,
  //   e o nó mais abaixo agora é o "a"
  avl_calcula_altura(a);
  avl_calcula_altura(b);

  return b;  // b é a nova raiz
}

// rotação -- o filho direito de a vira raiz
//   [A]        [B]
//  X   B      A   Z
//     Y Z    X Y
ABB *avl_rotação_simples_sobe_direito(ABB *a)
{
  // ... mesma coisa pro outro lado
  // cria variáveis auxiliares com as subárvores envolvidas (com nomes como na figura)
  ABB *b = a->dir;
  ABB *y = b->esq;

  // faz a rotação
  a->dir = y;
  b->esq = a;

  // os dois nós envolvidos na rotação alteraram a altura -- recalcula
  // da forma como calcula_altura() funciona, tem que chamar de baixo para cima,
  //   e o nó mais abaixo agora é o "a"
  avl_calcula_altura(a);
  avl_calcula_altura(b);

  return b;  // b é a nova raiz
}

ABB *avl_rotação_dupla_sobe_esquerdo(ABB *a)
{
  a->esq = avl_rotação_simples_sobe_direito(a->esq);
  return avl_rotação_simples_sobe_esquerdo(a);
}

ABB *avl_rotação_dupla_sobe_direito(ABB *a)
{
  a->dir = avl_rotação_simples_sobe_esquerdo(a->dir);
  return avl_rotação_simples_sobe_direito(a);
}

static ABB* avl_equilibra(ABB *a)
{
  if (abb_é_vazia(a)) return a;  // pode acontecer na remoção
  avl_calcula_altura(a); // pode ter mudado pela alteração na árvore
  int fat_eq = avl_fator_de_equilíbrio(a);
  if (fat_eq >= -1 && fat_eq <= 1) {
    // está equilibrada -- nada a fazer
    return a;
  }
  // está desequilibrada -- o tipo de rotação depende do fator de equilíbrio do filho mais alto
  if (fat_eq == 2) { // filho mais alto está na esquerda
    if (avl_fator_de_equilíbrio(a->esq) == -1) {
      return avl_rotação_dupla_sobe_esquerdo(a);
    } else {
      return avl_rotação_simples_sobe_esquerdo(a);
    }
  } else {  // filho mais alto está na direita
    // ... mesma coisa pro outro lado
    if (avl_fator_de_equilíbrio(a->dir) == 1) {
      return avl_rotação_dupla_sobe_direito(a);
    } else {
      return avl_rotação_simples_sobe_direito(a);
    }
  }
  return a;
}
