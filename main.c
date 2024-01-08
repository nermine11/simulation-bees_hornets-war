#include <MLV/MLV_all.h>
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
#define REINEA 'a'
#define REINEF 'r'
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
Case plateau[LIGNES][COLONNES];// les valeurs du 2dim tableau grille sont des cases
UListe abeille, frelon;
int tour; // Numero du tour
int pollen, ressourcesAbeille;
} Grille;














//initilaize unit et ajout_unit_case
Unite* initialiserUnite(char camp, char type, int force, int posx, int posy) {
    Unite* unite = (Unite*)malloc(sizeof(Unite));
    if (unite == NULL) {
        perror("Erreur d'allocation mémoire pour une unité");
        exit(EXIT_FAILURE);
    }

    // Initialiser les champs de l'unité
    unite->camp = camp;
    unite->type = type;
    unite->force = force;
    unite->posx = posx;
    unite->posy = posy;
    unite->destx = -1; // Valeur par défaut pour destination immobile
    unite->desty = -1;
    unite->production = '\0'; // Pas de production par défaut
    unite->temps = 0;
    unite->toursrestant = 0;
    unite->usuiv = NULL;
    unite->uprec = NULL;
    unite->colsuiv = NULL;
    unite->colprec = NULL;
    unite->vsuiv = NULL;
    unite->vprec = NULL;
    return unite;
}

void initialiserGrille(Grille* grille) {
    for (int i = 0; i < LIGNES; ++i) {
        for (int j = 0; j < COLONNES; ++j) {
            grille->plateau[i][j].x = i;  
            grille->plateau[i][j].y = j;  
            grille->plateau[i][j].colonie = NULL;  
            grille->plateau[i][j].occupant = NULL;  
        }
    }
    grille->tour = 1;
    grille->pollen = 10;
    grille->ressourcesAbeille = 10;
    grille->abeille = NULL;
    grille->frelon = NULL;
}

// ajoute insecte à case, colonie et ruche
int affilie_insecte(UListe* ruche, Unite* unite, Grille* grille) {
    Unite* current = *ruche;

    // Parcourir la liste pour atteindre le dernier élément
    while (current && current->usuiv) {
        current = current->usuiv;
    }

    // Ajouter la nouvelle unité à la fin de la liste
    if (current) {
        current->usuiv = unite;
        unite->uprec = current;
    } else {
        // Si la liste était vide, la nouvelle unité devient le premier élément
        *ruche = unite;
        unite->uprec = NULL;
    }

    // Affilier l'unité à la liste des colonies (ruches ou nids)
    unite->colsuiv = grille->plateau[unite->posx][unite->posy].colonie;
    unite->colprec = NULL;
    if (grille->plateau[unite->posx][unite->posy].colonie) {
        grille->plateau[unite->posx][unite->posy].colonie->colprec = unite;
    }
    grille->plateau[unite->posx][unite->posy].colonie = unite;

    // Affilier l'unité à la liste des unités sur la même case
    unite->vsuiv = grille->plateau[unite->posx][unite->posy].occupant;
    unite->vprec = NULL;
    if (grille->plateau[unite->posx][unite->posy].occupant) {
        grille->plateau[unite->posx][unite->posy].occupant->vprec = unite;
    }
    grille->plateau[unite->posx][unite->posy].occupant = unite;

    return 1;
}


// remove insecte from case, colonie et ruche
void detruireUnite(Unite* unite, Grille* grille) {
    // Désaffilier l'unité de la liste de la ruche ou du nid
    if (unite->uprec) {
        unite->uprec->usuiv = unite->usuiv;
    } else {
        // L'unité était le premier élément de la liste
        if (unite->camp == ABEILLE) {
            grille->abeille = unite->usuiv;
        } else {
            grille->frelon = unite->usuiv;
        }
    }

    // Désaffilier l'unité de la liste des colonies (ruches ou nids)
    if (unite->colprec) {
        unite->colprec->colsuiv = unite->colsuiv;
    } else {
        // L'unité était le premier élément de la liste des colonies
        grille->plateau[unite->posx][unite->posy].colonie = unite->colsuiv;
    }

    // Désaffilier l'unité de la liste des unités sur la même case
    if (unite->vprec) {
        unite->vprec->vsuiv = unite->vsuiv;
    } else {
        // L'unité était le premier élément de la liste des unités sur la même case
        grille->plateau[unite->posx][unite->posy].occupant = unite->vsuiv;
    }

    // Libérer la mémoire de l'unité
    free(unite);
}
 
// Fonction pour déplacer une unité
void deplacerUnite(Unite* unite, Grille* grille) {
    // Affichage des directions possibles
    printf("Directions possibles : N (nord), NE (nord-est), E (est), SE (sud-est), S (sud), SO (sud-ouest), O (ouest), NO (nord-ouest)\n");

    // Demander à l'utilisateur d'entrer la direction
    char direction[3];  // "NO" est la plus longue, donc besoin de 3 caractères
    printf("Entrez la direction : ");
    scanf("%s", direction);

    // Appliquer le déplacement en fonction de la direction
    if (strcmp(direction, "N") == 0) {
        // Déplacement vers le nord
        if (unite->posy > 0) {
            unite->posy--;
            printf("Déplacement vers le nord effectué.\n");
        } else {
            printf("Déplacement impossible vers le nord.\n");
        }
    } else if (strcmp(direction, "NE") == 0) {
        // Déplacement vers le nord-est
        if (unite->posy > 0 && unite->posx < COLONNES - 1) {
            unite->posy--;
            unite->posx++;
            printf("Déplacement vers le nord-est effectué.\n");
        } else {
            printf("Déplacement impossible vers le nord-est.\n");
        }
    } else if (strcmp(direction, "E") == 0) {
        // Déplacement vers l'est
        if (unite->posx < COLONNES - 1) {
            unite->posx++;
            printf("Déplacement vers l'est effectué.\n");
        } else {
            printf("Déplacement impossible vers l'est.\n");
        }
    } else if (strcmp(direction, "SE") == 0) {
        // Déplacement vers le sud-est
        if (unite->posy < LIGNES - 1 && unite->posx < COLONNES - 1) {
            unite->posy++;
            unite->posx++;
            printf("Déplacement vers le sud-est effectué.\n");
        } else {
            printf("Déplacement impossible vers le sud-est.\n");
        }
    } else if (strcmp(direction, "S") == 0) {
        // Déplacement vers le sud
        if (unite->posy < LIGNES - 1) {
            unite->posy++;
            printf("Déplacement vers le sud effectué.\n");
        } else {
            printf("Déplacement impossible vers le sud.\n");
        }
    } else if (strcmp(direction, "SO") == 0) {
        // Déplacement vers le sud-ouest
        if (unite->posy < LIGNES - 1 && unite->posx > 0) {
            unite->posy++;
            unite->posx--;
            printf("Déplacement vers le sud-ouest effectué.\n");
        } else {
            printf("Déplacement impossible vers le sud-ouest.\n");
        }
    } else if (strcmp(direction, "O") == 0) {
        // Déplacement vers l'ouest
        if (unite->posx > 0) {
            unite->posx--;
            printf("Déplacement vers l'ouest effectué.\n");
        } else {
            printf("Déplacement impossible vers l'ouest.\n");
        }
    } else if (strcmp(direction, "NO") == 0) {
        // Déplacement vers le nord-ouest
        if (unite->posy > 0 && unite->posx > 0) {
            unite->posy--;
            unite->posx--;
            printf("Déplacement vers le nord-ouest effectué.\n");
        } else {
            printf("Déplacement impossible vers le nord-ouest.\n");
        }
    } else {
        // Direction non reconnue
        printf("Direction non reconnue.\n");
    }
}




















/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pas terminé
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


//to modify zenom
void printgrille(Grille* grille){
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
            Unite* current = &grille;
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
                current = current->usuiv;
            }
        }
        printf("|\n");
    }

    for (int j = 0; j < COLONNES; ++j) {
        printf(" - -");
    }
    printf("\n");
}



void ajout_ruche_nid(Grille* grille, int ligne, int colonne, Unite* reine, char camp) {
    Unite* ruche = malloc(sizeof(Unite));
    ruche->type = RUCHE;
    ajout_unite_case(grille, ligne, colonne, ruche);
    extrait_insecte_affilie(reine);
    Unite* nouvelle_ruche = create_ruche();

    // Étape 3: Link la ruche aux autres ruches (colsuiv, colprec) boucle infinie ?
    ajout_ruche(grille, nouvelle_ruche);
    grille->plateau[ligne][colonne].colonie = camp;  
    grille->plateau[ligne][colonne].occupant = reine;  
    affilie_insecte(nouvelle_ruche, reine, grille);
}


int extrait_ruche_nid(UListe* ruche_nid, Grille** grille) {

    // si c'est une ruche et la premiere dans la liste chainee
    if (((*ruche_nid)->type == RUCHE) && (*grille)->abeille == *ruche_nid) 
        (*grille)->abeille = (*ruche_nid)->colsuiv;

     // si c'est un nid et la premiere dans la liste chainee
    else if (((*ruche_nid)->type == NID) && (*grille)->frelon == *ruche_nid) 
        (*grille)->frelon = (*ruche_nid)->colsuiv;
    
    if((*ruche_nid)->colprec){
        (*ruche_nid)->colprec->colsuiv = (*ruche_nid)->colsuiv;
    }

    if ((*ruche_nid)->colsuiv) {
        (*ruche_nid)->colsuiv->colprec = (*ruche_nid)->colprec;
    }

    (*ruche_nid)->colprec = (*ruche_nid)->colsuiv = NULL;

    return 1;


}



//narmin
int Conversion_ruche_nid(Grille** grille, UListe* ruche_nid, UListe* insecte_gagnant){
    
    extrait_ruche_nid(ruche_nid,grille);
    // s'il s'agit d'une ruche
    if((*ruche_nid)->type == RUCHE)
    {
        (*ruche_nid)-> camp = FRELON;
        (*ruche_nid)-> type = NID;
    }
    //s'il s'agit d'un nid
    (*ruche_nid)-> camp = ABEILLE;
    (*ruche_nid)-> type = RUCHE;
    (*ruche_nid)-> production = 'X';
    (*ruche_nid)-> temps = (*ruche_nid)-> toursrestant = -1;
    if((*ruche_nid)->usuiv){
        for(Unite* temp = *ruche_nid; temp->usuiv; temp = temp->usuiv){
            detruireUnite(*grille, &temp);
        }
    }
    ajout_ruche_nid(grille, *ruche_nid);
    extrait_insecte_affilie(*grille, *insecte_gagnant);
    affilie_insecte(ruche_nid, *insecte_gagnant, grille);
    return 1;


}


// vérifier si la case est vide pour déplacer l'abeille
// 1 si vide 0 sinon
int case_vide(Grille *grille, int posX, int posY) {
    // on regarde si la case fait partie du plateau
    if (posX >= 0 && posX < LIGNES && posY >= 0 && posY < COLONNES) {
        //on verifie si la case est vide
        return !grille->plateau[posX][posY].colonie && !grille->plateau[posX][posY].occupant;
    }
    return -1; // Hors plateau
}

// 0 si pollen pas suffisant ou ruche en train de produire une abeille
// 1 si tout a bien passé
int production_ruche_nid(UListe* ruche_nid, Grille* grille,  char choix_prod){
    //*ruche points to unite
    if(!((*ruche_nid)->production == 'X')) { // la ruche n'est pas en train de produire une autre abeille
        if(choix_prod != REINEA && grille->pollen >= CREINEA){
            (*ruche_nid)-> production = REINEA;
            (*ruche_nid)-> temps = TREINEA;
            (*ruche_nid)-> toursrestant = TREINEA;
            return 1;
        }
        if(choix_prod != OUVRIERE && grille->pollen >= COUVRIERE){
            (*ruche_nid)-> production = OUVRIERE;
            (*ruche_nid)-> temps = TOUVRIERE;
            (*ruche_nid)-> toursrestant = TOUVRIERE;
            return 1;
        }
        if(choix_prod != GUERRIERE && grille->pollen >= CGUERRIERE){
            (*ruche_nid)-> production = GUERRIERE;
            (*ruche_nid)-> temps = TGUERRIERE;
            (*ruche_nid)-> toursrestant = TGUERRIERE; 
            return 1;
        }
        if(choix_prod !=ESCADRON  && grille->pollen >= CESCADRON){
            (*ruche_nid)-> production = ESCADRON;
            (*ruche_nid)-> temps = TESCADRON;
            (*ruche_nid)-> toursrestant = TESCADRON;
            return 1;
        }
        if(choix_prod != REINEF && grille->pollen >= CREINEF){
            (*ruche_nid)-> production = REINEF;
            (*ruche_nid)-> temps = TREINEF;
            (*ruche_nid)-> toursrestant = TREINEF;
            return 1;
        }
        if(choix_prod != FRELON && grille->pollen >= CFRELON){
            (*ruche_nid)-> production = FREL;
            (*ruche_nid)-> temps = TFRELON;
            (*ruche_nid)-> toursrestant = TFRELON;
            return 1;
        }
        
        printf("Pas de pollen suffisant, la quantité du pollen actuelle est: %d\n",grille->pollen);
    }
    else
    {
        printf("La ruche est en train de produire une autre abeille, il reste %d tours pour finir la production",(*ruche_nid)-> toursrestant);

    }
    return 0;   
}


// retourne une cases voisine libre de la ruche choisie
Case* case_voisine_libre(UListe ruche, Grille* grille){

    int x = ruche->posx;
    int y = ruche->posy;

    if (case_vide(grille, x +1, y))
        return &(grille->plateau[x+1][y]);
    if (case_vide(grille, x +1, y+1))
        return &(grille->plateau[x+1][y+1]);
    if (case_vide(grille, x , y+1))
        return &(grille->plateau[x][y+1]);
    if (case_vide(grille, x -1, y))
        return &(grille->plateau[x-1][y]);
    if (case_vide(grille, x -1, y-1))
        return &(grille->plateau[x-1][y-1]);
    if (case_vide(grille, x , y-1))
        return &(grille->plateau[x][y-1]);
    return NULL;

}



// check if (*ruche)-> toursrestant == 0 au début du boucle while du tour dans le main
// initialize abeille, link abeille to ruche, link abeille to case
int ajout_abeille_produite(UListe* ruche, Grille* grille, char choix_prod){

    Case* casee = case_voisine_libre(*ruche, grille);
    if (casee ){
        int x = casee->x;
        int y =  casee->y;
        // initialize abeille et la liér à son case
        Unite* insecte = NULL;
        if(choix_prod != REINEA)
            insecte = initializeUnite(&grille, ABEILLE, choix_prod, FREINE, x,y , -1,-1, 'X', -1, -1);
        if(choix_prod != OUVRIERE)
            insecte = initializeUnite(&grille, ABEILLE, choix_prod, FOUVRIERE, x,y , -1,-1, 'X', -1, -1);
        if(choix_prod != GUERRIERE)
            insecte = initializeUnite(&grille, ABEILLE, choix_prod, FGUERRIERE, x,y , -1,-1, 'X', -1, -1);
        if(choix_prod != ESCADRON)
            insecte = initializeUnite(&grille, ABEILLE, choix_prod, FESCADRON, x,y , -1,-1, 'X', -1, -1);
        // affilier l'abeille à sa ruche
         if(insecte){
            affilie_insecte(ruche, insecte, grille);
            return 1;  
         }

    }
    return 0;
}

int ajout_frelon_produit(UListe* nid, Grille* grille, char choix_prod){
    Case* casee = case_voisine_libre(*nid, grille);
    if (casee){
        int x = casee->x;
        int y =  casee->y;
        // initialize frelon et le liér à son case
        Unite* insecte = NULL;
        if(choix_prod!= REINEF)
            insecte = initializeUnite(&grille, FRELON, choix_prod, FREINE, x,y , -1,-1, 'X', -1, -1);
        if(choix_prod != FREL)
            insecte = initializeUnite(&grille, FRELON, choix_prod, FFRELON, x,y , -1,-1, 'X', -1, -1);
        // affilier le frelon à son nid
        if(insecte){
            affilie_insecte(nid, insecte, grille);
            return 1;}
    }
    return 0;
}


void recolter(Grille* grille, UListe* ouvriere) {
    if ((*ouvriere) != NULL && (*ouvriere)->type == OUVRIERE) {
        if ((*ouvriere)->toursrestant > 0) {
            (*ouvriere)->toursrestant--;
        } else {
            grille->pollen++;
            detruireUnite(grille, ouvriere);
        }
    }
}



//retourne 1 si il ya au moins 2 camps différents sinon 0
int combat_case( Case* casee){

    int nb_abeille = 0;
    int nb_frelons = 0;
    // case vide
    if(!(casee->occupant))
        return 0;       
    Unite* curr = casee->occupant;
    do
    {
        if (curr->camp == ABEILLE)
            nb_abeille++;
        else
            nb_frelons++;
        curr = curr->vsuiv;

    }while(curr->vsuiv);
    // il existe au moins deux camps differents dans la case
    if (nb_abeille > 0 && nb_frelons >0)
        return 1;

}

// déterminer l'abeille qui va intervenir le premier
Unite* abeilles_fighter( Case* casee){
    Unite* curr = casee->occupant;
    do
    {
        if (curr->type == ESCADRON)
            return curr;
        if (curr->type == GUERRIERE)
            return curr;
        if (curr->type == REINEA)
            return curr;
        if (curr->type == OUVRIERE)
            return curr;
        curr = curr->vsuiv;

    }while(curr->vsuiv);
    return curr;
}
// déterminer le frelon qui va intervenir le premier
Unite* frelons_fighter( Case*casee){
    Unite* curr = casee->occupant;
    do
    {
        if (curr->type == FREL)
            return curr;
        if (curr->type == REINEF)
            return curr;
        curr = curr->vsuiv;

    }while(curr->vsuiv);
    return curr;
}

int tire_De(){
    return rand() % 60 +1;// random number between 1 and 60
}

Unite* insecte_gagnant(Unite* unite1, Unite* unite2){
    int result = tire_De();
    // unite1 et unite2 forces diff de 0 donc ils ne sont ni ruche ni nid
    if (unite1->force && unite2->force){
        if(result * (unite1->force) > result * (unite2->force) )
            return unite1;
         else
            return unite2;
    }
    else{
        if(!(unite1->force))
            return unite2;
        else
            return unite1;
    }

}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pas terminé
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */














void tour(Grille* grille, char camp) {
    // Première étape : déterminer le premier camp qui jouera
    // Cela est déjà fait dans le main avant d'appeler cette fonction

    // Deuxième étape : donner les commandes de production des ruches ou nids
    printf("Voulez-vous produire une ruche ou une abeille ? (o pour oui) ");
    char rep;
    scanf("%c", rep);
    if (rep=='o'){
        if (camp == ABEILLE) {
            donnerCommandesProduction(grille->abeille, grille);
        } else {
            donnerCommandesProduction(grille->frelon, grille);
        }
    }

    // Troisième étape : donner les ordres aux insectes
    if (camp == ABEILLE) {
        donnerOrdresInsectes(grille->abeille, grille);
        return;
    } else {
        donnerOrdresInsectes(grille->frelon, grille);
        return;
    }

    // Quatrième étape : résoudre les combats
    // Les combats sont résolus après que les deux camps aient joué
    // Ainsi, on ne résout les combats que dans le main après les deux appels à tour

    // Cinquième étape : mettre à jour les ressources
    // C'est également fait dans le main après la résolution des combats
}

void donnerCommandesProduction(UListe liste, Grille* grille) {
    UListe currentUnit;

    for (currentUnit = liste; currentUnit != NULL; currentUnit = currentUnit->usuiv) {
        switch (currentUnit->type) {
            case REINEA:
                ProductionRuche(currentUnit, grille);
                break;
            case REINEF:
                ProductionNid(currentUnit, grille);
                break;
            case RUCHE:
            case NID:
                ProductionUnite(currentUnit, grille);
                break;
            default:
                break;
        }
    }
}

void ProductionUnite(Unite* unite, Grille* grille) {
    // Logique de production générique pour les ruches et nids
    if (unite->temps > 0) {
        unite->temps--; // Réduire le temps restant pour la production
    } else {
        switch (unite->type) {
            case REINEA:
                // La production de la reine abeille est gérée dans ProductionRuche, donc rien à faire ici
                break;
            case REINEF:
                // La production de la reine frelon est gérée dans ProductionNid, donc rien à faire ici
                break;
            case OUVRIERE:
                if (grille->ressourcesAbeille >= COUVRIERE) {
                    Unite* ouvriere = initialiserUnite(ABEILLE, OUVRIERE, FOUVRIERE, unite->posx, unite->posy);
                    grille->ressourcesAbeille -= COUVRIERE;
                    unite->temps = TOUVRIERE; // Temps nécessaire à la production d'une ouvrière
                }
                break;
            case GUERRIERE:
                if (grille->ressourcesAbeille >= CGUERRIERE) {
                    Unite* guerriere = initialiserUnite(ABEILLE, GUERRIERE, FGUERRIERE, unite->posx, unite->posy);
                    grille->ressourcesAbeille -= CGUERRIERE;
                    unite->temps = TGUERRIERE; // Temps nécessaire à la production d'une guerrière
                }
                break;
            case ESCADRON:
                if (grille->ressourcesAbeille >= CESCADRON) {
                    Unite* escadron = initialiserUnite(ABEILLE, ESCADRON, FESCADRON, unite->posx, unite->posy);
                    grille->ressourcesAbeille -= CESCADRON;
                    unite->temps = TESCADRON; // Temps nécessaire à la production d'un escadron
                }
                break;
            case FREL:
                if (grille->pollen >= CFRELON) {
                    Unite* frelon = initialiserUnite(FRELON, FREL, FFRELON, unite->posx, unite->posy);
                    grille->pollen -= CFRELON;
                    unite->temps = TFRELON; // Temps nécessaire à la production d'un frelon
                }
                break;
            default:
                // Gérer les autres types d'unités si nécessaire
                break;
        }
    }
}


void ProductionRuche(Unite* reine, Grille* grille) {
    if (reine->temps > 0) {
        reine->temps--; // Réduire le temps restant pour la production
    } else if (grille->pollen >= CRUCHE) {
        Unite* ruche = initialiserUnite(ABEILLE, RUCHE, 0, reine->posx, reine->posy);
        grille->pollen -= CRUCHE;

        // Affilier la ruche à la reine
        affilie_insecte(reine, ruche, grille);

        // Mettre la ruche en attente d'un nouvel ordre de production
        ruche->temps = -1;
    }
}

void ProductionNid(Unite* reine, Grille* grille) {
    if (reine->temps > 0) {
        reine->temps--; // Réduire le temps restant pour la production
    } else if (grille->ressourcesAbeille >= CNID) {
        Unite* nid = initialiserUnite(FRELON, NID, 0, reine->posx, reine->posy);
        grille->ressourcesAbeille -= CNID;

        // Affilier le nid à la reine
        affilie_insecte(reine, nid, grille);

        // Mettre le nid en attente d'un nouvel ordre de production
        nid->temps = -1;
    }
}

void donnerOrdresInsectes(UListe liste, Grille* grille) {
    UListe currentUnit;

    for (currentUnit = liste; currentUnit != NULL; currentUnit = currentUnit->usuiv) {
        /*
        MLV_draw_text(currentUnit->posx, currentUnit->posy, currentUnit->type, MLV_COLOR_RED);
        MLV_update_window();
        */

        switch (currentUnit->type) {
            case REINEA:
            case REINEF:
                ordresReine(currentUnit, grille);
                break;
            case GUERRIERE:
            case ESCADRON:
            case FREL:
                ordresGuerriereEscadronFrelon(currentUnit, grille);
                break;
            case OUVRIERE:
                ordresOuvriere(currentUnit, grille);
                break;
            default:
                break;
        }
        /*
        MLV_draw_text(currentUnit->posx, currentUnit->posy, currentUnit->type, MLV_COLOR_GREEN);
        MLV_update_window();
        */
    }
}

void ordresReine(Unite* reine, Grille* grille) {
    printf("Voulez-vous (a) déplacer votre reine ? (b) passer le tour ?");
    char rep;
    scanf(" %c", &rep);  // Utiliser un espace avant %c pour ignorer les caractères blancs

    switch(rep) {
        case 'a':
            deplacerUnite(reine, grille);
            break;
        case 'b':
            // Ne rien faire, passer le tour
            break;
        default:
            // passer le tour
            break;
    }
}

void ordresGuerriereEscadronFrelon(Unite* insecte, Grille* grille) {
    printf("Voulez-vous (a) déplacer votre %c ? (b) passer le tour ?", insecte->type);
    char rep;
    scanf(" %c", &rep);

    switch(rep) {
        case 'a':
            deplacerUnite(insecte, grille);
            break;
        case 'b':
            // Ne rien faire, passer le tour
            break;
        default:
            // passer le tour
            break;
    }
}

void ordresOuvriere(Unite* ouvriere, Grille* grille) {
    printf("Voulez-vous (a) déplacer votre ouvrière ? (b) récolter du pollen ? (c) passer le tour ?");
    char rep;
    scanf(" %c", &rep);
    switch(rep) {
        case 'a':
            deplacerUnite(ouvriere, grille);
            break;
        case 'b':
            // Commencez le processus de récolte de pollen
            if (lancerRecoltePollen(ouvriere, grille)) {
                printf("Récolte de pollen lancée.\n");
            } else {
                printf("Impossible de lancer la récolte de pollen.\n");
            }
            break;
        case 'c':
            // Ne rien faire, passer le tour
            break;
        default:
            // passer le tour
            break;
    }
}

int lancerRecoltePollen(Unite* ouvriere, Grille* grille) {
    // Vérifier si l'ouvrière est sur une case libre sans présence de frelons
    if (caseLibrePourRecolte(ouvriere, grille)) {
        // Marquer l'abeille de type récolte
        ouvriere->type = RECOLTE;

        // Initialiser le compteur de tours de récolte
        ouvriere->toursrestant = TRECOLTE;

        return 1; // Récolte lancée avec succès
    }
    return 0; // Impossible de lancer la récolte
}

// Fonction pour vérifier si une case est libre pour la récolte de pollen
int caseLibrePourRecolte(Unite* ouvriere, Grille* grille) {
    // Vérifier que la case est dans les limites de la grille
    if (ouvriere->posx >= 0 && ouvriere->posx < LIGNES && ouvriere->posy >= 0 && ouvriere->posy < COLONNES) {
        // Vérifier que la case n'est pas occupée par des frelons
        for (Unite* currentUnit = grille->frelon; currentUnit != NULL; currentUnit = currentUnit->vsuiv) {
            if (currentUnit->posx == ouvriere->posx && currentUnit->posy == ouvriere->posy) {
                return 0; // La case est occupée par des frelons
            }
        }
        return 1; // La case n'est pas occupée par des frelons
    }

    return 0; // Case hors des limites de la grille
}

// Fonction pour mettre à jour l'état de l'ouvrière pendant la récolte de pollen
void UpdateRecoltePollen(Unite* ouvriere, Grille* grille) {
    if (ouvriere->toursrestant > 0) {
        // Décrémenter le compteur de tours restants
        ouvriere->toursrestant--;
        grille->pollen++;
        }

        // Si les tours restants atteignent zéro, détruire l'ouvrière
        if (ouvriere->toursrestant == 0) {
            detruireUnite(ouvriere, grille);
        }
}


















/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pas terminé
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


void resolveCombat(Unite* insecte, Grille* grille) {
    Unite* abeille_fighter = abeilles_fighter(case);
    Unite* frelon_fighter = frelons_fighter(case);
    Unite* insecte_gagnant = insecte_gagnante(abeille_fighter, frelon_fighter);
    Unite* unite_perdu = (insecte_gagnant == abeille_fighter) ? frelon_fighter : abeille_fighter;
    // si l'insecte perdu est une insectr
    if (unite_perdu->force){
        detruireUnite(grille, &unite_perdu);
        return;
    }
    Conversion_ruche_nid(&grille, &unite_perdu, &insecte_gagnant);

}

void printgrille(Grille* grille) {
    // Implémentez la logique pour afficher la grille
}

int PasdeTroupes(Grille* grille, char camp) {
    // Implémentez la logique pour vérifier s'il n'y a plus de troupes du camp spécifié
    // sur la grille
    return 0; // À adapter en fonction de l'implémentation
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pas terminé
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */













int main() {
    srand(time(NULL));
    int fin = 0;

    Grille* grille = (Grille*)malloc(sizeof(Grille));
    memset(grille, 0, sizeof(Grille));
    initialiserGrille(grille);

    Unite* ruche1 = initialiserUnite(ABEILLE, RUCHE, 0, 0, 0);
    grille->abeille = ruche1; // lien grille à abeille
    Unite* reine1 = initialiserUnite(ABEILLE, REINEA, FREINE, 3, 3); // pos(3,3)
    Unite* reine1_2 = initialiserUnite(ABEILLE, REINEA, FREINE, 4, 5); // pos(4,5)
    Unite* ouvriere1 = initializeUnite(ABEILLE, OUVRIERE, FOUVRIERE, 8, 3);// pos(8,3)
    Unite* guerriere1 = initializeUnite(ABEILLE, GUERRIERE, FGUERRIERE, 5, 4);// pos(5,4)
    Unite* escadron1 = initializeUnite(ABEILLE, ESCADRON, FESCADRON, 10, 7);// pos(10,7)
    affilie_insecte(&ruche1,reine1, grille);
    affilie_insecte(&ruche1,reine1_2, grille);
    affilie_insecte(&ruche1,ouvriere1, grille);
    affilie_insecte(&ruche1,guerriere1, grille);
    affilie_insecte(&ruche1,escadron1, grille);

    Unite* nid1 = initialiserUnite(FRELON, NID, 0, 11, 17);
    grille->frelon = nid1; // link grille à frelon
    Unite* reinef1 = initialiserUnite(FRELON, REINEF, FREINE, 13, 9);// pos(13,9)
    Unite* frelon1_1 = initialserUnite(FRELON, FREL, FFRELON, 8, 9);// pos(8,9)
    Unite* frelon1_2 = initialiserUnite(FRELON, FREL, FFRELON, 9, 9);// pos(9,9)
    Unite* frelon1_3 = initialiserUnite(FRELON, FREL, FFRELON, 10, 9);// pos(10,9)
    affilie_insecte(&nid1,reinef1, grille);
    affilie_insecte(&nid1,frelon1_1, grille);
    affilie_insecte(&nid1,frelon1_2, grille);
    affilie_insecte(&nid1,frelon1_3, grille);
    // Simulation
    while (!fin) {
        char firstCamp = (rand() % 2 == 0) ? ABEILLE : FRELON;
        char secondCamp = (firstCamp == ABEILLE) ? FRELON : ABEILLE;

        tour(grille, firstCamp);
        tour(grille, secondCamp);

        // Combat
        UListe currentUnit;
        for (currentUnit = grille->abeille; currentUnit != NULL; currentUnit = currentUnit->usuiv) {
            resolveCombat(currentUnit, grille);
        }
        for (currentUnit = grille->frelon; currentUnit != NULL; currentUnit = currentUnit->usuiv) {
            resolveCombat(currentUnit, grille);
        }

        updateResources(grille);

        printgrille(grille);

        if (PasdeTroupes(grille, ABEILLE) || PasdeTroupes(grille, FRELON))
            fin = 1;
    }

    // Libérer la mémoire
    // ...

    return 0;
}
