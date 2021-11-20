//Compactador de 3 camadas: Bitmap, MeioByte e Huffman
//Autor: Natanael Aguilar Barreto
//


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct
{
    unsigned char key;
    unsigned char value;

} DictByte;

typedef struct
{
    unsigned char *none;
    int n_none;
} Zero;

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


int sizeFile(FILE *p)
{
    int n;
    fseek(p, 0, SEEK_END);
    n = ftell(p);
    printf("Tamanho: %d bytes\n", n);
    rewind(p);
    return n;
}

int printByte(unsigned char ch, unsigned char byte[])
{

    itoa(ch, byte, 2);
    printf("%s\n", byte);

    return 1;
}

//
//  COMPRESSÃO: BITMAP
//

char searchOptimal(FILE *p)
{

    int caracteres[256] = {0}, maior;
    char indice, c;

    while ((c = fgetc(p)) != EOF)
    {
        caracteres[c]++;
    }
    maior = caracteres[0];
    indice = 0;
    for (int i = 1; i < 256; i++)
    {
        if (caracteres[i] > maior)
        {
            maior = caracteres[i];
            indice = i;
        }
    }

    printf("O caractar %d se repete %d vezes \n", indice, maior);
    rewind(p);

    return indice;
}

int bitMap(FILE *pRead, FILE *pWrite, char C)
{

    int map = 0, n = 0, i = 0, resto = 0, c;
    unsigned char temp[9] = "", caracteres[8] = "", bi[8], cabecalho[2];

    //bytes reservados para o cabeçalho
    fseek(pWrite, 2, SEEK_SET);

    do
    {
        if ((c = fgetc(pRead)) != EOF)
        {
            if (i < 8)
            {
                if (c != C)
                {
                    map = map | 1;
                    temp[n] = c;
                    n++;
                }
                caracteres[i] = c;
                map = map << 1;
                i++;
            }
            else
            {
                map = map >> 1;

                fputc(map, pWrite);
                fwrite(temp, n, sizeof(char), pWrite);

                map = 0;
                n = 0;
                i = 0;
                fseek(pRead, -1, SEEK_CUR);
            }
        }
        else if (i == 8)
        {

            map = map >> 1;

            fputc(map, pWrite);
            fwrite(temp, n, sizeof(char), pWrite);

            map = 0;
            n = 0;
            i = 0;
        }
        else if (i > 0)
        {
            resto = i;
            fwrite(caracteres, i, sizeof(char), pWrite);
        }

    } while (c != EOF);

    cabecalho[0] = C;
    cabecalho[1] = resto | 48;
    cabecalho[2] = '\0';
    printf("resto bit: %d\n", resto);

    rewind(pWrite);

    fwrite(cabecalho, 2, sizeof(char), pWrite);
    fclose(pWrite);

    return 1;
}

//
//
//  COMPRESSÃO: MEIO-BYTE
//
//

Zero searchNone(FILE *p)
{
    int caracteres[256] = {0}, indice, c, n = 0;
    Zero zero;

    zero.none = NULL;
    zero.n_none = 0;

    while ((c = fgetc(p)) != EOF)
    {
        caracteres[c]++;
    }

    for (int i = 0; i < 256; i++)
    {
        if (i >= 7 && i <= 13)
            continue;

        {
            if (caracteres[i] == 0)
            {
                n++;
                zero.none = realloc(zero.none, n * sizeof(unsigned char));
                zero.none[n - 1] = i;
            }
        }
    }

    zero.n_none = n;

    rewind(p);

    return zero;
}

int zipMeioByte(FILE *pRead, FILE *pWrite, Zero zero)
{
    // Cabeçalho possui 10 bytes

    int mask = 240, meioByte, i = 0, byte, c, ce = 0;
    unsigned char *buffer = NULL, c1, byteimpresso[8];
    DictByte dict[5];

    if (zero.n_none < 5)
    {
        printf("Nao e possivel comprimir o arquivo!\n");
        return 0;
    }

    dict[0].key = zero.none[0];
    dict[0].value = 48;
    dict[1].key = zero.none[1];
    dict[1].value = 64;
    dict[2].key = zero.none[2];
    dict[2].value = 80;
    dict[3].key = zero.none[3];
    dict[3].value = 96;
    dict[4].key = zero.none[4];
    dict[4].value = 112;

    fwrite(dict, 5, sizeof(DictByte), pWrite);

    do
    {

        if ((c = fgetc(pRead)) != EOF)
        {
            if (i == 0)
            {
                meioByte = c & mask;
                
                 for (int k = 0; k < 5; k++)
                {
                    if (meioByte == dict[k].value)
                    {
                        buffer = realloc(buffer, (i + 1) * sizeof(unsigned char));
                        buffer[i] = c;
                        i++;
                    }
                }

                if(i == 0){
                    fwrite(&c, 1, sizeof(unsigned char), pWrite);
                }
            }
            else if ((c & mask) == meioByte && i < 15)
            {
                buffer = realloc(buffer, (i + 1) * sizeof(unsigned char));
                buffer[i] = c;
                i++;
            }
            else if (i > 5)
            {
                for (int k = 0; k < 5; k++)
                {
                    if (meioByte == dict[k].value)
                    {
                        ce = dict[k].key;
                    }
                }

                fputc(ce, pWrite);

                for (int j = 0; j < i; j++)
                {
                    if (j == 0)
                    {
                        byte = i << 4;
                        c1 = buffer[j];
                        byte = byte | (c1 & 15);
                        byte = byte & 255;
                    }
                    else
                    {
                        c1 = buffer[j];
                        byte = c1 << 4;
                        byte = byte & 255;
                        if (++j < i)
                        {
                            c1 = buffer[j];
                            byte = byte | (c1 & 15);
                        }
                    }

                    fputc(byte, pWrite);
                }
                fseek(pRead, -1, SEEK_CUR);
                i = 0;
            }
            else
            {

                fwrite(buffer, i, sizeof(char), pWrite);
                fseek(pRead, -1, SEEK_CUR);
                i = 0;
            }
        }
        else if (i > 5)
        {

            for (int k = 0; k < 5; k++)
            {
                if (meioByte == dict[k].value)
                {
                    ce = dict[k].key;
                }
            }

            fputc(ce, pWrite);

            for (int j = 0; j < i; j++)
            {
                if (j == 0)
                {
                    byte = i << 4;
                    c1 = buffer[j];
                    byte = byte | (c1 & 15);
                    byte = byte & 255;
                }
                else
                {
                    c1 = buffer[j];
                    byte = c1 << 4;
                    byte = byte & 255;
                    if (++j < i)
                    {
                        c1 = buffer[j];
                        byte = byte | (c1 & 15);
                    }
                }

                fputc(byte, pWrite);
            }
        }
        else
        {
            fwrite(buffer, i, sizeof(char), pWrite);
        }

    } while (c != EOF);
}


//
//
// COMPRESSÃO HUFFMAN
//
//

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
		}else{
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


void montarMapa(No* no, huffmanMap *mapa, char *codigo, int nivel, unsigned char c){
	
	if(c != '\0'){
		codigo[nivel-1] = c;
	}
	
	if(no->esquerda == NULL){
	 	codigo[nivel] = '\0';
		// printf("char: %c codigo: %s\n", no->caracter, codigo);
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

void huffmanZip(FILE *pRead, FILE *pWrite, huffmanMap *mapa, int quantidade, int delimiter){

	int i = 0, j = 0, k = 0, byte = 0, offset = 0, c;
	unsigned char bit;
	fputc(quantidade, pWrite);
	fputc(delimiter, pWrite);
	fseek(pWrite, 1, SEEK_CUR);

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
						}

                        byte = byte<<1;
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
		printf("\nresto: %d", k);
		fputc(byte, pWrite);
		rewind(pWrite);
		fseek(pWrite, 2, SEEK_SET);
		fputc(k, pWrite);   
	}else{
        printf("\nresto: %d", k);
        rewind(pWrite);
		fseek(pWrite, 2, SEEK_SET);
		fputc(k, pWrite);  
    }

	fclose(pWrite);

}




int main()
{
    char nome[100], nome2[100], codigo[256] = "", C;
    int i = 0, n = 0, tamanho, quantidade, delimiter;
    FILE *pRead, *pWrite, *pTemp, *pTemp2;
    Zero zero;
    No *cabeca;
    huffmanMap *mapa = NULL;

    printf("Nome do arquivo que sera compactado: ");
    setbuf(stdin, NULL);
    fgets(nome, 100, stdin);

    nome[strcspn(nome, "\n")] = '\0';

    strcpy(nome2, nome);

    tamanho = strlen(nome);

    for (int i = 0; i < tamanho; i++)
    {
        if (nome[i] == '.')
        {
            nome2[i] = '\0';
            strcat(nome2, "-zip.txt");
        }
    }

    pRead = fopen(nome, "r");
    pWrite = fopen(nome2, "w");
    pTemp = fopen("temp.txt", "w");
    pTemp2 = fopen("temp2.txt", "w");

    if (pRead == NULL || pWrite == NULL || pTemp == NULL || pTemp2 == NULL)
    {
        printf("Erro ao abrir o arquivo!\n");
        exit(1);
    }

    //
    // 1ª CAMADA: BITMAP
    //
    sizeFile(pRead);
    C = searchOptimal(pRead);
    bitMap(pRead, pTemp, C);

    fclose(pRead);
    fclose(pTemp);
    pTemp = fopen("temp.txt", "r");

    //
    // 2ª CAMADA: MEIOBYTE
    //

    zero = searchNone(pTemp);
    zipMeioByte(pTemp, pTemp2, zero);

    fclose(pTemp);
    fclose(pTemp2);
    pTemp2 = fopen("temp2.txt", "r");

    //
    // 3ª CAMADA: HUFFMAN
    //

    cabeca = scanner(pTemp2, &quantidade, &delimiter);
	//imprimir(cabeca);
	//puts("----------------------------------");
	selectionSort(cabeca);
	imprimir(cabeca);
	puts("----------------------------------");
	mapa = inicializarMapa(cabeca, quantidade);
	arvoreHuffman(cabeca);
	montarMapa(cabeca->prox, mapa, codigo, 0, '\0');
	huffmanZip(pTemp2, pWrite, mapa, quantidade, delimiter);


    
    fclose(pTemp2);
    fclose(pWrite);
    remove("temp.txt");
    remove("temp2.txt");

    return 1;
}
