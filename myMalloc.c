#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

struct block_meta {
  size_t size;
  struct block_meta *next, *prev;
  int free;
};

#define META_SIZE sizeof(struct block_meta)

struct block_meta *top_block = NULL;

void *global_base = NULL;//memoria ja alocada

static inline size_t align(size_t size) { //retorna o tamanho multiplo do size_t pra poder a memoria ficar alinhada
    return size + (sizeof(size_t) - 1) & ~(sizeof(size_t) - 1);
}

//procura um bloco livre
struct block_meta *find_free_block(struct block_meta **last, size_t size) {
  struct block_meta *current = global_base;
  while (current && !(current->free && current->size >= size)) {
    *last = current;
    current = current->next;
  }
  return current;
}

void merge_next(struct block_meta *c) { // JUNTA BLOCOS
  
    c->size = align(c->size + c->next->size); // Tamanho do bloco total = tamanho atual + tamanho do proximo + tamanho do metadado
    printf("novo tamanho: %td \n", c-> size);
    c->next = c->next->next; // Proximo atual = proximo do proximo
    if (c->next) {
        c->next->prev = c; // Se existir proximo, proximo->anterior = c
    }
}

//caso nao tenha bloco livre devera pedir mais memoria pro SO
struct block_meta *request_space(struct block_meta* last, size_t size) {  
  struct block_meta *block;
  block = sbrk(0);// te retorna o proximo endereco de memoria livre
  void *request = sbrk(align(size + META_SIZE)); // requisita o tamanho pedido e retorna o topo (mesmo valor do sbrk(0))
  assert((void*)block == request); 
  if (request == (void*) -1) {
    return NULL; // sbrk falhou.
  }

  if (last) { // NULL ou primeira chamada.
    last->next = block;
  }

  block->size = size;
  block->next = NULL;
  block->free = 0;
  block->prev = last;
  return block;
}

void *MyMalloc(size_t size) {
  struct block_meta *block;

  if (size <= 0) {
    return NULL;
  }

  if (!global_base) { //  primeira chamada
    block = request_space(NULL, size);
    if (!block) {
      return NULL;
    }
    global_base = block;
    top_block = block;
  } else {
    struct block_meta *last = global_base;
    block = find_free_block(&last, size);
    if (!block) { // Failed to find free block.
      block = request_space(last, size);
      if (!block) {
        return NULL;
      }
    } else {      // Found free block
    
      block->free = 0;
    }
  }

  return(block+1);
}

struct block_meta *get_block_ptr(void *ptr) { // pega o metadado do bloco  
  return (struct block_meta*)ptr - 1;
}

void MyMallocFree(void *ptr) {
  if (!ptr) {
    return;
  }
  struct block_meta* block_ptr = get_block_ptr(ptr);
  assert(block_ptr->free == 0);
  block_ptr->free = 1;

  
  if (block_ptr->next && block_ptr->next->free) { // Se o C proximo existe e é livre = unir C com C próximo
      printf("TEM VAZIO NA FRENTE\n");
      merge_next(block_ptr);
  }

  if (top_block != block_ptr){ // o primeiro bloco nao tem anterior  
    if (block_ptr->prev->free) { // Se o C anterior é livre == unir C com C anterior
      printf("TEM VAZIO ATRAS\n");
      printf("3\n");
      merge_next(block_ptr->prev);
    }
  }

}

int main () {
    int* n1;
    int* n2;
    int* n3;
    n1 = (int*)MyMalloc(sizeof(int));
    n2 = (int*)MyMalloc(sizeof(int));
    n3 = (int*)MyMalloc(sizeof(int));
    *n1 = 88000;    
    *n2 = 500;    
    *n3 = 1000;
    printf("Endereco do novo ponteiro: %p \n", &n1);
    printf("TAMANHO do novo ponteiro: %ld \n", get_block_ptr(n1)->size);
    printf("Endereco do novo ponteiro: %p \n", &n2);
    printf("Endereco do novo ponteiro: %p \n", &n3);
    printf("Valor: %d \n", *n1);
    MyMallocFree(n2);
    MyMallocFree(n3);
    printf("Valor: %d \n", *n1);
}