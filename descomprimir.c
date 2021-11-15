#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct
{
    unsigned char key;
    unsigned char value;

}DictByte;

int sizeFile(FILE *p) {
    int n;
    fseek(p, 0, SEEK_END);
    n = ftell(p);
    printf("Tamanho: %d bytes\n", n);
    rewind(p);
    return n;
}


int unzipBitMap(FILE *pRead, FILE *pWrite) {
    
    int c, i=0, bitMap, C, resto, tamanho;

    tamanho = sizeFile(pRead);

    C = fgetc(pRead);
    resto = fgetc(pRead);
    resto = resto & 15;

    while( (c = fgetc(pRead)) != EOF) {

        if(ftell(pRead) <= (tamanho - resto)){
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

int main()
{
    char nome[100], nome2[100];
    int i = 0, n = 0, tamanho;
    FILE *pRead, *pWrite, *pTemp;

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

    if(pRead == NULL || pWrite == NULL || pTemp == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        exit(1);
    }

    unzipMeioByte(pRead, pTemp);

    fclose(pRead);
    fclose(pTemp);
    pTemp = fopen("temp.txt", "r");

    unzipBitMap(pTemp, pWrite);

    fclose(pTemp);
    fclose(pWrite);
    remove("temp.txt");

    return 1;   
}
