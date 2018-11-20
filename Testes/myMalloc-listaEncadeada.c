#include <stdio.h>
#include <stdlib.h>

/*
   Estrutura  que define a Lista
   Lembrando que a a instrução: struct celulaLista *proximo
   é uma estrutura auto-referenciada, ou seja, neste caso
   é uma estrutura que aponta para a próxima estrutura.
*/
typedef struct celulaLista{
   int informacao;
   struct celulaLista *proximo;
}celula;

int main(){
   // Declarações
      celula *pLista;
      int opcao = 0;
   // Funções
      void initLista(celula **pRecebido);
      void insertLista(celula **pRecebido);
      void buscaListaSimples(celula **pRecebido);
      void removeElementoListaSimples(celula **pRecebido);
      int leValor(int *valorRecebido);
   // Instruções
      pLista = (celula *)MyMalloc(sizeof(struct celulaLista));
      initLista(&pLista);
      insertLista(&pLista);
      removeElementoListaSimples(&pLista);
      insertLista(&pLista);
      removeElementoListaSimples(&pLista);
      insertLista(&pLista);
      removeElementoListaSimples(&pLista);
      insertLista(&pLista);
      return 0;
}
/*
   Inicialização da Lista Encadeada
*/
void initLista(celula **pRecebido){
   (*pRecebido)->proximo = NULL;
}

/*
   Função para Percorrer Elementos
*/
void buscaListaSimples(celula **pRecebido){
   // Declarações
      celula *temporario;
   // Instruções
      if((*pRecebido)->proximo == NULL){
         printf("Lista Vazia!\n");
      }
      else{
         temporario = (celula *)MyMalloc(sizeof(celula));
         temporario = (*pRecebido)->proximo;
         printf("Lista = ");
         while(temporario != NULL){
            printf("%d ", temporario->informacao);
            temporario = temporario->proximo;
         }
         printf("\n");
      }   
}

/*
   Função para Inserção no Início
*/
void insertLista(celula **pRecebido){
   // Declarações
      celula *temporario;
      int valor, i=0;
   // Instruções
      for(i = 0; i < 20; i++){
         valor = 10 * i;
         temporario = (celula *)MyMalloc(sizeof(celula));
         temporario->informacao = valor;
         temporario->proximo = (*pRecebido)->proximo;
         (*pRecebido)->proximo = temporario;
      }  

      buscaListaSimples(pRecebido);
}

/*
   Remove Elemento da Cabeça
*/
void removeElementoListaSimples(celula **pRecebido){
   // Declarações
      celula *temporario;
   // Instruções
      if((*pRecebido)->proximo == NULL){
         printf("\nLista Vazia!");
      }
      else{
         while(((*pRecebido)->proximo != NULL)){
            temporario = (*pRecebido)->proximo;
            (*pRecebido)->proximo = temporario->proximo;
            MyMallocFree(temporario);
         }
      }

      buscaListaSimples(pRecebido);
}