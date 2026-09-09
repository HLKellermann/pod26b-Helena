#include "abb.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
  chave_t c;
  dado_t d;
  ABB *a = abb_cria();
  for (int i = 0; i < 100; i++) {
    sprintf(d.s, "%d", i);
    a = abb_insere(a, i, d);
  }

  for (int j = 0; j < 100; j++) {
    int r = rand() % 100;
    printf("%d ", r);
    a = abb_remove(a, r);
    for (int i = 0; i < 100; i++) {
      dado_t *p = abb_busca(a, i);
      if (p != DADO_NAO_EXISTE) printf("%s ", p->s);
    }
    printf("\n");
  }
}
