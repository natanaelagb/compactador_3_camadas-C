#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	int caracter;
	int frequencia;
} Caracter;

struct No
{
	int caracter;
	int frequencia;
	struct No *prox;
	struct No *ant;
	struct No *esquerda;
	struct No *direita;
};
typedef struct No No;

typedef struct 
{
	unsigned char *codificacao;
	int caractere;

}huffmanMap;


No *scanner(FILE *p, int *quantidade)
{
	int caracteres[256] = {0}, sum_freq = 0;
	char c, n = 0;
	No *no, *noAnt, *noCabeca;

	no = malloc(sizeof(No));
	no->prox = NULL;
	no->ant = NULL;
	no->frequencia = 0;

	noCabeca = noAnt = no;

	while ((c = fgetc(p)) != EOF)
	{
		caracteres[c]++;
	}

	for (int i = 0; i < 256; i++)
	{
		if (caracteres[i] > 2)
		{
			n++;
			no = malloc(sizeof(No));
			no->prox = NULL;
			no->ant = noAnt;
			no->frequencia = caracteres[i];
			no->caracter = i;
			no->esquerda = NULL;
			no->direita = NULL;

			noAnt->prox = no;
			noAnt = no;
		}
	}

	*quantidade = n;

	return noCabeca;
}

void imprimir(No *cabeca)
{
	No *no = cabeca->prox;

	while (no != NULL)
	{
		printf("char: %c freq: %d\n", no->caracter, no->frequencia);
		no = no->prox;
	}
}

void imprimirInverso(No *cabeca)
{
	No *no = cabeca->prox;

	while (no->prox != NULL)
	{
		no = no->prox;
	}

	while (no != cabeca)
	{
		printf("char: %c freq: %d\n", no->caracter, no->frequencia);
		no = no->ant;
	}
}

void *selectionSort(No *cabeca)
{
	No *noAux;
	No *min, *minAnt, *minProx;
	No *no = cabeca->prox, *noAnt, *noProx;

	while (no->prox != NULL)
	{
		noAnt = no->ant;
		noProx = no->prox;

		min = no;
		noAux = no->prox;
		while (noAux != NULL)
		{

			if (noAux->frequencia < min->frequencia)
			{
				minAnt = noAux->ant;
				minProx = noAux->prox;
				min = noAux;
			}
			noAux = noAux->prox;
		}

		if (no != min)
		{

			if (noProx == min)
			{
				min->ant = noAnt;
				min->prox = no;
				no->ant = min;
				no->prox = minProx;

				if(minProx != NULL) {
            		minProx->ant = no;
        		}
        		noAnt->prox = min;
			}
			else
			{
				noProx->ant = min;
				noAnt->prox = min;

				minAnt->prox = no;
				if (minProx != NULL)
				{
					minProx->ant = no;
				}

				min->prox = noProx;
				min->ant = noAnt;

				no->ant = minAnt;
				no->prox = minProx;
			}

			no = min->prox;
		}
		else
		{
			no = no->prox;
		}
	}
}


void arvoreHuffman(No *cabeca){

	No *no1, *no2, *novoNo;
	
	if(cabeca->prox->prox == NULL){
		printf("Não compactável\n");
		exit(1);
	}

	no1 = cabeca->prox;

	while (no1->prox != NULL)
	{	
		no2 = no1->prox;
		novoNo = malloc(sizeof(No));

		novoNo->frequencia = no1->frequencia + no2->frequencia;
		novoNo->ant = no1->ant;
		novoNo->prox = no2->prox;
		novoNo->caracter = '?';
		novoNo->esquerda = no1;
		novoNo->direita = no2;

		// imprimir(cabeca);
		// puts("---------------------------------------------");

		cabeca->prox = novoNo;
		no1 = cabeca->prox;
	}	

	//imprimir(cabeca);
}

int tamanhoArvore(No* no, int n, int* max){
    if(no->esquerda != NULL){
        n = tamanhoArvore(no->esquerda,n+1,max);
    }
    if(no->direita != NULL){
        n = tamanhoArvore(no->direita,n+1,max);
    }

    if(n > *max){
        *max = n;
    }
    return (n-1);
}


void montarMapa(No* no, huffmanMap *mapa, char *codigo, int nivel, unsigned char c){

	// // printf("%d\n", nivel);
    // if(no->esquerda != NULL ){
	// 	codigo = realloc(codigo, nivel*sizeof(char));
	// 	codigo[nivel-1] = '1';
    //     montarMapa(no->esquerda, mapa, codigo, nivel+1);
    // }else{
	// 	codigo = realloc(codigo, nivel*sizeof(char));
	// 	codigo[nivel] = '\0';
	// 	// int i = 0;
	// 	// while (no->caracter != mapa[i].caractere)
	// 	// {
	// 	// 	i++;
	// 	// }
		
	// 	// strcpy(mapa[i].codificacao, codigo); 

	// 	printf("char: %c freq: %d\n", no->caracter, no->frequencia);
	// 	printf("codigo: %s\n\n", codigo);		
	// }
    
    // if(no->direita != NULL){
	// 	codigo = realloc(codigo, nivel*sizeof(char));
	// 	codigo[nivel-1] = '0';
    //     montarMapa(no->direita, mapa, codigo, nivel+1);
    // }else{
	// 	codigo = realloc(codigo, nivel*sizeof(char));
	// 	codigo[nivel] = '\0';
	// 	// int i = 0;
	// 	// while (no->caracter != mapa[i].caractere)
	// 	// {
	// 	// 	i++;
	// 	// }
		
	// 	// strcpy(mapa[i].codificacao, codigo); 
	// 	//printf("char: %c freq: %d\n ", no->caracter, no->frequencia);
	// 	printf("char: %c freq: %d\n", no->caracter, no->frequencia);
	// 	printf("codigo: %s\n\n", codigo);	
	// }


	if(no->esquerda == NULL || no->direita == NULL){
		//codigo = realloc(codigo, nivel*sizeof(char));
	 	//codigo[nivel-1] = '\0';
		printf("CHAR: %c\n", no->caracter);
		return;
	}


	codigo = realloc(codigo, nivel*sizeof(char));
	codigo[nivel-1] = c;

	montarMapa(no->esquerda, mapa, codigo, nivel+1, '1');
	montarMapa(no->direita, mapa, codigo, nivel+1, '0');


	return;

}

huffmanMap *inicializarMapa(No *cabeca, int quantidade, huffmanMap *map, int tamanho) {

	int i = 0;
	huffmanMap *mapa = map;
	No *c = cabeca->prox;

	if(map == NULL){
		mapa = malloc(quantidade*sizeof(huffmanMap));

		if(mapa == NULL){
			printf("Não foi possível alocar a memória");
			exit(1);
		}

		while(c != NULL)
		{	
			mapa[i].caractere = c->caracter;
			c = c->prox;
			i++;
		}
	}else {

		for(int i = 0; i < quantidade; i++){
			mapa[i].codificacao = malloc(tamanho*sizeof(unsigned char));
		}
	}

	return mapa;
}

int main()
{
	int tamanho, quantidade, i = 0;
	No *cabeca;
	No *c;
	huffmanMap *mapa = NULL;
	FILE *pRead;

	pRead = fopen("meio.txt", "r");

	if (pRead == NULL)
	{
		printf("Erro ao abrir o arquivo!\n");
		exit(1);
	}

	cabeca = scanner(pRead, &quantidade);
	//imprimir(cabeca);
	//puts("----------------------------------");
	selectionSort(cabeca);
	imprimir(cabeca);
	puts("----------------------------------");

	mapa = inicializarMapa(cabeca, quantidade, mapa, 0);
	arvoreHuffman(cabeca);
	tamanhoArvore(cabeca->prox, 1, &tamanho);
	mapa = inicializarMapa(cabeca, quantidade, mapa, tamanho);
	// imprimir(cabeca);
	//montarMapa(cabeca->prox, mapa, NULL, 1, '1');
	//printf("c:%c f:%d\n", cabeca->prox->esquerda->esquerda->esquerda->esquerda->caracter, cabeca->prox->esquerda->esquerda->esquerda->esquerda->frequencia);

	// for (int i = 0; i < quantidade; i++)
	// {
	// 	printf("char: %c codificacao: %s\n", mapa[i].caractere, mapa[i].codificacao);
	// }

	// for (No* n = cabeca->prox; n != NULL; n = n->prox)
	// {
	// 	printf("char: %c freq: %d\n", n->caracter, n->frequencia);
	// }
	
	

	



	printf("Tamanho da arvore: %d\n", tamanho);
	// imprimir(cabeca);
  	// puts("----------------------------------------------\n");
	// selectionSort(cabeca);
	// imprimir(cabeca);
	

	/*

  for (int i = 0; i < cabeca_char.tamanho; i++)
  {
    printf("char: %c freq: %d\n", array_char[i].caracter, array_char[i].frequencia);
  }

  puts("----------------------------------------------\n");

  mergeSort(array_char, 0, cabeca_char.tamanho - 1);

  for (int i = 0; i < cabeca_char.tamanho; i++)
  {
    printf("char: %c freq: %d\n", array_char[i].caracter, array_char[i].frequencia);
  }*/
}
