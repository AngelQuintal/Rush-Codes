#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TOWER_A 'A'
#define TOWER_B 'B'
#define TOWER_C 'C'


#define DISC_CHARACTER '*'


unsigned int numberOfDiscs = 0;
char *tower;

void rewindCursor(const unsigned int lines, const unsigned int columns)
{
    printf("\x1b[%dA", lines);
    printf("\x1b[%dD", columns);
}

void printCentered(char *str, unsigned int size)
{
    const unsigned int spaces = (size - strlen(str));
    const unsigned int spacesBefore = spaces / 2;
    const unsigned int spacesAfter = spacesBefore + spaces % 2;
    for (unsigned int i = 0; i < spacesBefore; i++)
        putchar(' ');
    printf("%s", str);
    for (unsigned int i = 0; i < spacesAfter; i++)
        putchar(' ');
}

void printHeader()
{
    printCentered("Tower A", 2 * numberOfDiscs);
    printCentered("Tower B", 2 * numberOfDiscs);
    printCentered("Tower C", 2 * numberOfDiscs);
    putchar('\n');
}

void printTower()
{
    static const unsigned int offset = 1;
    const unsigned int baseDiscSize = 2 * numberOfDiscs - 1;
    const unsigned int numberOfColumns = 3 * (baseDiscSize + offset);
    unsigned int a = numberOfDiscs, b = numberOfDiscs, c = numberOfDiscs;
    char table[numberOfDiscs][numberOfColumns];
    memset(table, ' ', sizeof(table));

    for (unsigned int i = numberOfDiscs - 1; i < numberOfDiscs; i--)
    {
        unsigned int line;
        const unsigned int discSize = baseDiscSize - 2 * (numberOfDiscs - i - 1);
        const unsigned int column = (tower[i] - TOWER_A) * (baseDiscSize + offset) + numberOfDiscs - i + offset - 1;
        if (tower[i] == TOWER_A)
            line = --a;
        else if (tower[i] == TOWER_B)
            line = --b;
        else if (tower[i] == TOWER_C)
            line = --c;
        memset(&table[line][column], DISC_CHARACTER, discSize);
    }
    for (unsigned int line = 0; line < numberOfDiscs; line++)
    {
        printf("%.*s\n", numberOfColumns, table[line]);
    }
}

void move(unsigned int n, char a, char b)
{
    static unsigned int steps = 0;
    sleep(1);
    tower[n - 1] = b;
    rewindCursor(numberOfDiscs + 3, 0);
    printf("Step %d: Move disc %d from %c to %c \n\n", ++steps, n, a, b);
    printHeader();
    printTower();
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
        numberOfDiscs = atoi(argv[1]);
    } 
    if (numberOfDiscs < 4) {
        numberOfDiscs = 5;
    }
    tower = malloc(numberOfDiscs);
    memset(tower, TOWER_A, numberOfDiscs);
    printf("Initial position:\n\n");
    printHeader();
    printTower();
    hanoi(numberOfDiscs, TOWER_A, TOWER_B, TOWER_C);
    free(tower);
    return EXIT_SUCCESS;
}
