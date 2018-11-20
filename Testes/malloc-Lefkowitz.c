#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>

#define TAM_NOME 100
#define NOME_ARQUIVO "arquivo_A1.dat"
#define NOME_ARQUIVO_INDICE "arquivo_A5.dat"
#define NOME_ARQUIVO_CHAVE_SECUNDARIA "arquivo_A2.dat"
#define NOME_ARQUIVO_CHAVE_PONTEIRO "arquivo_A6.dat"
#define NOME_ARQUIVO_CHAVE_SECUNDARIA_ORDENADO "arquivo_A4.dat"
#define NOME_ARQUIVO_CHAVE_PONTEIRO_ORDENADO "arquivo_A7.dat"
#define NOME_ARQUIVO_FINAL "arquivo_A8.dat"

typedef struct Cliente {
	int cod_cliente;
	char nome[TAM_NOME];
	int idade;
} Cliente;

typedef struct ListaClientes {
	Cliente **lista;
	int qtd;
} ListaClientes;

typedef struct RegistroChaveSecundaria {
	int endereco_disco;
	int cod_cliente;
	int idade;
} RegistroChaveSecundaria;

typedef struct ListaRegistroChaveSecundaria {
	RegistroChaveSecundaria **lista;
	int qtd;
} ListaRegistroChaveSecundaria;

typedef struct RegistroIndiceIdade {
	int idade;
	int pt;
	int q;
} RegistroIndiceIdade;

typedef struct ListaRegistrosIndiceIdade {
	RegistroIndiceIdade **lista;
	int qtd;
} ListaRegistrosIndiceIdade;

typedef struct RegistroChavePonteiro {
	int endereco_disco;
	int cod_cliente;
	int idade;
	int prox;
} RegistroChavePonteiro;

typedef struct ListaRegistroChavePonteiro {
	RegistroChavePonteiro **lista;
	int qtd;
} ListaRegistroChavePonteiro;

typedef struct RegistroFinal {
	int cod_cliente;
	char nome[TAM_NOME];
	int idade;
	int prox;
} RegistroFinal;

typedef struct ListaFinal {
	RegistroFinal **lista;
	int qtd;
} ListaFinal;

void imprime_cliente(Cliente *cliente);
Cliente *cliente(int cod, char *nome, int idade);
void salva_cliente(Cliente *cliente, FILE *out);
Cliente *le_cliente(FILE *in);
int cmp_cliente(Cliente *c1, Cliente *c2);
int tamanho_cliente();


void imprime_clientes(ListaClientes *lc);
ListaClientes *cria_clientes(int qtd, ...);
void salva_clientes(char *nome_arquivo, ListaClientes *lc);
ListaClientes *le_clientes(char *nome_arquivo);
int cmp_clientes(ListaClientes *c1, ListaClientes *c2);
void libera_clientes(ListaClientes *lc);


ListaRegistroChaveSecundaria *cria_lista_registro_chave_segundaria(ListaClientes *lc);
void salva_lista_registro_chave_segundaria(char *nome_arquivo, ListaRegistroChaveSecundaria *lcs);
void imprime_lista_registro_chave_segundaria(ListaRegistroChaveSecundaria *lcs);


void imprime_registro_indice_idade(RegistroIndiceIdade *registro_indice_idade);
RegistroIndiceIdade *registro_indice_idade(int idade, int pt, int q);
void salva_registro_indice_idade(RegistroIndiceIdade *registro_indice_idade, FILE *out);
RegistroIndiceIdade *le_registro_indice_idade(FILE *in);
int cmp_registro_indice_idade(RegistroIndiceIdade *r1, RegistroIndiceIdade *r2);
int tamanho_registro_indice_idade();


void imprime_registros(ListaRegistrosIndiceIdade *lr);
ListaRegistrosIndiceIdade *cria_registros(ListaRegistroChaveSecundaria *lcs);
void salva_registros(char *nome_arquivo, ListaRegistrosIndiceIdade *l);
ListaRegistrosIndiceIdade *le_registros(char *nome_arquivo);
int cmp_registros(ListaRegistrosIndiceIdade *l1, ListaRegistrosIndiceIdade *l2);
void libera_registros(ListaRegistrosIndiceIdade *l);

ListaRegistroChavePonteiro *cria_lista_registro_chave_ponteiro(ListaRegistroChaveSecundaria *lc);
void salva_lista_registro_chave_ponteiro(char *nome_arquivo, ListaRegistroChavePonteiro *lcp);
void imprime_lista_registro_chave_ponteiro(ListaRegistroChavePonteiro *lcp);

ListaFinal* criar_lista_final(ListaClientes *lc, ListaRegistroChavePonteiro *lcp);
void salvar_lista_final(char *nome_arquivo, ListaFinal *lf);
void imprime_lista_final(ListaFinal *lf);

void lefkowitz(char *nome_arquivo, int tam);

int main()
{
    ListaClientes *tab_cliente;

	tab_cliente = cria_clientes(5,
		cliente(1, "Ana", 18),
                cliente(2, "Carlos", 20),
                cliente(3, "Bruna", 22),
                cliente(4, "Marcos", 20),
                cliente(5, "Fernanda", 22));
	salva_clientes(NOME_ARQUIVO, tab_cliente);

	lefkowitz(NOME_ARQUIVO, 5);

	libera_clientes(tab_cliente);
}

void lefkowitz(char *nome_arquivo, int tam)
{
	/*Inicio Lefkowitz - Passo 1
	1 --> Arquivo de dados original, ordenado pela chave primária, é chamado A1
	2 --> Criar um novo arquivo A2
	3 --> Copiar para este arquivo a Chave Primária (CP) e todas as
	Chaves Secundárias (CSs) para as quais se deseja construir um
	índice
	4 --> Inserir também uma coluna ED (Endereço de Disco) para cada
	registro
	*/

	//Le arquivo de dados original
    ListaClientes *tab_cliente = le_clientes(nome_arquivo);

    int i, j;
    Cliente *c;

	//Ordena o arquivos de dados original
    for(i = 0; i < tam ; i++)
    {
        for(j = i + 1; j < tam ; j++)
        {
            if (tab_cliente->lista[i]->cod_cliente > tab_cliente->lista[j]->cod_cliente)
            {
                c = tab_cliente->lista[i];
                tab_cliente->lista[i] = tab_cliente->lista[j];
                tab_cliente->lista[j] = c;
            }
        }
	}

	printf("Arquivo A1:\n");
	imprime_clientes(tab_cliente);
	printf("\n");

	//Cria novo Arquivo A2 (Passo 2, 3 e 4)
    ListaRegistroChaveSecundaria *tab_chaveSec = cria_lista_registro_chave_segundaria(tab_cliente);

	printf("Arquivo A2:\n");
	imprime_lista_registro_chave_segundaria(tab_chaveSec);
	printf("\n");

	/*** Fim Lefkowitz - Passo 1 ***/


	/*Inicio Lefkowitz - Passo 2 
	1 --> Decompor o arquivo A2 em vários arquivos A3, um para
	cada atributo que será indexado
	*/

	/* Este passo não é feito poís só existe um atributo que será indexado,
	portanto o arquivo A3 = arquivo A2 gerado no passo anterior
	*/

	/*** Fim Lefkowitz - Passo 2 ***/


	/*Inicio Lefkowitz - Passo 3
	1 --> Ordenar os arquivos A3 por chave secundária, gerando
	arquivos A4
	*/

	//Ordena chave secundaria
	RegistroChaveSecundaria *r;

	for(i = 0; i < tam ; i++)
    {
        for(j = i + 1; j < tam ; j++)
        {
            if (tab_chaveSec->lista[i]->idade > tab_chaveSec->lista[j]->idade)
            {
                r = tab_chaveSec->lista[i];
                tab_chaveSec->lista[i] = tab_chaveSec->lista[j];
                tab_chaveSec->lista[j] = r;
            }
        }
	}

	//Cria arquivo 4
	salva_lista_registro_chave_segundaria(NOME_ARQUIVO_CHAVE_SECUNDARIA_ORDENADO, tab_chaveSec);

	printf("Arquivo A4:\n");
	imprime_lista_registro_chave_segundaria(tab_chaveSec);
	printf("\n");

	/*** Fim Lefkowitz - Passo 3 ***/


	/*Inicio Lefkowitz - Passo 4
	1 --> Cada arquivo A4 é processado para adicionar a quantidade
	de registros que possuem a Chave Secundária
	2 --> CSi, e o endereço do primeiro registro que possui CSi,
	gerando vários arquivos A5 (um para cada arquivo A4)
	3 --> Cada arquivo A4 é processado para adicionar o endereço
	do próximo registro que contém a Chave Secundária CSi,
	gerando vários arquivos A6 (um para cada arquivo A4)
	*/

	//Cria arquivo A5
	ListaRegistrosIndiceIdade *tab_regInd = cria_registros(tab_chaveSec);

	printf("Arquivo A5:\n");
	imprime_registros(tab_regInd);
	printf("\n");

	//Cria arquivo A6
	ListaRegistroChavePonteiro *tab_chavePont = cria_lista_registro_chave_ponteiro(tab_chaveSec);

	printf("Arquivo A6:\n");
	imprime_lista_registro_chave_ponteiro(tab_chavePont);
	printf("\n");

	/*** Fim Lefkowitz - Passo 4 ***/


	/*Inicio Lefkowitz - Passo 5
	1 --> Ordenar os arquivos A6 por chave primária, gerando
	arquivos A7
	*/

	//Ordena por chave primaria
	RegistroChavePonteiro *cp;

	for(i = 0; i < tam ; i++)
    {
        for(j = i + 1; j < tam ; j++)
        {
            if (tab_chavePont->lista[i]->cod_cliente > tab_chaveSec->lista[j]->cod_cliente)
            {
                cp = tab_chavePont->lista[i];
                tab_chavePont->lista[i] = tab_chavePont->lista[j];
                tab_chavePont->lista[j] = cp;
            }
        }
	}

	//Cria arquivo A7
	salva_lista_registro_chave_ponteiro(NOME_ARQUIVO_CHAVE_PONTEIRO_ORDENADO, tab_chavePont);

	printf("Arquivo A7:\n");
	imprime_lista_registro_chave_ponteiro(tab_chavePont);
	printf("\n");

	/*** Fim Lefkowitz - Passo 5 ***/

	/*Inicio Lefkowitz - Passo 6
	1 --> Juntar o arquivo A1 com todos os arquivos A7, gerando um
	arquivo A8 (no arquivo A8, a coluna ED não deve estar
	presente)
	2 --> Os arquivos de índice são os arquivos A5
	*/

	ListaFinal *lf = criar_lista_final(tab_cliente, tab_chavePont);

	printf("Arquivo A8:\n");
	imprime_lista_final(lf);
	printf("\n");

	/*** Fim Lefkowitz - Passo 6 ***/

}

/*****************************************************************/

ListaRegistroChaveSecundaria *cria_lista_registro_chave_segundaria(ListaClientes *lc)
{
	ListaRegistroChaveSecundaria *r = (ListaRegistroChaveSecundaria *)malloc(sizeof(ListaRegistroChaveSecundaria));

	r->lista = (RegistroChaveSecundaria**)malloc(sizeof(RegistroChaveSecundaria*) * lc->qtd);
	r->qtd = lc->qtd;

	int i;
	for (i = 0; i < lc->qtd; i++) {
		r->lista[i] = (RegistroChaveSecundaria *)malloc(sizeof(RegistroChaveSecundaria));

		r->lista[i]->endereco_disco = i;
		r->lista[i]->cod_cliente = lc->lista[i]->cod_cliente;
		r->lista[i]->idade = lc->lista[i]->idade;
	}

	salva_lista_registro_chave_segundaria(NOME_ARQUIVO_CHAVE_SECUNDARIA, r);

	return r;
}

void salva_lista_registro_chave_segundaria(char *nome_arquivo, ListaRegistroChaveSecundaria *lcs)
{
	FILE *out = fopen(nome_arquivo, "wb");
	int i;
	for (i = 0; i < lcs->qtd; i++) 
	{
		fwrite(&lcs->lista[i]->endereco_disco, sizeof(int), 1, out);
		fwrite(&lcs->lista[i]->cod_cliente, sizeof(int), 1, out);
		fwrite(&lcs->lista[i]->idade, sizeof(int), 1, out);
	}
	fclose(out);
}

void imprime_lista_registro_chave_segundaria(ListaRegistroChaveSecundaria *lcs)
{
	int i;
	for(i = 0; i < lcs->qtd; i++)
	{
		printf("%d	%d	%d\n",lcs->lista[i]->endereco_disco, lcs->lista[i]->cod_cliente, lcs->lista[i]->idade);
	}
}


/****************************************************************/

void imprime_cliente(Cliente *cliente)
{
	printf("%d, %s, %d\n", cliente->cod_cliente, cliente->nome, cliente->idade);
}

Cliente *cliente(int cod, char *nome, int idade)
{
	Cliente *cliente = (Cliente *) malloc(sizeof(Cliente));
	if (cliente) memset(cliente, 0, sizeof(Cliente));
	cliente->cod_cliente = cod;
	strcpy(cliente->nome, nome);
	cliente->idade = idade;
	return cliente;
}

void salva_cliente(Cliente *cliente, FILE *out)
{
	fwrite(&cliente->cod_cliente, sizeof(int), 1, out);
	fwrite(cliente->nome, sizeof(char), sizeof(cliente->nome), out);
	fwrite(&cliente->idade, sizeof(int), 1, out);
}

Cliente *le_cliente(FILE *in)
{
	Cliente *cliente = (Cliente *) malloc(sizeof(Cliente));
	if (0 >= fread(&cliente->cod_cliente, sizeof(int), 1, in)) {
		free(cliente);
		return NULL;
	}
	fread(cliente->nome, sizeof(char), sizeof(cliente->nome), in);
	fread(&cliente->idade, sizeof(int), 1, in);
	return cliente;
}

int cmp_cliente(Cliente *c1, Cliente *c2)
{
	if (c1 == NULL) {
		return (c2 == NULL);
	}
	if (c1->cod_cliente != c2->cod_cliente) {
		return 0;
	}
	if (strcmp(c1->nome, c2->nome) != 0) {
		return 0;
	}
	if (c1->idade != c2->idade) {
		return 0;
	}
	return 1;
}

int tamanho_cliente()
{
	return sizeof(int) + // cod_cliente
		sizeof(char) * TAM_NOME + // nome
		sizeof(int); // idade
}


/*******************************************************************************************/

void imprime_clientes(ListaClientes *lc)
{
	int i;
	for (i = 0; i < lc->qtd; i++) {
		imprime_cliente(lc->lista[i]);
	}
}

ListaClientes *cria_clientes(int qtd, ...)
{
	va_list ap;
	ListaClientes *lc = (ListaClientes *)  malloc(sizeof(ListaClientes));
	lc->qtd = qtd;
	lc->lista = (Cliente **) malloc(sizeof(Cliente *) * (qtd));
	int i;
	va_start(ap, qtd);
	for (i = 0; i < qtd; i++) {
        lc->lista[i] = va_arg(ap, Cliente *);
    }
    va_end(ap);
    return lc;
}

void salva_clientes(char *nome_arquivo, ListaClientes *lc)
{
	FILE *out = fopen(nome_arquivo, "wb");
	int i;
	for (i = 0; i < lc->qtd; i++) {
		salva_cliente(lc->lista[i], out);
	}
	fclose(out);
}

ListaClientes * le_clientes(char *nome_arquivo)
{
	int qtd = 0;
	ListaClientes *lc = (ListaClientes *)  malloc(sizeof(ListaClientes));
	FILE *in = fopen(nome_arquivo, "rb");
	if (in != NULL) {
		Cliente *cliente = NULL;
		while((cliente = le_cliente(in)) != NULL) {
			qtd += 1;
			free(cliente);
		}
		fseek(in, 0, SEEK_SET);
		lc->qtd = qtd;
		lc->lista = (Cliente **) malloc(sizeof(Cliente *) * (qtd));
		qtd = 0;
		while((cliente = le_cliente(in)) != NULL) {
			lc->lista[qtd++] = cliente;
		}
		fclose(in);
	} else {
		lc->qtd = 0;
		lc->lista = NULL;
	}
	return lc;
}

int cmp_clientes(ListaClientes *c1, ListaClientes *c2)
{
	if (c1->qtd != c2->qtd) {
		return 0;
	}
	int i;
	for (i = 0; i < c1->qtd; i++) {
		if (!cmp_cliente(c1->lista[i], c2->lista[i])) {
			return 0;
		}
	}
	return 1;
}

void libera_clientes(ListaClientes *lc)
{
	int i;
	for (i = 0; i < lc->qtd; i++) {
		free(lc->lista[i]);
	}
	free(lc->lista);
	free(lc);
}


/******************************************/

void imprime_registro_indice_idade(RegistroIndiceIdade *registro_indice_idade)
{
	printf("%d, %d, %d\n", registro_indice_idade->idade, registro_indice_idade->pt, registro_indice_idade->q);
}

RegistroIndiceIdade *registro_indice_idade(int idade, int pt, int q)
{
	RegistroIndiceIdade *registro_indice_idade = (RegistroIndiceIdade *) malloc(sizeof(RegistroIndiceIdade));
	if (registro_indice_idade) memset(registro_indice_idade, 0, sizeof(RegistroIndiceIdade));
	registro_indice_idade->idade = idade;
	registro_indice_idade->pt = pt;
	registro_indice_idade->q = q;
        return registro_indice_idade;
}

void salva_registro_indice_idade(RegistroIndiceIdade *registro_indice_idade, FILE *out)
{
	fwrite(&registro_indice_idade->idade, sizeof(int), 1, out);
	fwrite(&registro_indice_idade->pt, sizeof(int), 1, out);
        fwrite(&registro_indice_idade->q, sizeof(int), 1, out);
}

RegistroIndiceIdade *le_registro_indice_idade(FILE *in)
{
	RegistroIndiceIdade *registro_indice_idade = (RegistroIndiceIdade *) malloc(sizeof(RegistroIndiceIdade));
	if (0 >= fread(&registro_indice_idade->idade, sizeof(int), 1, in)) {
		free(registro_indice_idade);
		return NULL;
	}
	fread(&registro_indice_idade->pt, sizeof(int), 1, in);
        fread(&registro_indice_idade->q, sizeof(int), 1, in);
	return registro_indice_idade;
}

int cmp_registro_indice_idade(RegistroIndiceIdade *r1, RegistroIndiceIdade *r2)
{
	if (r1 == NULL) {
		return (r2 == NULL);
	}
	if (r1->idade != r2->idade) {
		return 0;
	}
	if (r1->pt != r2->pt) {
		return 0;
	}
	if (r1->q != r2->q) {
		return 0;
	}return 1;
}

int tamanho_registro_indice_idade()
{
	return sizeof(int) + // idade
		sizeof(int) + // pt
		sizeof(int); // q
}

/***************************************************************************/

void imprime_registros(ListaRegistrosIndiceIdade *l)
{
	int i;
	for (i = 0; i < l->qtd; i++) {
		imprime_registro_indice_idade(l->lista[i]);
	}
}

ListaRegistrosIndiceIdade *cria_registros(ListaRegistroChaveSecundaria *lcs)
{
	ListaRegistrosIndiceIdade *lc = (ListaRegistrosIndiceIdade *)  malloc(sizeof(ListaRegistrosIndiceIdade));
	lc->lista = (RegistroIndiceIdade **) malloc(sizeof(RegistroIndiceIdade *) * (lcs->qtd));

	int i, j = -1;
	for (i = 0; i < lcs->qtd; i++) {
        if(i == 0 || lcs->lista[i]->idade != lc->lista[j]->idade)
		{
			j++;
			lc->lista[j] = (RegistroIndiceIdade*)malloc(sizeof(RegistroIndiceIdade));
			lc->lista[j]->idade = lcs->lista[i]->idade;
			lc->lista[j]->pt = lcs->lista[i]->endereco_disco;
			lc->lista[j]->q = 1;
		}
		else
		{
			lc->lista[j]->q++;
		}
    }

	lc->qtd = j + 1;
	salva_registros(NOME_ARQUIVO_INDICE, lc);
    return lc;
}

void salva_registros(char *nome_arquivo, ListaRegistrosIndiceIdade *l)
{
	FILE *out = fopen(nome_arquivo, "wb");
	int i;
	for (i = 0; i < l->qtd; i++) {
		salva_registro_indice_idade(l->lista[i], out);
	}
	fclose(out);
}

ListaRegistrosIndiceIdade * le_registros(char *nome_arquivo)
{
	int qtd = 0;
	ListaRegistrosIndiceIdade *l = (ListaRegistrosIndiceIdade *)  malloc(sizeof(ListaRegistrosIndiceIdade));
	FILE *in = fopen(nome_arquivo, "rb");
	if (in != NULL) {
		RegistroIndiceIdade *registro = NULL;
		while((registro = le_registro_indice_idade(in)) != NULL) {
			qtd += 1;
			free(registro);
		}
		fseek(in, 0, SEEK_SET);
		l->qtd = qtd;
		l->lista = (RegistroIndiceIdade **) malloc(sizeof(RegistroIndiceIdade *) * (qtd));
		qtd = 0;
		while((registro = le_registro_indice_idade(in)) != NULL) {
			l->lista[qtd++] = registro;
		}
		fclose(in);
	} else {
		l->qtd = 0;
		l->lista = NULL;
	}
	return l;
}

int cmp_registros(ListaRegistrosIndiceIdade *c1, ListaRegistrosIndiceIdade *c2)
{
	if (c1->qtd != c2->qtd) {
		return 0;
	}
	int i;
	for (i = 0; i < c1->qtd; i++) {
		if (!cmp_registro_indice_idade(c1->lista[i], c2->lista[i])) {
			return 0;
		}
	}
	return 1;
}

void libera_registros(ListaRegistrosIndiceIdade *l)
{
	int i;
	for (i = 0; i < l->qtd; i++) {
		free(l->lista[i]);
	}
	free(l->lista);
	free(l);
}

/***********************************************************************************/

ListaRegistroChavePonteiro *cria_lista_registro_chave_ponteiro(ListaRegistroChaveSecundaria *lc)
{
	ListaRegistroChavePonteiro *lcp = (ListaRegistroChavePonteiro *)malloc(sizeof(ListaRegistroChavePonteiro));

	lcp->lista = (RegistroChavePonteiro**)malloc(sizeof(RegistroChavePonteiro*) * lc->qtd);
	lcp->qtd = lc->qtd;
	int i;
	for(i = 0; i < lc->qtd; i++)
	{
		lcp->lista[i] = (RegistroChavePonteiro*)malloc(sizeof(RegistroChavePonteiro));

		lcp->lista[i]->endereco_disco = lc->lista[i]->endereco_disco;
		lcp->lista[i]->cod_cliente = lc->lista[i]->cod_cliente;
		lcp->lista[i]->idade = lc->lista[i]->idade;
		lcp->lista[i]->prox = (i + 1) < lc->qtd && lc->lista[i + 1]->idade == lcp->lista[i]->idade ? lc->lista[i + 1]->endereco_disco : -1 ;
	}

	salva_lista_registro_chave_ponteiro(NOME_ARQUIVO_CHAVE_PONTEIRO, lcp);

	return lcp;
}

void salva_lista_registro_chave_ponteiro(char *nome_arquivo, ListaRegistroChavePonteiro *lcp)
{
	FILE *out = fopen(nome_arquivo, "wb");
	int i;
	for (i = 0; i < lcp->qtd; i++) {
		fwrite(&lcp->lista[i]->endereco_disco, sizeof(int), 1, out);
		fwrite(&lcp->lista[i]->cod_cliente, sizeof(int), 1, out);
		fwrite(&lcp->lista[i]->idade, sizeof(int), 1, out);
		fwrite(&lcp->lista[i]->prox, sizeof(int), 1, out);
	}
	fclose(out);
}

void imprime_lista_registro_chave_ponteiro(ListaRegistroChavePonteiro *lcp)
{
	int i;
	for (i = 0; i < lcp->qtd; i++) {
		printf("%d	%d	%d	%d\n", lcp->lista[i]->endereco_disco, lcp->lista[i]->cod_cliente, lcp->lista[i]->idade, lcp->lista[i]->prox);
	}
}

ListaFinal* criar_lista_final(ListaClientes *lc, ListaRegistroChavePonteiro *lcp)
{
	ListaFinal *lf = (ListaFinal*)malloc(sizeof(ListaFinal));

	lf->lista = (RegistroFinal**)malloc(sizeof(RegistroFinal*) * lc->qtd);
	lf->qtd = lc->qtd;

	int i;
	for(i = 0; i < lc->qtd; i++)
	{
		lf->lista[i] = (RegistroFinal*)malloc(sizeof(RegistroFinal));

		lf->lista[i]->cod_cliente = lc->lista[i]->cod_cliente;
		strcpy(lf->lista[i]->nome, lc->lista[i]->nome);
		lf->lista[i]->idade = lc->lista[i]->idade;
		lf->lista[i]->prox = lcp->lista[i]->prox;
	}

	salvar_lista_final(NOME_ARQUIVO_FINAL, lf);
	return lf;
}

void salvar_lista_final(char *nome_arquivo, ListaFinal *lf)
{
	FILE *out = fopen(nome_arquivo, "wb");
	int i;
	for (i = 0; i < lf->qtd; i++) {
		fwrite(&lf->lista[i]->cod_cliente, sizeof(int), 1, out);
		fwrite(&lf->lista[i]->nome, sizeof(char), sizeof(lf->lista[i]->nome), out);
		fwrite(&lf->lista[i]->idade, sizeof(int), 1, out);
		fwrite(&lf->lista[i]->prox, sizeof(int), 1, out);
	}
	fclose(out);
}

void imprime_lista_final(ListaFinal *lf)
{
	int i;
	for (i = 0; i < lf->qtd; i++) 
	{
		printf("%d	%s		%d	%d\n", lf->lista[i]->cod_cliente, lf->lista[i]->nome, lf->lista[i]->idade, lf->lista[i]->prox);
	}
}