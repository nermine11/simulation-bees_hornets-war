//#<include <MLV/MLV_all.h>
#include <stdio.h>
#include<stdlib.h>
#include<time.h>


// Dimensions de la grille en nombre de cases (origine en haut a gauche) :
#define COLONNES 12
#define LIGNES 18
// Les deux camps :
#define ABEILLE 'A'
#define FRELONS 'F'
// Les types d'unites :
#define REINEA 'r'
#define REINEF 'rf'
#define OUVRIERE 'o'
#define ESCADRON 'e'
#define GUERRIERE 'g'
#define FREL 'f'
#define RUCHE 'R'
#define NID 'N'
// Pour la recolte de pollen
#define RECOLTE 'p'
// Les temps necessaires a la production :
#define TREINEA 8
#define TREINEF 8
#define TOUVRIERE 2
#define TGUERRIERE 4
#define TESCADRON 6
#define TFRELON 5
#define TRECOLTE 4

// Les couts necessaires a la production :
#define CREINEA 7
#define CREINEF 8
#define COUVRIERE 3
#define CGUERRIERE 5
#define CESCADRON 6
#define CFRELON 3
#define CRUCHE 10
#define CNID 10
// La force des unites
#define FREINE 6
#define FOUVRIERE 1
#define FGUERRIERE 5
#define FESCADRON 12
#define FFRELON 8

// La structure Unite :
typedef struct unite {
char camp; // ABEILLE ou FRELON
char type; // RUCHE, NID, REINE, OUVRIERE, GUERRIERE, ESCADRON ou FREL
int force; // la force de l'unite
int posx, posy; // position actuelle sur la grille
int destx, desty; // destination (negatif si immobile)
char production; // production d'une ruche ou d'un nid et RECOLTE pour la recolte de pollen
int temps; // nombres de tours total pour cette production
int toursrestant; // tours restant pour cette production
struct unite *usuiv, *uprec; // liste des unites affiliees a une ruche ou un nid
struct unite *colsuiv, *colprec; // liste des autres ruches ou nids (colonies) du même camp
struct unite *vsuiv, *vprec; // liste des autres unites sur la meme case
} Unite, *UListe;

// La structure Case :
typedef struct {
Unite* colonie; // S'il y a une ruche ou un nid sur la case
Unite* occupant; // les autres occupants de la case
} Case;

// La structure Grille :
typedef struct {
//Case plateau[X][Y];
Unite* abeille, frelon;
int tour; // Numero du tour
int ressourcesAbeille, ressourcesFrelon;
} Grille;

void initializeGrid(Unite grid[LIGNES][COLONNES]) {
    for (int i = 0; i < LIGNES; ++i) {
        for (int j = 0; j < COLONNES; ++j) {
            grid[i][j].type = -1;
        }
    }

    grid[0][0] = (Unite){ABEILLE, RUCHE, .posx = 0, .posy = 0};
               //(Unite){.camp = ABEILLE, .type = RUCHE, .posx = 0, .posy = 0}
    grid[0][1] = (Unite){ABEILLE, OUVRIERE, FOUVRIERE, 0, 1};
    grid[0][2] = (Unite){ABEILLE, GUERRIERE, FGUERRIERE, 0, 2};
    grid[0][3] = (Unite){ABEILLE, ESCADRON, FESCADRON, 0, 3};

    grid[LIGNES - 1][COLONNES - 1] = (Unite){FRELONS, REINEF, FREINE, LIGNES - 1, COLONNES - 1};//grid[17][11]
    grid[LIGNES - 1][COLONNES - 2] = (Unite){FFRELON, FREL, FFRELON, LIGNES - 1, COLONNES - 2}; //gird[17][10]
    grid[LIGNES - 1][COLONNES - 3] = (Unite){FRELONS, FREL, FFRELON, LIGNES - 1, COLONNES - 3}; //grid[17][9]
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
                case REINEF:
                    printf(" rF ");
                    break;
                case REINEA:
                    printf(" rA ");
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
                case RECOLTE:
                    printf(" p ");
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


/* in main :
 char input 
printf("Ou se déplace l'abeille?")
scanf("%c", &input);
// we can gere case of error with while but i dont know if necessary
*/
// Etape "Colonie d'abeilles"
void Move(Unite* unite, char input) {
    // il faudra déclarer input avant d'appeler la fonction 
    switch(input) {
        case 'N':
            unite->posx = unite->posx - 1;
            break;
        case 'S':
            unite->posx = unite->posx + 1;
            break;
        case 'O':
            unite->posy = unite->posy - 1;
            break;
        case 'E':
            unite->posy = unite->posy + 1;
            break;
        case 'NO':
            unite->posx = unite->posx - 1;
            unite->posy = unite->posy - 1;
            break;        
        case 'NE':
            unite->posx = unite->posx - 1;
            unite->posy = unite->posy + 1;
            break;
        case 'SO':
            unite->posx = unite->posx + 1;
            unite->posy = unite->posy - 1;
            break;
        case 'SE':
            unite->posx = unite->posx + 1;
            unite->posy = unite->posy + 1;
            break;
        default:
            break;
    }
}



void production_ruche()
// Narmin
    ;

void recolter(Grille* grid, Unite* unit) {
    if (unit->temps > 0) {
        unit->temps--; 
    } else {
        grid->ressourcesAbeille++;
        unit->type = -1; // pas de ressource pour les frelons
        unit->production = RECOLTE;
        unit->temps = TRECOLTE; 
    }
}

// apres finir ces fonctions , on voit ce qu'il manquera pour etape abeille, lezz goo



void tour(Unite grid[LIGNES][COLONNES], char camp) {
    // A FAIRE

    //recolter(&grid, &unit);
    ;
}

void resolveCombat(Unite* unit1, Unite* unit2) {
    //  A FAIRE
    ;
}

void updateResources(int* pollen, int* defeatedAbeille) {
    //  A FAIRE
    ;
}



// return 1 if voisin else 0
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
    Unite grid[LIGNES][COLONNES];
    initializeGrid(grid);

    while (!fin) {
        char firstCamp = (rand() % 2 == 0) ? ABEILLE : FRELONS; // if random number is 0 abeille joue first else frelon
        char secondCamp = (firstCamp == ABEILLE) ? FRELONS : ABEILLE;
        int pollen, defeatedAbeille = 10;
        tour(grid, firstCamp);
        tour(grid, secondCamp);

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
