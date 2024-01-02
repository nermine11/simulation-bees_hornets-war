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
Case plateau[LIGNES][COLONNES];// les valeurs du 2dim tableau grid sont des cases
Unite* abeille;
Unite* frelon;
int tour; // Numero du tour
int pollen;
int ressourcesAbeille;
} Grille;



// ajoute à la fin d'une liste doublement chainee
int affilie_insecte(UListe* ruche, Unite* nv_unite){

    nv_unite->uprec = *ruche;
    if(*ruche)
        (*ruche)-> usuiv = nv_unite;
        return 1;
    return 0;
}   

//ajout unite à une case(ajout au debut si case vide sinon à la fin)
int ajout_unite_case(Grille** grid, Unite* nv_unite, int x, int y){

    Case* temp = &((*grid)->plateau[x][y]);//temp pointe vers la case 
    // case vide
    if(!temp->occupant){
        temp->occupant = nv_unite; 
        nv_unite->vprec = temp;
        nv_unite->vsuiv = NULL;
        return 1;
    }
    
    //case non vide
    while(temp->occupant->vsuiv){
        temp = temp->occupant->vsuiv;
    }
    // case contient une seule unite
    if(!temp->occupant->vsuiv){
        temp->occupant->vsuiv = nv_unite;

    }
    // case contient plusieurs unités
    else
    {
        temp->vsuiv = nv_unite;

    }
    nv_unite->vprec = temp;
    nv_unite->vsuiv = NULL;
    
    return 1;    
}




//initilaize unit et ajout_unit_case
Unite* initializeUnite(Grille** grid, char camp, char type, int force, int posx, int posy, int destx, int desty, char production, 
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
        ajout_unite_case(*grid, new_unit, posx, posy);
    } 

    return new_unit;
}


// j'ai modifié
void initializeGrille(Grille* grid) {

    grille->tour = 0;
    grille->pollen = 10;
    grille->ressourcesAbeille = 10;

    for (int i = 0; i < LIGNES; ++i) {
        for (int j = 0; j < COLONNES; ++j) {
            grille->plateau[i][j].x = i;  
            grille->plateau[i][j].y = j;  
            grille->plateau[i][j].colonie = NULL;  
            grille->plateau[i][j].occupant = NULL;  
        }
    }
    int affilie_insecte(UListe* ruche, Unite* nv_unite)

    Unite* ruche1 = initializeUnite(ABEILLE, RUCHE, 0, 0, -1, -1, "X", -1, -1)
    grille->abeille = ruche1; // link grille to abeille
    Unite* reine1 = initializeUnite(ABEILLE, REINE, FREINE, 3, 3, -1, -1, "X", -1, -1 )//pos(3,3)
    Unite* reine1_2 = initializeUnite(ABEILLE, REINE, FREINE, 4, 5, -1, -1, "X", -1, -1 )//pos(4,5)
    Unite* ouvriere1 = initializeUnite(ABEILLE, OUVRIERE, FOUVRIERE, 8, 3, -1, -1, "X", -1, -1)//(8,3)
    Unite* guerriere1 = initializeUnite(ABEILLE, GUERRIERE, FGUERRIERE, 5, 4,-1, -1, "X", -1, -1)//(5,4)
    Unite* escadron1 = initializeUnite(ABEILLE, ESCADRON, FESCADRON, 10, 7,-1, -1, "X", -1, -1)//(10,7)
    affilie_insecte(&ruche1,reine1);
    affilie_insecte(&ruche1,reine1_2);
    affilie_insecte(&ruche1,ouvriere1);
    affilie_insecte(&ruche1,guerriere1);
    affilie_insecte(&ruche1,escadron1);

    // frelons
    Unite* nid1 = initializeUnite(FRELON, NID, 11, 17, -1, -1, "X", -1, -1)
    grille->frelon = nid1; // link grille to frelon
    Unite* reinef1 = initializeUnite(FRELON, REINEF, FREINE, 13, 9, -1, -1, "X", -1, -1 )//pos(13,9)
    Unite* frelon1_1 = initializeUnite(FRELON, FREL, FFRELON, 8, 9, -1, -1, "X", -1, -1)//(8,9)
    Unite* frelon1_2 = initializeUnite(FRELON, FREL, FFRELON, 9, 9, -1, -1, "X", -1, -1)//(9,9)
    Unite* frelon1_3 = initializeUnite(FRELON, FREL, FFRELON, 10, 9, -1, -1, "X", -1, -1)//(10,9)
    affilie_insecte(&nid1,reinef1);
    affilie_insecte(&nid1,frelon1_1);
    affilie_insecte(&nid1,frelon1_2);
    affilie_insecte(&nid1,frelon1_3);

    // reste pour initializer les frelons
}


void initializeGrid(Unite* grid[LIGNES][COLONNES]) {
    for (int i = 0; i < LIGNES; ++i) {
        for (int j = 0; j < COLONNES; ++j) {
            grid[i][j] = NULL;
        }
    }

    // Initialiser et ajouter des unités à chaque liste chaînée
    addUnitToList(&grid[0][0], initializeUnite(ABEILLES, RUCHE, 0, 0));
    addUnitToList(&grid[0][0], initializeUnite(ABEILLES, OUVRIERE, 0, 1));
    addUnitToList(&grid[0][0], initializeUnite(ABEILLES, GUERRIERE, 0, 2));
    addUnitToList(&grid[0][0], initializeUnite(ABEILLES, ESCADRON, 0, 3)); //en haut à gauche

    addUnitToList(&grid[LIGNES - 1][COLONNES - 1], initializeUnite(FRELONS, REINE, LIGNES - 1, COLONNES - 1));
    addUnitToList(&grid[LIGNES - 1][COLONNES - 1], initializeUnite(FRELONS, FREL, LIGNES - 1, COLONNES - 2));
    addUnitToList(&grid[LIGNES - 1][COLONNES - 1], initializeUnite(FRELONS, FREL, LIGNES - 1, COLONNES - 3)); //en bas à droite
}


//to modify zenom
void printGrid(Unite* grid[LIGNES][COLONNES]) {
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
            
            // Imprimer la liste chaînée d'unités à cette position
            Unite* current = grid[i][j];
            while (current != NULL) {
                switch (current->type) {
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
                current = current->next;
            }
        }
        printf("|\n");
    }

    for (int j = 0; j < COLONNES; ++j) {
        printf(" - -");
    }
    printf("\n");
}




// retrancher des insectes, ruche ou nid de leurs case quand ils sont déplacés ou détruits
//zenom
int extrait_case(Grille** grid, Unite* unite){
    return;
}

//zenom
int extrait_insecte_affilie(Grille** grid, Unite* unite){
    return;
}
// remove insecte from case et ruche
void detruire_insecte(Grille* grid, UListe* insecte) {
    // Vérifier si c'est le premier de la liste
    if ((*insecte)->uprec != NULL) {
        if ((*insecte)->uprec->usuiv == (*insecte)) {
            (*insecte)->uprec->usuiv = (*insecte)->usuiv;
        }
    } else {
        // C'est le premier de la liste, mettre à jour la tête de liste
        grid->plateau[(*insecte)->posx][(*insecte)->posy].occupant = (*insecte)->usuiv;
    }
    // Vérifier si c'est le dernier de la liste
    if ((*insecte)->usuiv != NULL) {
        (*insecte)->usuiv->uprec = (*insecte)->uprec;
    }
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
//zenom
int ajout_ruche(Grille** grid, Unite* ruche){
    return;
}

int ajout_nid(Grille** grid, Unite* nid){
    return;
}


//zenom
int create_ruche(Grille**grid, Unite* ruche, Unite* reine){
    return;
}


//narmin
int extrait_ruche(Grille** grid, UListe* ruche){
    return;
}

// narmin
int extrait_nid(){
    return;
}



//narmin
int Conversion_ruche_nid(Grille** grid, UListe* ruche, UListe* insecte_gagnate){
    
    extrait_ruche(ruche);
    (*ruche)-> camp = FRELON;
    (*ruche)-> type = NID;
    (*ruche)-> production = "X";
    (*ruche)-> temp = (*ruche)-> toursrestant = -1:
    Unite* temp = *ruche;
    if((*ruche)->usuiv){
        for(; temp->usuiv; temp = temp->usuiv){
            detruire_insecte(&grid, &temp);
        }
    }
    ajout_nid(grid, *ruche);
    extrait_insecte_affilie(grid, *insecte_gagnate);
    affilie_insecte(ruche, *insecte_gagnate);
    return 1;


}




void addUnitToList(Unite** list, TypeUnite type, int posx, int posy) {
    Unite* newUnit = (Unite*)malloc(sizeof(Unite));
    if (newUnit != NULL) {
        newUnit->type = type;
        newUnit->posx = posx;
        newUnit->posy = posy;
        newUnit->next = NULL;

        if (*list == NULL) {
            *list = newUnit;
        } else {
            Unite* current = *list;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = newUnit;
        }
    }
}



// vérifier si la case est vide pour déplacer l'abeille
// 1 si vide 0 sinon
int case_vide(Grille *grid, int posX, int posY) {
    // on regarde si la case fait partie du plateau
    if (posX >= 0 && posX < LIGNES && posY >= 0 && posY < COLONNES) {
        //on verifie si la case est vide
        return !grid->plateau[posX][posY].colonie && !grid->plateau[posX][posY].occupant;
    }
    return -1; // Hors plateau
}


// 1 si tout est bon
// 0 si la case n'est pas vide
// delete insecte from case and link it to new case
int deplace_insecte(UListe* insecte, Grille** grid, char* input){

    if (!strcmp(input, "N") & case_vide(*grid, (*insecte)->posx - 1, (*insecte)->posy)) {
        (*insecte)->posx -= 1;
        remove_from_case(&grid, insecte);
        ajout_unite_case(&grid, insecte, (*insecte)->posx , (*insecte)->posy);
        return 1;
    }
    if (!strcmp(input, "s") && case_vide(*grid, (*insecte)->posx + 1, (*insecte)->posy)) {
        (*insecte)->posx += 1;
        remove_from_case(&grid, insecte);
        ajout_unite_case(&grid, insecte, (*insecte)->posx , (*insecte)->posy);
        return 1;
    }
    if (!strcmp(input, "O") && case_vide(*grid, (*insecte)->posx, (*insecte)->posy - 1)) {
        (*insecte)->posy -= 1;
        remove_from_case(&grid, insecte);
        ajout_unite_case(&grid, insecte, (*insecte)->posx , (*insecte)->posy);
        return 1;
    }
    if (!strcmp(input, "E") && case_vide(*grid, (*insecte)->posx , (*insecte)->posy + 1)) {
        (*insecte)->posy += 1;
        remove_from_case(&grid, insecte);
        ajout_unite_case(&grid, insecte, (*insecte)->posx , (*insecte)->posy);
        return 1;
    }
    if (!strcmp(input, "NO") && case_vide(*grid, (*insecte)->posx - 1, (*insecte)->posy - 1)) {
        (*insecte)->posx -= 1;
        (*insecte)->posy -= 1;
        remove_from_case(&grid, insecte);
        ajout_unite_case(&grid, insecte, (*insecte)->posx , (*insecte)->posy);
        return 1;
    }    
    if (!strcmp(input, "NE") && case_vide(*grid, (*insecte)->posx - 1, (*insecte)->posy + 1)) {
        (*insecte)->posx -= 1;
        (*insecte)->posy += 1;
        remove_from_case(&grid, insecte);
        ajout_unite_case(&grid, insecte, (*insecte)->posx , (*insecte)->posy);
        return 1;
    }    
    if (!strcmp(input, "SO") && case_vide(*grid, (*insecte)->posx + 1, (*insecte)->posy - 1)) {
        (*insecte)->posx += 1;
        (*insecte)->posy -= 1;
        remove_from_case(&grid, insecte);
        ajout_unite_case(&grid, insecte, (*insecte)->posx , (*insecte)->posy);
        return 1;
    }    
    if (!strcmp(input, "SE") && case_vide(*grid, (*insecte)->posx + 1, (*insecte)->posy + 1)) {
        (*insecte)->posx += 1;
        (*insecte)->posy += 1;
        remove_from_case(&grid, insecte);
        ajout_unite_case(&grid, insecte, (*insecte)->posx , (*insecte)->posy);
        return 1;
    }    
    printf("Case non vide");
    (*insecte)->destx = (*insecte)->desty = -1; // same in main if user choose not to move it 
    return 0;
}

// 0 si pollen pas suffisant ou ruche en train de produire une abeille
// 1 si tout a bien passé
int production_ruche(UListe* ruche, Grille* grid,  char* choix_prod){
    //*ruche points to unite
    if(!strcmp((*ruche)->production,"-1")){ // la ruche n'est pas en train de produire une autre abeille
        if(!(strcmp(choix_prod, "REINEA")) && grid->pollen >= CREINEA){
            (*ruche)-> production = REINEA;
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

int production_nid(UListe* nid, Grille* grid,  char* choix_prod){
    if(!strcmp((*nid)->production,"-1")){ 
        if(!(strcmp(choix_prod, "REINEF")) && grid->pollen >= CREINEF){
            (*ruche)-> production = REINEF;
            (*ruche)-> temps = TREINEF;
            (*ruche)-> toursrestant = TREINEF;
            return 1;
        }
        if(!(strcmp(choix_prod, "FRELON")) && grid->pollen >= CFRELON){
            (*ruche)-> production = FREL;
            (*ruche)-> temps = TFRELON;
            (*ruche)-> toursrestant = TFRELON;
            return 1;
        }

        printf("Pas de ressources suffisants, la quantité des ressources actuelles est: %d\n":grid->ressourcesAbeille);
    }
    else
    {
        printf("Le nid est en train de produire un autre frelon, il reste %d tours pour finir la production",(*nid)-> toursrestant);

    }
    return 0;   
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



// check if (*ruche)-> toursrestant == 0 au début du boucle while du tour dans le main
// initialize abeille, link abeille to ruche, link abeille to case
int ajout_abeille_produite(UListe* ruche, Grille* grid, char* choix_prod){

    Case* case = case_voisine_libre(*ruche, grid);
    if (case){
        int x = case->x;
        int y =  case->y;
        // initialize abeille et la liér à son case
        if(!(strcmp(choix_prod, "REINEA")))
            Unite* insecte = initializeUnite(&grid, ABEILLE, choix_prod, FREINE, x,y , -1,-1, "X", -1, -1);
        if(!(strcmp(choix_prod, "OUVRIERE")))
            Unite* insecte = initializeUnite(&grid, ABEILLE, choix_prod, FOUVRIERE, x,y , -1,-1, "X", -1, -1);
        if(!(strcmp(choix_prod, "GUERRIERE")))
            Unite* insecte = initializeUnite(&grid, ABEILLE, choix_prod, FGUERRIERE, x,y , -1,-1, "X", -1, -1);
        if(!(strcmp(choix_prod, "ESCADRON")))
            Unite* insecte = initializeUnite(&grid, ABEILLE, choix_prod, FESCADRON, x,y , -1,-1, "X", -1, -1);
        // affilier l'abeille à sa ruche
        affilie_insecte(ruche, insecte);
        return 1;
    }
    return 0;
}

int ajout_frelon_produit(UListe* nid, Grille* grid, char* choix_prod){
    Case* case = case_voisine_libre(*nid, grid);
    if (case){
        int x = case->x;
        int y =  case->y;
        // initialize frelon et le liér à son case
        if(!(strcmp(choix_prod, "REINEF")))
            Unite* insecte = initializeUnite(&grid, FRELON, choix_prod, FREINE, x,y , -1,-1, "X", -1, -1);
        if(!(strcmp(choix_prod, "FREL")))
            Unite* insecte = initializeUnite(&grid, FRELON, choix_prod, FFRELON, x,y , -1,-1, "X", -1, -1);
        // affilier le frelon à son nid
        affilie_insecte(nid, insecte);
        return 1;
    }
    return 0;
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
    Grille* grid = (Grille*)(sizeof(Grille));
    initializeGrille(grid);




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
