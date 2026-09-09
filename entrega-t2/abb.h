#ifndef ABB_H
#define ABB_H

// abb.h
// árvore binária de busca
// pod26b

#include <stdbool.h>

// tipos de chave e dado do usuário.
// melhor se fosse definido em outro arquivo.
typedef int chave_t;

typedef struct {
  char s[8];
} dado_t;

#define DADO_NAO_EXISTE NULL

// função de comparação entre duas chaves
typedef enum { IGUAIS, MENOR, MAIOR } comparação;
comparação compara_chaves(chave_t a, chave_t b);
// fim da declaração de chave e dado do usuário


// declaração de tipo opaco da árvore
typedef struct abb ABB;

// funções que formam a interface de operações em uma ABB

// cria uma nova árvore, vazia
ABB *abb_cria();

// retorna true se a árvore a for vazia
bool abb_é_vazia(ABB *a);

// busca o dado associado a chave
// retorna um ponteiro para o dado se existir
// retorna DADO_NAO_EXISTE se não existir
dado_t *abb_busca(ABB *a, chave_t chave);

// insere dado associado a chave na árvore a
// retorna a nova árvore
ABB *abb_insere(ABB *a, chave_t chave, dado_t dado);

// remove o nó contendo chave da árvore a
// retorna a nova árvore
ABB *abb_remove(ABB *a, chave_t chave);

#endif // ABB_H
