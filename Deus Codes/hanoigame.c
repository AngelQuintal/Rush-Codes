#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define torre_A 'A'
#define torre_B 'B'
#define torre_C 'C'


#define DISC_CHARACTER '*'


unsigned int numeroDeDiscos = 0;
char *torre;

void rewindCursor(const unsigned int lineaas, const unsigned int columnaas)
{
    printf("\x1b[%dA", lineaas);
    printf("\x1b[%dD", columnaas);
}

void printCentered(char *str, unsigned int size)
{
    const unsigned int espacios = (size - strlen(str));
    const unsigned int espaciosAntes = espacios / 2;
    const unsigned int espaciosDespues = espaciosAntes + espacios % 2;
    for (unsigned int i = 0; i < espaciosAntes; i++)
        putchar(' ');
    printf("%s", str);
    for (unsigned int i = 0; i < espaciosDespues; i++)
        putchar(' ');
}

void printHeader()
{
    printCentered("torre A", 2 * numeroDeDiscos);
    printCentered("torre B", 2 * numeroDeDiscos);
    printCentered("torre C", 2 * numeroDeDiscos);
    putchar('\n');
}

void printtorre()
{
    static const unsigned int offset = 1;
    const unsigned int basedeisco = 2 * numeroDeDiscos - 1;
    const unsigned int NumeroDecolumnaas = 3 * (basedeisco + offset);
    unsigned int a = numeroDeDiscos, b = numeroDeDiscos, c = numeroDeDiscos;
    char tabla[numeroDeDiscos][NumeroDecolumnaas];
    memset(tabla, ' ', sizeof(tabla));

    for (unsigned int i = numeroDeDiscos - 1; i < numeroDeDiscos; i--)
    {
        unsigned int linea;
        const unsigned int tamdisco = basedeisco - 2 * (numeroDeDiscos - i - 1);
        const unsigned int columna = (torre[i] - torre_A) * (basedeisco + offset) + numeroDeDiscos - i + offset - 1;
        if (torre[i] == torre_A)
            linea = --a;
        else if (torre[i] == torre_B)
            linea = --b;
        else if (torre[i] == torre_C)
            linea = --c;
        memset(&tabla[linea][columna], DISC_CHARACTER, tamdisco);
    }
    for (unsigned int linea = 0; linea < numeroDeDiscos; linea++)
    {
        printf("%.*s\n", NumeroDecolumnaas, tabla[linea]);
    }
}

void move(unsigned int n, char a, char b)
{
    static unsigned int steps = 0;
    sleep(1);
    torre[n - 1] = b;
    rewindCursor(numeroDeDiscos + 3, 0);
    printf("Paso %d: Mover disco %d de %c a %c \n\n", ++steps, n, a, b);
    printHeader();
    printtorre();
}

void hanoi(unsigned int n, char a, char b, char c)
{
    if (n == 1)
        move(n, a, b);
    else
    {
        hanoi(n - 1, a, c, b);
        move(n, a, b);
        hanoi(n - 1, c, b, a);
    }
}

int main(const int argc, const char *const argv[])
{
    if (argc >= 2)
    {
        numeroDeDiscos = atoi(argv[1]);
    }
    if (numeroDeDiscos < 4) {
        numeroDeDiscos = 5;
    }
    torre = malloc(numeroDeDiscos);
    memset(torre, torre_A, numeroDeDiscos);
    printf("Posicion inicial:\n\n");
    printHeader();
    printtorre();
    hanoi(numeroDeDiscos, torre_A, torre_B, torre_C);
    free(torre);
    return EXIT_SUCCESS;
}
