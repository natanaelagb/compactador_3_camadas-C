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






No *scanner(FILE *p, int *quantidade, int *delimiter)
{
	int caracteres[256] = {0}, sum_freq = 0;
	int c, n = 0;
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
		if (caracteres[i] > 0)
		{
			printf("%d\n", i);
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
		}else if(i<126){
			*delimiter = i;
		}
	}

	*quantidade = n;

	rewind(p);

	return noCabeca;
}




void imprimir(No *cabeca)
{
	No *no = cabeca->prox;

	while (no != NULL)
	{
		printf("char: %d freq: %d\n", no->caracter, no->frequencia);
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

}









void codHuffman(FILE *pRead, FILE *pWrite, huffmanMap *mapa, int quantidade, int delimiter){

	int i = 0, j = 0, k = 0, byte = 0, c;
	unsigned char bit;
	
	fputc(quantidade, pWrite);
	fputc(delimiter, pWrite);
	for (i = quantidade-1; i >= 0; i--)
	{
		fputc(mapa[i].caractere, pWrite);
		fputs(mapa[i].codificacao, pWrite);
		fputc(delimiter, pWrite);
	}
	fputc(delimiter, pWrite);

	while((c = fgetc(pRead)) != EOF){

		for (i = quantidade-1; i >= 0; i--)
		{
			if(mapa[i].caractere == c){
				while(mapa[i].codificacao[j] != '\0' || k == 8){
					if(k < 8){
						bit = mapa[i].codificacao[j];
						
						if(bit == '1'){
							byte = byte | 1;
							byte = byte<<1;
							// printf("1");
						}else {
							byte = byte<<1;
							// printf("0");
						}
						j++;
						k++;
					}else{
						// printf("\n");
						byte = byte>>1;
						fputc(byte, pWrite);
						k = 0;
						byte = 0;
					}
				}

				j = 0;
				break;
			}
		}
	}

	if(k>0){
		byte = byte<<(8-(k+1));
		// printf("\n%d", byte);
		fputc(byte, pWrite);
	}

	fclose(pWrite);

}


void montarMapa(No* no, huffmanMap *mapa, char *codigo, int nivel, unsigned char c){
	
	if(c != '\0'){
		codigo[nivel-1] = c;
	}
	
	if(no->esquerda == NULL){
	 	codigo[nivel] = '\0';
		printf("char: %c codigo: %s\n", no->caracter, codigo);
		int i=0;
		while (mapa[i].caractere != no->caracter)
		{
			i++;
		}
		strcpy(mapa[i].codificacao, codigo);

		return;
	}

	montarMapa(no->esquerda, mapa, codigo, nivel+1, '1');
	montarMapa(no->direita, mapa, codigo, nivel+1, '0');

	return;
}


huffmanMap *inicializarMapa(No *cabeca, int quantidade) {

	int i = 0;
	huffmanMap *mapa;
	No *c = cabeca->prox;

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

	for(int i = 0; i < quantidade; i++){
		mapa[i].codificacao = malloc(quantidade*sizeof(unsigned char));
	}

	return mapa;
}




int main()
{
	int quantidade, i = 0;
	char codigo[256] = "";
	int delimiter;
	No *cabeca;
	No *c;
	huffmanMap *mapa = NULL;
	FILE *pRead, *pWrite;

	pRead = fopen("meio.txt", "r");
	pWrite = fopen("mapa.txt", "w");

	if (pRead == NULL || pWrite == NULL)
	{
		printf("Erro ao abrir o arquivo!\n");
		exit(1);
	}

	cabeca = scanner(pRead, &quantidade, &delimiter);
	//imprimir(cabeca);
	//puts("----------------------------------");
	selectionSort(cabeca);
	imprimir(cabeca);
	puts("----------------------------------");
	mapa = inicializarMapa(cabeca, quantidade);
	arvoreHuffman(cabeca);
	montarMapa(cabeca->prox, mapa, codigo, 0, '\0');
	codHuffman(pRead, pWrite, mapa, quantidade, delimiter);

	printf("\nquant: %d delimiter: %d", quantidade, delimiter);
	strcpy(codigo, mapa[0].codificacao);
	c = cabeca->prox;

	for (int i = 0; i < quantidade-1; i++)
	{
		if(codigo[i] == '1'){
			c = c->esquerda;
		}else{
			c = c->direita;
		}
	}

	printf("\n%c %d\n", c->caracter, c->frequencia);
	
}
