#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <MLV/MLV_all.h>

#define COLONNES 12
#define LIGNES 18

#define ABEILLE 'A'
#define FRELON 'F'
#define REINE 'r'
#define OUVRIERE 'o'
#define ESCADRON 'e'
#define GUERRIERE 'g'
#define FREL 'f'
#define RUCHE 'R'
#define NID 'N'
#define RECOLTE 'p'

#define TREINEA 8
#define TREINEF 8
#define TOUVRIERE 2
#define TGUERRIERE 4
#define TESCADRON 6
#define TFRELON 5
#define TRECOLTE 4

#define CREINEA 7
#define CREINEF 8
#define COUVRIERE 3
#define CGUERRIERE 5
#define CESCADRON 6
#define CFRELON 3
#define CRUCHE 10
#define CNID 10

#define FREINE 6
#define FOUVRIERE 1
#define FGUERRIERE 5
#define FESCADRON 12
#define FFRELON 8

typedef struct {
    char camp;
    char type;
    int force;
    int posx, posy;
    int destx, desty;
    char production;
    int temps;
    int toursrestant;
    struct unite* usuiv, * uprec;
    struct unite* colsuiv, * colprec;
    struct unite* vsuiv, * vprec;
} Unite, * UListe;

typedef struct {
    Unite* colonie;
    UListe occupant;
} Case;

typedef struct {
    Case plateau[LIGNES][COLONNES];
    UListe abeille, frelon;
    int tour;
    int ressourcesAbeille, ressourcesFrelon;
} Grille;

void initializeGrid(Unite grid[LIGNES][COLONNES]) {
    for (int i = 0; i < LIGNES; ++i) {
        for (int j = 0; j < COLONNES; ++j) {
            grid[i][j].type = -1;
        }
    }

    grid[0][0] = (Unite){RUCHE, REINE, FREINE, 0, 0};
    grid[0][1] = (Unite){RUCHE, OUVRIERE, FOUVRIERE, 0, 1};
    grid[0][2] = (Unite){RUCHE, GUERRIERE, FGUERRIERE, 0, 2};
    grid[0][3] = (Unite){RUCHE, ESCADRON, FESCADRON, 0, 3};

    grid[LIGNES - 1][COLONNES - 1] = (Unite){NID, REINE, FREINE, LIGNES - 1, COLONNES - 1};
    grid[LIGNES - 1][COLONNES - 2] = (Unite){NID, FREL, FFRELON, LIGNES - 1, COLONNES - 2};
    grid[LIGNES - 1][COLONNES - 3] = (Unite){NID, FREL, FFRELON, LIGNES - 1, COLONNES - 3};
}

void printGrid(Unite grid[LIGNES][COLONNES]) {
    for (int i = 0; i < LIGNES; ++i) {
        for (int j = 0; j < COLONNES; ++j) {
            printf(" - -");
        }
        for (int j = 0; j < COLONNES; ++j) {
            printf(" ");
        }
        printf("\n");

        for (int j = 0; j < COLONNES; ++j) {
            printf("|");
            switch (grid[i][j].type) {
                case RUCHE:
                    printf(" R ");
                    break;
                case REINE:
                    printf(" A ");
                    break;
                case GUERRIERE:
                    printf(" G ");
                    break;
                case OUVRIERE:
                    printf(" O ");
                    break;
                case NID:
                    printf(" N ");
                    break;
                case FREL:
                    printf(" f ");
                    break;
                default:
                    printf("   ");
            }
        }
        printf("|\n");
    }

    for (int j = 0; j < COLONNES; ++j) {
        printf(" - -");
    }
    printf("\n");
}

void processCampTurn(Unite grid[LIGNES][COLONNES], char camp) {
    // A FAIRE
}

void resolveCombat(Unite* unit1, Unite* unit2) {
    //  A FAIRE
}

void updateResources(int* pollen, int* defeatedAbeille) {
    //  A FAIRE
}

int voisins(Unite unit1, Unite unit2) {
    int dx = abs(unit1.posx - unit2.posx);
    int dy = abs(unit1.posy - unit2.posy);
    return (dx <= 1 && dy <= 1 && (dx + dy > 0));
}

int PasdeTroupes(Unite grid[LIGNES][COLONNES], char type) {
    return 1;
}

int main() {
    srand(time(NULL));
    int fin = 0;
    int pollen, defeatedAbeille = 10;
    Unite grid[LIGNES][COLONNES];
    initializeGrid(grid);

    while (!fin) {
        char firstCamp = (rand() % 2 == 0) ? ABEILLE : FRELON;
        char secondCamp = (firstCamp == ABEILLE) ? FRELON : ABEILLE;

        processCampTurn(grid, firstCamp);
        processCampTurn(grid, secondCamp);

        for (int i = 0; i < LIGNES; ++i) {
            for (int j = 0; j < COLONNES; ++j) {
                Unite x = grid[i][j];

                for (int k = 0; k < LIGNES; ++k) {
                    for (int l = 0; l < COLONNES; ++l) {
                        Unite y = grid[k][l];

                        if (voisins(x, y) && (x.type != y.type)) {
                            resolveCombat(&x, &y);
                        }
                    }
                }
            }
        }
        updateResources(&pollen, &defeatedAbeille);
        printGrid(grid);
        if (PasdeTroupes(grid, firstCamp) || PasdeTroupes(grid, secondCamp))
            fin = 1;
    }

    return 0;
}
