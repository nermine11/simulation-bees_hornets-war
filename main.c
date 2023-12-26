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
#define REINEA 'rA'
#define REINEF 'rF'
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
Case plateau[X][Y];
Unite* abeille;
Unite* frelon;
int tour; // Numero du tour
int pollen;
int ressourcesAbeille;
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

    grid[LIGNES - 1][COLONNES - 1] = (Unite){FRELONS, REINE, FREINE, LIGNES - 1, COLONNES - 1};//grid[17][11]
    grid[LIGNES - 1][COLONNES - 2] = (Unite){FRELONS, FREL, FFRELON, LIGNES - 1, COLONNES - 2}; //gird[17][10]
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
void Move_Abeille(UListe* abeille, char* input){
    // il faudra déclarer input avant d'appeler la fonction 
    // i will change this cuz i have to check if case empty first
    if()
    switch(input){
        case 'N':
            (*UListe)->posx = x-1;
            break;
        case 'S':
            (*UListe)->posx = x+1;
            break;
        case 'O':
            (*UListe)->posy = y-1;
            break;
        case 'E':
            (*UListe)->posy = y+1;
            break;
        case "NO": 
            (*UListe)->posx = x-1;
            (*UListe)->posy = y-1;
            break;        
        case "NE":
            (*UListe)->posx = x-1;
            (*UListe)->posy = y+1;
            break;
        case "SO":
            (*UListe)->posx = x+1;
            (*UListe)->posy = y-1;
            break;
        case "SE":
            (*UListe)->posx = x+1;
            (*UListe)->posy = y+1;
            break;
        default:
            break;

    }
}

//choix= {REINE, OUVRIERE, GUERRIERE, ESCADRON}
// 0 si pollen pas suffisant
// 1 si tout a bien passé
int production_ruche(UListe* ruche, Grille* grid,  char* choix_prod){

    if(choix_prod == REINE && grid->pollen >= CREINEA){
        (*UListe)-> production = REINE;
        (*UListe)-> temps = TREINEA;
        (*UListe)-> toursrestant = TREINEA;// still in doubt abt this
        return 1;
        }
    if(choix_prod == OUVRIERE && grid->pollen >= COUVRIERE){
        (*UListe)-> production = OUVRIERE;
        (*UListe)-> temps = TOUVRIERE;
        (*UListe)-> toursrestant = TOUVRIERE;// 
        return 1;
        }
    if(choix_prod == GUERRIERE && grid->pollen >= CGUERRIERE){
        (*UListe)-> production = GUERRIERE;
        (*UListe)-> temps = TGUERRIERE;
        (*UListe)-> toursrestant = TGUERRIERE;// 
        return 1;
        }
    if(choix_prod == ESCADRON && grid->pollen >= CESCADRON){
        (*UListe)-> production = ESCADRON;
        (*UListe)-> temps = TESCADRON;
        (*UListe)-> toursrestant = TESCADRON;// 
        return 1;
        }
    
     print("Pas de pollen suffisant, la quantité du pollen actuelle est: %d\n":grid->pollen);
     return 0;   

    }


void recolter(Grille* grid, UListe* ouvriere) {
    // on doit pas aussi tester si la grille est vide pour qu'elle puisse recolter
    if ((*ouvriere)->temps > 0) {
        (*ouvriere)->temps--; 
    } else 
    {
        grid->ressourcesAbeille++; // je n'ai pas compris
        (*ouvriere)->type = -1; // pas de ressource pour les frelons // je n'ai pas compris
        (*ouvriere)->destx = (*ouvriere)->desty = -1; // Ouvriere immobile
        (*ouvriere)->production = RECOLTE;
        (*ouvriere)->temps = TRECOLTE; 
    }
}

void detruire_insecte()
    // supprimer son unité
    // zenomium 


void tour(Unite grid[LIGNES][COLONNES], char camp) {
    // A FAIRE

//recolter(&grid, &unit);
    ;
 }

void resolveCombat(Unite* unit1, Unite* unit2) {
    //  A FAIRE;
}

void updateResources(int* pollen, int* defeatedAbeille) {
    //  A FAIRE;
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
