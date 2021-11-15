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

int sizeFile(FILE *p)
{
    int n;
    fseek(p, 0, SEEK_END);
    n = ftell(p);
    printf("Tamanho: %d bytes\n", n);
    rewind(p);
    return n;
}

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
    printf("%d\n", resto);

    rewind(pWrite);

    fwrite(cabecalho, 2, sizeof(char), pWrite);
    fclose(pWrite);

    return 1;
}

int printByte(unsigned char ch, unsigned char byte[])
{

    itoa(ch, byte, 2);
    printf("%s\n", byte);

    return 1;
}

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
    //
    // Cabeçalho possui 10 bytes
    //

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
                buffer = realloc(buffer, (i + 1) * sizeof(unsigned char));
                buffer[i] = c;
                i++;
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

int main()
{
    char nome[100], nome2[100], C;
    int i = 0, n = 0, tamanho;
    FILE *pRead, *pWrite, *pTemp;
    Zero zero;

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

    if (pRead == NULL || pWrite == NULL || pTemp == NULL)
    {
        printf("Erro ao abrir o arquivo!\n");
        exit(1);
    }

    sizeFile(pRead);
    C = searchOptimal(pRead);
    bitMap(pRead, pTemp, C);

    fclose(pRead);
    fclose(pTemp);
    pTemp = fopen("temp.txt", "r");

    zero = searchNone(pTemp);
    zipMeioByte(pTemp, pWrite, zero);
    
    fclose(pTemp);
    fclose(pWrite);
    remove("temp.txt");

    return 1;
}
