#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct 
{
	unsigned char *codificacao;
	int caractere;
}huffmanMap;

huffmanMap *inicializarMapa(int quantidade) {

	int i = 0;
	huffmanMap *mapa;

	mapa = malloc(quantidade*sizeof(huffmanMap));

	if(mapa == NULL){
		printf("Não foi possível alocar a memória");
		exit(1);
	}

	for(int i = 0; i < quantidade; i++){
		mapa[i].codificacao = malloc(quantidade*sizeof(unsigned char));
	}

	return mapa;
}

int main() {

    int quantidade, i = 0, j = 0, c, continua = 1;
	char codigo[256] = "";
	unsigned char delimiter;
	huffmanMap *mapa = NULL;
	FILE *pRead, *pWrite;

	pRead = fopen("mapa.txt", "r");
	pWrite = fopen("mapa-unzip.txt", "w");

	if (pRead == NULL)
	{
		printf("Erro ao abrir o arquivo!\n");
		exit(1);
	}

    quantidade = fgetc(pRead);
    delimiter = fgetc(pRead);

    mapa = inicializarMapa(quantidade);

    while (continua)
    {
        c = fgetc(pRead);
        mapa[i].caractere = c;

        while((c=fgetc(pRead)) != delimiter){
            codigo[j] = c;
            j++;
        }

        codigo[j] = '\0';
        strcpy(mapa[i].codificacao, codigo);

        i++;
        j = 0;
        //verifica fim cabeçalho
        if((c=fgetc(pRead)) == delimiter){
            continua = 0;
        }else{
            fseek(pRead, -1, SEEK_CUR);
        }
    }

    while ((c=fgetc(pRead))  != EOF)
    {
        /* code */
    }
    

     
        


}