#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

struct myMalloc_information{
    int total_memory_ocupied;
    int total_memory_free;
    int number_blocks_ocupied;
    int number_block_free;
};

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

struct block_meta *get_new_block(){
  struct block_meta *new_block = sbrk(align(sizeof(struct block_meta))); 
  new_block->prev = NULL;
  new_block->next = NULL;
  new_block->size = 0;
  new_block->free = 0;
  return new_block;
}

void merge_next(struct block_meta *c) { // JUNTA BLOCOS
  
    c->size = align(c->size + c->next->size); // Tamanho do bloco total = tamanho atual + tamanho do proximo
    //printf("novo tamanho: %td \n", c-> size);
    c->next = c->next->next; // Proximo atual = proximo do proximo
    if (c->next) {
        c->next->prev = c; // Se existir proximo, next->prev = c
    }
}

void split_next(struct block_meta *c, size_t size) { // Dividir dois blocos (será usádo para dividir a parte não usada em outro bloco)
    //printf("split \n");
    struct block_meta *new_block = (struct block_meta*)((char*) c + size); // posição do novo chunk! endereço C (atual) + tamanho!
    new_block->prev = c; // novo->anterior aponta para o C
    new_block->next = c->next; // novo->proximo aponta para o proximo do c (inserido no meio)
    //printf("split \n");
    
    new_block->size = c->size - size; // tamanho do novo chunk = tamanho do C - espaço usado
    new_block->free = 1; // novo chunk setado como livre   
    //printf("split \n");
    
    if (c->next) {
        //printf("Endereco do novo ponteiro: %p \n", &c->next);
        c->next->prev = new_block;

    }
    c->next = new_block; //proximo de C = novo C
    c->size = size - sizeof(new_block); // tamanho de C = tamanho usado - tamanho de bloco
}

//procura um bloco livre
struct block_meta *find_free_block(size_t size) {
  struct block_meta *current = top_block;
  while (current && !(current->free && current->size >= size)) {
  //printf("achar um bloco\n");
    global_base = current;
    current = current->next;
  }
  return current;
}

//caso nao tenha bloco livre devera pedir mais memoria pro SO
struct block_meta *request_space(struct block_meta* last, size_t size) {  
  //printf("request space\n");
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

  if (!top_block) { //  primeira chamada
    block = request_space(NULL, size);
    if (!block) {
      return NULL;
    }
    global_base = block;
    top_block = block;
  } else {
    struct block_meta *global = global_base;
    block = find_free_block(size);
    if (!block) { // Failed to find free block.
      block = request_space(global, size);
      if (!block) {
        return NULL;
      }
    } else {      // Found free block      
      if (block) {
        //printf("achou vazio \n");
       // split_next(block, align(size));
        block->free = 0;
      }
    }
  }

  return(block+1);
}

struct myMalloc_information MyMallocGerency(){
    struct myMalloc_information myMalloc_informations;

    struct block_meta *block_aux = top_block;
    int total_memory_ocupied = 0, total_memory_free = 0;
    int number_blocks_ocupied = 0, number_block_free = 0;
    while(block_aux){
        if(block_aux->free == 0){
            total_memory_ocupied += (block_aux->size);
            number_blocks_ocupied++;
        } 
        else{
            total_memory_free += (block_aux->size);
            number_block_free++;
        }

        block_aux = block_aux->next;
    }
    myMalloc_informations.total_memory_ocupied = total_memory_ocupied;
    myMalloc_informations.total_memory_free = total_memory_free;
    myMalloc_informations.number_blocks_ocupied = number_blocks_ocupied;
    myMalloc_informations.number_block_free = number_block_free;
    
    return myMalloc_informations;
}

void print_block(struct block_meta *block){
  struct block_meta *block_aux = top_block;
  int count = 0;
  int count_free = 0;

  printf("###### Informações dos Blocos ######\n\n");
  while(block_aux){
    printf("---------- Bloco %d ----------\nStatus: %s\nTamanho: %lu \n\n", count, 
              block_aux->free == 1 ? "Livre" : "Ocupado", 
              block_aux->size + sizeof(struct block_meta));

      if(block_aux->free == 1){
        count_free++;
      }
      count++;
      if(block_aux->next){
        block_aux = block_aux->next;
      }
      else{
        break;
      }
  }
}

void print_myMalloc_information(struct block_meta *block){
  struct myMalloc_information myMalloc_informations;
  myMalloc_informations = MyMallocGerency();

  printf("\n###### Informações do MyMalloc ######\n\n");
  printf(" -> Número de Blocos Livres:   \t %d \n",myMalloc_informations.number_block_free);
  printf(" -> Número de Blocos Ocupados: \t %d \n",myMalloc_informations.number_blocks_ocupied);
  printf(" -> Total de Memória Livre:    \t %d \n",myMalloc_informations.total_memory_free);
  printf(" -> Total de Memória Ocupada:  \t %d \n\n",myMalloc_informations.total_memory_ocupied);

  printf("----------------------------------------\n\n");
  print_block(block);
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
      //printf("TEM VAZIO NA FRENTE\n");
      merge_next(block_ptr);
  }

  if (top_block != block_ptr){ // o primeiro bloco nao tem anterior  
    if (block_ptr->prev->free) { // Se o C anterior é livre == unir C com C anterior
      //printf("TEM VAZIO ATRAS\n");
      //printf("3\n");
      merge_next(block_ptr->prev);
    }
  }

}

int main () {
    int* n1;
    int* n2;
    int* n3;
    //int* n4;
    n1 = (int*)MyMalloc(sizeof(int));
    n2 = (int*)MyMalloc(sizeof(int));
    n3 = (int*)MyMalloc(sizeof(int));
    *n1 = 88000;    
    *n2 = 500;    
    *n3 = 1000;
    //printf("Endereco do novo ponteiro: %p \n", &n1);
    //printf("Endereco do novo ponteiro: %p \n", &n2);
    //printf("Endereco do novo ponteiro: %p \n", &n3);
    //printf("Valor: %d \n", *n1);
    MyMallocFree(n2);
    MyMallocFree(n3);
    //printf("Valor: %d \n", *n1);
    print_myMalloc_information(MyMalloc(sizeof(int)));

    //n4 = (int*)MyMalloc(sizeof(int));
    //*n4 = 2000;
    
    //printf("Endereco do novo ponteiro: %p \n", &n4);


}