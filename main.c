//#<include <MLV/MLV_all.h>
#include <stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

// Dimensions de la grille en nombre de cases (origine en haut a gauche) :
#define COLONNES 12
#define LIGNES 18
// Les deux camps :
#define ABEILLE 'A'
#define FRELON 'F'
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
char type; // RUCHE, NID, REINEA, REINEF, OUVRIERE, GUERRIERE, ESCADRON ou FREL
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
int x; //posx
int y; // posy
Unite* colonie; // S'il y a une ruche ou un nid sur la case
Unite* occupant; // les autres occupants de la case
} Case;

// La structure Grille :
typedef struct {
Case plateau[LIGNES][COLONNES];// les valeurs du 2dim tableau plateaux sont des cases
Unite* abeille;
Unite* frelon;
int tour; // Numero du tour
int pollen;
int ressourcesAbeille;
} Grille;


int ajout_insecte_ruche(UListe* ruche, Unite* nv_unite){
    Unite* temp  = *ruche;
    if(!*ruche)
        *ruche = nv_unite; 
    while(temp->suivant){
        temp = temp->suivant;
    }    
    temp->suivant = nv_unite;
    return 1;

}

// ajouter des insectes, ruche ou nid à leurs case
int ajout_case(Grille** grid, Unite* nv_unite, int x, int y){
    Case* temp  = (*grid)->plateau[x][y];
    if(!*temp)
        *temp = nv_unite; 
    while(temp->occupant){
        temp = temp->occupant;
    }    
    temp->occupant = nv_unite;
    if(!strcmp(unite->type, RUCHE) || !strcmp(unite->type, NID)) {
        Case* temp1  = (*grid)->plateau[x][y];
        while(temp1->colonie){
            temp1 = temp1->colonie;
        }    
        temp1->colonie = nv_unite;

    }  
    return 1;

}

// retrancher des insectes, ruche ou abeilles de leurs case quand ils sont déplacés ou détruits
int remove_from_case(Grille** grid, Unite* unite){

    int x = unite->posx;
    int y = unite->posy;

    if (!((*grid)->plateau[x][y]))
        return;
    
    

}





Unite* initializeUnite(Grille* grid, char camp, char type, int force, int posx, int posy, int destx, int desty, char production, 
int temps, int toursrestant ){
    Unite* new_unit = (Unite*) malloc(sizeof(Unite));
    if(new_unit){
        new_unit->type = type;
        new_unit->force = force;
        new_unit->posx = posx;
        new_unit->posy = posy;
        new_unit->destx = destx;
        new_unit->desty = desty;
        new_unit->production = production;
        new_unit->temps= temps;
        new_unit->toursrestant = toursrestant;
        new_unit->usuiv = NULL;
        new_unit->uprec = NULL;
        new_unit->colsuiv = NULL;
        new_unit->colprec = NULL;
        new_unit->vsuiv= NULL;
        new_unit->vprec = NULL;
        ajout_insecte_case()
    } 

    return new_unit;
}




/* to change zinom
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
*/
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

// vérifier si la case est vide pour déplacer l'abeille
// 1 si vide 0 sinon
int case_vide(Grille *grid, int posX, int posY) {
    return !grid->plateau[posX][posY]->colonie && //(plateau.colonie ou plateau->colonie)
           !grid->plateau[posX][posY]->occupant;
}

// retourne une cases voisine libre de la ruche choisie
Case* case_voisine_libre(UListe ruche, Grille* grid){

    int x = ruche->posx;
    int y = ruche->posy;

    if case_vide(grid, x +1, y)
        return grid->plateau[x+1][y];
    if case_vide(grid, x +1, y+1)
        return grid->plateau[x+1][y+1];
    if case_vide(grid, x , y+1)
        return grid->plateau[x][y+1];
    if case_vide(grid, x -1, y)
        return grid->plateau[x-1][y];
    if case_vide(grid, x -1, y-1)
        return grid->plateau[x-1][y-1];
    if case_vide(grid, x , y-1)
        return grid->plateau[x][y-1];
    return NULL;

}




// 1 si tout est bon
// 0 si la case n'est pas vide
int Move_Abeille(UListe* abeille, Grille* grid, char* input){

    if (!strcmp(input, "N") & case_vide(grid, (*abeille)->posx - 1, (*abeille)->posy)) {
        (*abeille)->posx -= 1;
        ajout_insecte_case(&grid, abeille, (*abeille)->posx , (*abeille)->posy);
        return 1;
    }
    if (!strcmp(input, "s") && case_vide(grid, (*abeille)->posx + 1, (*abeille)->posy)) {
        (*abeille)->posx += 1;
        ajout_insecte_case(&grid, abeille, (*abeille)->posx , (*abeille)->posy);
        return 1;
    }
    if (!strcmp(input, "O") && case_vide(grid, (*abeille)->posx, (*abeille)->posy - 1)) {
        (*abeille)->posy -= 1;
        ajout_insecte_case(&grid, abeille, (*abeille)->posx , (*abeille)->posy);
        return 1;
    }
    if (!strcmp(input, "E") && case_vide(grid, (*abeille)->posx , (*abeille)->posy + 1)) {
        (*abeille)->posy += 1;
        ajout_insecte_case(&grid, abeille, (*abeille)->posx , (*abeille)->posy);
        return 1;
    }
    if (!strcmp(input, "NO") && case_vide(grid, (*abeille)->posx - 1, (*abeille)->posy - 1)) {
        (*abeille)->posx -= 1;
        (*abeille)->posy -= 1;
        ajout_insecte_case(&grid, abeille, (*abeille)->posx , (*abeille)->posy);

        return 1;
    }    
    if (!strcmp(input, "NE") && case_vide(grid, (*abeille)->posx - 1, (*abeille)->posy + 1)) {
        (*abeille)->posx -= 1;
        (*abeille)->posy += 1;
        ajout_insecte_case(&grid, abeille, (*abeille)->posx , (*abeille)->posy);
        return 1;
    }    
    if (!strcmp(input, "SO") && case_vide(grid, (*abeille)->posx + 1, (*abeille)->posy - 1)) {
        (*abeille)->posx += 1;
        (*abeille)->posy -= 1;
        ajout_insecte_case(&grid, abeille, (*abeille)->posx , (*abeille)->posy);
        return 1;
    }    
    if (!strcmp(input, "SE") && case_vide(grid, (*abeille)->posx + 1, (*abeille)->posy + 1)) {
        (*abeille)->posx += 1;
        (*abeille)->posy += 1;
        ajout_insecte_case(&grid, abeille, (*abeille)->posx , (*abeille)->posy);
        return 1;
    }    
    printf("Case non vide");
    (*abeille)->destx = (*abeille)->desty = -1; // same in main if user choose not to move it 
    return 0;
}


// 0 si pollen pas suffisant ou ruche en train de produire une abeille
// 1 si tout a bien passé
int production_ruche(UListe* ruche, Grille* grid,  char* choix_prod){
    //*ruche points to unite
    if(!strcmp((*ruche)->production,"-1")){ // la ruche n'est pas en train de produire une autre abeille
        if(!(strcmp(choix_prod, "REINE")) && grid->pollen >= CREINEA){
            (*ruche)-> production = REINE;
            (*ruche)-> temps = TREINEA;
            (*ruche)-> toursrestant = TREINEA;
            return 1;
        }
        if(!(strcmp(choix_prod, "OUVRIERE")) && grid->pollen >= COUVRIERE){
            (*ruche)-> production = OUVRIERE;
            (*ruche)-> temps = TOUVRIERE;
            (*ruche)-> toursrestant = TOUVRIERE;
            return 1;
        }
        if(!(strcmp(choix_prod, "GUERRIERE")) && grid->pollen >= CGUERRIERE){
            (*ruche)-> production = GUERRIERE;
            (*ruche)-> temps = TGUERRIERE;
            (*ruche)-> toursrestant = TGUERRIERE; 
            return 1;
        }
        if(!(strcmp(choix_prod, "ESCADRON")) && grid->pollen >= CESCADRON){
            (*ruche)-> production = ESCADRON;
            (*ruche)-> temps = TESCADRON;
            (*ruche)-> toursrestant = TESCADRON;
            return 1;
        }
        
        printf("Pas de pollen suffisant, la quantité du pollen actuelle est: %d\n":grid->pollen);
    }
    else
    {
        printf("La ruche est en train de produire une autre abeille, il reste %d tours pour finir la production",(*ruche)-> toursrestant);

    }
    return 0;   
}

// retourne une cases voisine libre de la ruche choisie ( posx et posy)
int case_voisine_libre(UListe ruche, Grille* grid){

    int x = ruche->posx;
    int y = ruche->posy;
    case_vide()
}



// check if (*ruche)-> toursrestant == 0 au début du boucle while du tour dans le main
int ajout_abeille_produite(UListe* ruche, Grille* grid, char* choix_prod){
        
    // get posx and posy from case_voisine_libre and use ajout_insecte_ruche and ajout_insecte case


}


void recolter(Grille* grid, UListe* ouvriere) {
    if ((*ouvriere) != NULL && (*ouvriere)->type == OUVRIERE) {
        if ((*ouvriere)->toursrestant > 0) {
            (*ouvriere)->toursrestant--;
        } else {
            grid->pollen++;
            detruire_insecte(grid, ouvriere);
        }
    }
}

void detruire_insecte(Grille* grid, UListe* insecte) {
    // Supprimer l'unité de la liste d'affiliation à la ruche ou au nid
    if ((*insecte)->uprec != NULL) { //Si ce n'est pas le premier de la liste (pas ruche ou nid)
        if ((*insecte)->uprec->usuiv == (*insecte)) {
            (*insecte)->uprec->usuiv = (*insecte)->usuiv;

    // Si l'unité occupe une case, la libérer
    int posX = (*insecte)->posx;
    int posY = (*insecte)->posy;

    if (grid->plateau[posX][posY].occupant == (*insecte)) {
        grid->plateau[posX][posY].occupant = NULL;
    }

    // Libérer la mémoire de l'unité
    free(*insecte);
    *insecte = NULL;  // Afin d'éviter les pointeurs non valides
} 

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
int voisins(Unite* unit1, Unite* unit2) {
    int dx = abs(unit1->posx - unit2->posx);
    int dy = abs(unit1->posy - unit2->posy);
    return (dx <= 1 && dy <= 1 && (dx + dy > 0));
}

int PasdeTroupes(Unite grid[LIGNES][COLONNES], char type) {
    return 1;
}




int main() {
    srand(time(NULL));
    int fin = 0;
    Unite* ruche1 = initializeUnite(ABEILLE, RUCHE, 0, 0, -1, -1, "X", -1, -1)
    Unite* reine1 = initializeUnite(ABEILLE, REINE, FREINE, 3, 3, -1, -1, "X", -1, -1 )//pos(3,3)
    Unite* reine1_2 = initializeUnite(ABEILLE, REINE, FREINE, 4, 5, -1, -1, "X", -1, -1 )//pos(4,5)
    Unite* ouvriere1 = initializeUnite(ABEILLE, OUVRIERE, FOUVRIERE, 8, 3, -1, -1, "X", -1, -1)//(8,3)
    Unite* guerriere1 = initializeUnite(ABEILLE, GUERRIERE, FGUERRIERE, 5, 4,-1, -1, "X", -1, -1)//(5,4)
    Unite* escadron1 = initializeUnite(ABEILLE, ESCADRON, FESCADRON, 10, 7,-1, -1, "X", -1, -1)//(10,7)
    ruche1->usuiv = reine1; 
    reine1->uprec = ruche1; reine1->usuiv = reine1_2; 
    reine1_2->uprec = reine1; reine1_2->usuiv = ouvriere1;
    ouvriere1->uprec = reine1_2; ouvriere1->usuiv = guerriere1;
    guerriere1->uprec = ouvriere1; guerriere1->usuiv = escadron1;
    escadron1->uprec = guerriere1;



    Unite grid[LIGNES][COLONNES]; // should be case plateau[LIGNES][COLONNES] zenom
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
