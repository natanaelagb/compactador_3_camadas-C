//Descompactador de 3 camadas: Bitmap, MeioByte e Huffman
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

}DictByte;

typedef struct 
{
	unsigned char *codificacao;
	int caractere;
}huffmanMap;


int sizeFile(FILE *p) {
    int n;
    fseek(p, 0, SEEK_END);
    n = ftell(p);
    printf("Tamanho: %d bytes\n", n);
    rewind(p);
    return n;
}


//
// DESCOMPRESSÃO BITMAP
//

int unzipBitMap(FILE *pRead, FILE *pWrite) {
    
    int c, i=0, bitMap, C, resto, tamanho;

    tamanho = sizeFile(pRead);

    C = fgetc(pRead);
    resto = fgetc(pRead);
    resto = resto & 15;

    while( (c = fgetc(pRead)) != EOF) {

        if(ftell(pRead) < (tamanho - resto -1)){
            bitMap = c;
            for(i=0; i<8; i++){
                if(bitMap & (int)pow(2,7-i)){
                    c = getc(pRead);
                    if(c == EOF ) {
                        break;
                    }
                    fputc(c, pWrite);
                }else{
                    fputc(C, pWrite);
                }
            }
        }else {
            fputc(c, pWrite);
        }

    }
}

//
// DESCOMPRESSÃO MEIOBYTE
//

int unzipMeioByte(FILE *pRead, FILE *pWrite)
{
    int c, ce, meioByte, byte, n, seq = 0, i;
    DictByte dict[5];

    fread(dict, 5, sizeof(DictByte), pRead);
 
    while ((c = fgetc(pRead)) != EOF)
    {
        ce = c;
        for (i = 0; i < 5; i++)
        {
            if(ce == dict[i].key){
                seq = 1;
                meioByte = dict[i].value;
            }
        }

        if(seq){
            byte = fgetc(pRead);
            n = byte >> 4;
            byte = (byte & 15) | meioByte;
            fputc(byte, pWrite);

            for (int i = 0; i < n - 1; i++)
            {
                c = fgetc(pRead);
                byte = c >> 4;
                byte = byte | meioByte;
                fputc(byte, pWrite);

                if (++i < n - 1)
                {
                    byte = c & 15;
                    byte = byte | meioByte;
                    fputc(byte, pWrite);
                }
            }
            
            seq = 0;
        }else{
            fputc(c, pWrite);
        }           
    }
}


//
// DESCOMPRESSÃO HUFFMAN
//
huffmanMap *inicializarMapa(int quantidade) {

	int i = 0;
	huffmanMap *mapa;

	mapa = malloc(quantidade*sizeof(huffmanMap));

	if(mapa == NULL){
		printf("Não foi possível alocar a memoria!");
		exit(1);
	}

	for(int i = 0; i < quantidade; i++){
		mapa[i].codificacao = malloc(quantidade*sizeof(unsigned char));
	}

	return mapa;
}

void huffmanUnzip(FILE *pRead, FILE *pWrite) {
    int quantidade, i = 0, j = 0, c, continua = 1, resto, restoCod, posicao, tamanho;
	char codigo[256] = "";
	unsigned char delimiter;
	huffmanMap *mapa = NULL;

    fseek(pRead, 0, SEEK_END);
    tamanho = ftell(pRead);
    rewind(pRead);

    quantidade = fgetc(pRead);
    delimiter = fgetc(pRead);
    resto = fgetc(pRead);

    printf("quant: %d\n", quantidade);
    
    
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
    
    j = 0;

    while ((c = fgetc(pRead))  != EOF )
    {
        posicao = ftell(pRead);
        for (i = 7; i >= 0; i--)
        {
            if(posicao == tamanho && (8-i) > resto && resto != 0){
                break;
            }else{
                if((c>>i & 1) == 1){
                    codigo[j] = '1';
                }else {
                    codigo[j] = '0';
                }
            }
        
            j++;
            codigo[j] = '\0';
            for (int k = 0; k < quantidade; k++)
            {
                if (strcmp(codigo, mapa[k].codificacao) == 0)
                {
                    fputc(mapa[k].caractere, pWrite);
                    j = 0;
                }
                
            }         
        }
    }
}



int main()
{
    char nome[100], nome2[100];
    int i = 0, n = 0, tamanho;
    FILE *pRead, *pWrite, *pTemp, *pTemp2;

    printf("Nome do arquivo que sera descompactado: ");
    setbuf(stdin, NULL);
    fgets(nome, 100, stdin);

    nome[strcspn(nome, "\n")] = '\0';

    strcpy(nome2, nome);

    tamanho = strlen(nome);

    for (int i = 0; i < tamanho; i++)
    {
        if(nome[i] == '.'){
            nome2[i-4] = '\0';
            strcat(nome2, "-unzip.txt");
        }
    }

    pRead = fopen(nome, "r");
    pWrite = fopen(nome2, "w");
    pTemp = fopen("temp.txt", "w");
    pTemp2 = fopen("temp2.txt", "w");

    if(pRead == NULL || pWrite == NULL || pTemp == NULL || pTemp2 == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        exit(1);
    }

    //
    // 3ª CAMADA: HUFFMAN
    //

    printf("end: %d\n", pRead);

    huffmanUnzip(pRead, pTemp);

    fclose(pRead);
    fclose(pTemp);
    pTemp = fopen("temp.txt", "r");

    //
    // 2ª CAMADA: MEIOBYTE
    //

    unzipMeioByte(pTemp, pTemp2);

    fclose(pTemp);
    fclose(pTemp2);
    pTemp2 = fopen("temp2.txt", "r");

    //
    // 1ª CAMADA: BITMAP
    //
    unzipBitMap(pTemp2, pWrite);

    fclose(pTemp2);
    fclose(pWrite);
    remove("temp.txt");
    remove("temp2.txt");

    return 1;   
}
