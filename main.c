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
    if (!temp)
        return 0;
    // case vide
    if(!temp->occupant){
        temp->occupant = nv_unite; 
        nv_unite->vprec = NULL;
        nv_unite->vsuiv = NULL;
        return 1;
    }
    // case contient une seule unite
    if(!((temp->occupant)->vsuiv)){
        (temp->occupant)->vsuiv = nv_unite;
        nv_unite->vprec = temp->occupant;
        nv_unite->vsuiv = NULL;

    }
    // case contient plusieurs unités
    else
    {
        Unite* curr = (temp->occupant);
        while(curr->vsuiv){
            curr = curr->vsuiv;
        }
        curr->vsuiv = nv_unite;
        nv_unite->vprec = curr;
        nv_unite->vsuiv = NULL;
    }

    return 1;    
}

void extrait_case(Grille** grid, Unite* unite) {
    int x = unite->posx;
    int y = unite->posy;

    // Vérifier si c'est la première unité de la case
    if ((*grid)->plateau[x][y].occupant == unite) {
        (*grid)->plateau[x][y].occupant = unite->usuiv;
    }

    if (unite->uprec != NULL) {
        unite->uprec->usuiv = unite->usuiv;
    }

    if (unite->usuiv != NULL) {
        unite->usuiv->uprec = unite->uprec;
    }

    // Réinitialisation des liens de l'unité (optionnel)
    unite->usuiv = unite->uprec = NULL;
}

void extrait_insecte_affilie(UListe* insecte) {
    // Vérifier si c'est le premier de la liste
    if (insecte->uprec ) 
        insecte->uprec->usuiv = insecte->usuiv;

    // Vérifier si c'est le dernier de la liste
    if (insecte->usuiv) {
        insecte->usuiv->uprec = insecte->uprec;
    }

    // Réinitialisation des liens de l'unité (optionnel)
    insecte->usuiv = insecte->uprec = NULL;
}

// remove insecte from case et ruche
void detruire_insecte(Grille* grid, UListe* insecte) {
 
    extrait_case(&grid, &unite);
    extrait_insecte_affilie(insecte);
    free(*insecte);
}





//initilaize unit et ajout_unit_case
Unite* initialiserUnite(Grille** grid, char camp, char type, int force, int posx, int posy, File* fileW) {
    Unite* unite = (Unite*)malloc(sizeof(Unite));
    if (!unite ) {
        perror("Erreur d'allocation mémoire pour une unité");
        exit(EXIT_FAILURE);
    }

    // Initialiser les champs de l'unité
    unite->camp = camp;
    fprintf(fileW, "%c", unite->camp);
    unite->type = type;
    fprintf(fileW, "%c", unite->type);
    unite->force = force;
    fprintf(fileW, "%d", unite->force);
    unite->posx = posx;
    fprintf(fileW, "%d", unite->posx);
    unite->posy = posy;
    fprintf(fileW, "%d", unite->posy);
    unite->destx = -1; // Valeur par défaut pour destination immobile
    fprintf(fileW, "%d", unite->destx);
    unite->desty = -1;
    fprintf(fileW, "%d", unite->desty);
    unite->production = '\0'; // Pas de production par défaut
    fprintf(fileW, "%c", unite->production);
    unite->temps = 0;
    fprintf(fileW, "%d", unite->temps);
    unite->toursrestant = 0;
    fprintf(fileW, "%d", unite->toursrestant);
    unite->usuiv = NULL;
    unite->uprec = NULL;
    unite->colsuiv = NULL;
    unite->colprec = NULL;
    unite->vsuiv = NULL;
    unite->vprec = NULL;
    ajout_unite_case(grid, new_uFILE *fileW);
    fileW = fopen("ma_sauvegarde.jo", "a");
    if (!fileW)
    {
        printf("Erreur: %s\n", "ma_sauvegarde_jo");
        exit(1);
    }
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

void printGrille(Grille* grille) {
    for (int i = 0; i < LIGNES; ++i) {
        for (int j = 0; j < COLONNES; ++j) {
            printf("-  - -  -");
        }
        for (int j = 0; j < COLONNES; ++j) {
            printf(" ");
        }
        printf("\n");

        for (int j = 0; j < COLONNES; ++j) {
            printf("|");

            // Vérifier la présence de ruche
            if (grille->plateau[i][j].colonie != NULL && grille->plateau[i][j].colonie->type == RUCHE) {
                printf("R");
            } else {
                printf(" ");
            }

            // Vérifier la présence d'abeilles sur la case
            for (Unite* current = grille->plateau[i][j].occupant; current != NULL; current = current->vsuiv) {
                switch (current->type) {
                    case REINEA:
                        printf("r");
                        break;
                    case OUVRIERE:
                        printf("o");
                        break;
                    case GUERRIERE:
                        printf("g");
                        break;
                    case ESCADRON:
                        printf("e");
                        break;
                    default:
                        break;
                }
            }
            printf(" - ");
            // Vérifier la présence de nid
            if (grille->plateau[i][j].colonie != NULL && grille->plateau[i][j].colonie->type == NID) {
                printf("N");
            } else {
                printf(" ");
            }

            // Vérifier la présence de frelons sur la case
            for (Unite* current = grille->plateau[i][j].occupant; current != NULL; current = current->vsuiv) {
                switch (current->type) {
                    case REINEF:
                        printf("r");
                        break;
                    case FREL:
                        printf("f");
                        break;
                    default:
                        break;
                }
            }

            // Espace pour les cases vides
            printf("   ");
        }
        printf("|\n");
    }

    for (int j = 0; j < COLONNES; ++j) {
        printf("-  - -  -");
    }
    printf("\n");
}





// Fonction pour déplacer une unité
void deplacerUnite(Unite* unite, Grille* grille) {
    // Affichage des directions possibles
    printGrille(grille);
    printf("Directions possibles : n (nord), ne (nord-est), e (est), se (sud-est), s (sud), so (sud-ouest), o (ouest), no (nord-ouest)\n");
    // Demander à l'utilisateur d'entrer la direction
    char direction[3];  // "NO" est la plus longue, donc besoin de 3 caractères
    printf("Entrez la direction : \n");
    scanf("%s", direction);

    // Appliquer le déplacement en fonction de la direction
    if (strcmp(direction, "n") == 0) {
        // Déplacement vers le nord
        if (unite->posy > 0) {
            unite->posy--;
            extrait_case(&grid, unite);
            ajout_unite_case(&grid, insecte, unite->posx , unite->posy);
            printf("Déplacement vers le nord effectué.\n"); // Un insecte peut aller sur une case déjà occupé, c'est le principe
        } else {
            printf("Déplacement impossible vers le nord.\n");
        }
    } else if (strcmp(direction, "ne") == 0) {
        // Déplacement vers le nord-est
        if (unite->posy > 0 && unite->posx < COLONNES - 1) {
            unite->posy--;
            unite->posx++;
            extrait_case(&grid, unite);
            ajout_unite_case(&grid, insecte, unite->posx , unite->posy);
            printf("Déplacement vers le nord-est effectué.\n");
        } else {
            printf("Déplacement impossible vers le nord-est.\n");
        }
    } else if (strcmp(direction, "e") == 0) {
        // Déplacement vers l'est
        if (unite->posx < COLONNES - 1) {
            unite->posx++;
            extrait_case(&grid, unite);
            ajout_unite_case(&grid, insecte, unite->posx , unite->posy);
            printf("Déplacement vers l'est effectué.\n");
        } else {
            printf("Déplacement impossible vers l'est.\n");
        }
    } else if (strcmp(direction, "se") == 0) {
        // Déplacement vers le sud-est
        if (unite->posy < LIGNES - 1 && unite->posx < COLONNES - 1) {
            unite->posy++;
            unite->posx++;
            extrait_case(&grid, unite);
            ajout_unite_case(&grid, insecte, unite->posx , unite->posy);
            printf("Déplacement vers le sud-est effectué.\n");
        } else {
            printf("Déplacement impossible vers le sud-est.\n");
        }
    } else if (strcmp(direction, "s") == 0) {
        // Déplacement vers le sud
        if (unite->posy < LIGNES - 1) {
            unite->posy++;
            extrait_case(&grid, unite);
            ajout_unite_case(&grid, insecte, unite->posx , unite->posy);
            printf("Déplacement vers le sud effectué.\n");
        } else {
            printf("Déplacement impossible vers le sud.\n");
        }
    } else if (strcmp(direction, "so") == 0) {
        // Déplacement vers le sud-ouest
        if (unite->posy < LIGNES - 1 && unite->posx > 0) {
            unite->posy++;
            unite->posx--;
            extrait_case(&grid, unite);
            ajout_unite_case(&grid, insecte, unite->posx , unite->posy);
            printf("Déplacement vers le sud-ouest effectué.\n");
        } else {
            printf("Déplacement impossible vers le sud-ouest.\n");
        }
    } else if (strcmp(direction, "o") == 0) {
        // Déplacement vers l'ouest
        if (unite->posx > 0) {
            unite->posx--;
            extrait_case(&grid, unite);
            ajout_unite_case(&grid, insecte, unite->posx , unite->posy);
            printf("Déplacement vers l'ouest effectué.\n");
        } else {
            printf("Déplacement impossible vers l'ouest.\n");
        }
    } else if (strcmp(direction, "no") == 0) {
        // Déplacement vers le nord-ouest
        if (unite->posy > 0 && unite->posx > 0) {
            unite->posy--;
            unite->posx--;
            extrait_case(&grid, unite);
            ajout_unite_case(&grid, insecte, unite->posx , unite->posy);
            printf("Déplacement vers le nord-ouest effectué.\n");
        } else {
            printf("Déplacement impossible vers le nord-ouest.\n");
        }
    } else {
        // Direction non reconnue
        printf("Direction non reconnue.\n");
    }
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

int PasdeTroupes(Grille* grille, char camp) {
    // Implémentez la logique pour vérifier s'il n'y a plus de troupes du camp spécifié
    // sur la grille
    return 0; // À adapter en fonction de l'implémentation
}


void ajout_ruche_nid(Grille* grille, Unite* ruche_nid) {

}


void ProductionNid() {
    Ruche* nouvelle_ruche = malloc(sizeof(Ruche));

}





void ProductionRuche() {

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

void detruireListeUnites(UListe liste, Grille* grille, Unite* frelonCapturant) {
    Unite* current = liste;

    while (current != NULL) {
        Unite* suivant = current->usuiv;

        // Vérifier si l'unité est différente du frelon capturant
        if (current->camp != FRELON) {
            // Ajouter des ressources abeilles pour les abeilles détruites
            if (current->type == REINEA) {
                grille->ressourcesAbeille += 7;
            } else if (current->type == OUVRIERE) {
                grille->ressourcesAbeille += 3;
            } else if (current->type == GUERRIERE) {
                grille->ressourcesAbeille += 5;
            } else if (current->type == ESCADRON) {
                grille->ressourcesAbeille += 6;
            }

            // Retirer l'unité de la grille
            detruireUnite(current, grille);
        }

        // Passer à l'unité suivante dans la liste
        current = suivant;
    }
}

// Fonction pour simuler le lancer d'un dé à 60 faces
int tireDe() {
    // Utilisation d'une fonction de génération de nombres aléatoires appropriée
    // (Par exemple, MLV_integer_random(1, 60) si la bibliothèque MLV est utilisée)
    // Remplacer par MLV
    return 1 + rand() % 60;
}

void combatUnites(UListe campAbeille, UListe campFrelon, Grille* grille) {
    while (campAbeille != NULL && campFrelon != NULL) {
        Unite* abeille = campAbeille;
        Unite* frelon = campFrelon;

        // Calculer les scores de combat
        int scoreAbeille = tireDe() * abeille->force;
        int scoreFrelon = tireDe() * frelon->force;

        // Déterminer le résultat du combat
        if (scoreAbeille > scoreFrelon) {
            // L'abeille gagne
            detruireUnite(frelon, grille);
            // Passer à l'unité suivante dans la liste des abeilles
            campAbeille = campAbeille->usuiv;

        } else if (scoreFrelon > scoreAbeille) {
            // Le frelon gagne
            // Gérer la capture de ruche par un frelon
            if (abeille->type == RUCHE) {
                // Retirer la ruche de la liste campAbeille
                retirerUnite(&campAbeille, abeille);
                //détruire toutes les abeilles de la même colonie
                abeille->camp=FRELON;
                abeille->type = NID;
                affilie_insecte(&abeille, frelon, grille);
                detruireListeUnites(abeille, grille, frelon);
            } else {
                detruireUnite(abeille, grille);
            }

            // Passer à l'unité suivante dans la liste des frelons
            campFrelon = campFrelon->usuiv;
        } else {
            // Égalité, les deux unités sont détruites
            detruireUnite(abeille, grille);
            detruireUnite(frelon, grille);

            // Passer aux unités suivantes dans les listes des abeilles et des frelons
            campAbeille = campAbeille->usuiv;
            campFrelon = campFrelon->usuiv;
        }
    }
}

// Fonction pour déterminer l'ordre des unités dans le combat
int ordreUniteCombat(Unite* unite) {
    // Les escadrons, guerrières, reines et ouvrières sont classés du côté abeilles
    // Les frelons et reines sont classés du côté frelons
    // Les ruches et nids ne participent pas directement au combat
    if (unite->camp == ABEILLE) {
        if (unite->type == ESCADRON) {
            return 0;
        } else if (unite->type == GUERRIERE) {
            return 1;
        } else if (unite->type == REINEA) {
            return 2;
        } else if (unite->type == OUVRIERE) {
            return 3;
        }
    } else if (unite->camp == FRELON) {
        if (unite->type == FREL) {
            return 4;
        } else if (unite->type == REINEF) {
            return 5;
        }
    }

    return -1; // Valeur par défaut en cas de type inconnu
}

// Fonction pour échanger deux unités dans une liste
void echangerUnites(UListe unite1, UListe unite2) {
    Unite temp = *unite1;
    *unite1 = *unite2;
    *unite2 = temp;
}

void trierUnitesPourCombat(UListe* listeFrelon, UListe* listeAbeille) {
    // Utilisation de l'algorithme de tri par sélection pour simplifier
    if (*listeFrelon == NULL || (*listeFrelon)->usuiv == NULL) {
        return; // Rien à trier si la liste est vide ou a un seul élément
    }

    UListe i, j, min;

    // Tri des frelons
    for (i = *listeFrelon; i->usuiv != NULL; i = i->usuiv) {
        // Trouver le minimum dans la partie non triée
        min = i;
        for (j = i->usuiv; j != NULL; j = j->usuiv) {
            // Comparaison pour déterminer l'ordre des unités
            int ordre_i = ordreUniteCombat(min);
            int ordre_j = ordreUniteCombat(j);

            if (ordre_i > ordre_j) {
                min = j;
            }
        }

        // Échanger l'élément minimum avec le premier élément non trié
        if (min != i) {
            echangerUnites(i, min);
        }
    }

    // Tri des abeilles
    if (*listeAbeille == NULL || (*listeAbeille)->usuiv == NULL) {
        return; // Rien à trier si la liste est vide ou a un seul élément
    }

    for (i = *listeAbeille; i->usuiv != NULL; i = i->usuiv) {
        // Trouver le minimum dans la partie non triée
        min = i;
        for (j = i->usuiv; j != NULL; j = j->usuiv) {
            // Comparaison pour déterminer l'ordre des unités
            int ordre_i = ordreUniteCombat(min);
            int ordre_j = ordreUniteCombat(j);

            if (ordre_i > ordre_j) {
                min = j;
            }
        }

        // Échanger l'élément minimum avec le premier élément non trié
        if (min != i) {
            echangerUnites(i, min);
        }
    }
}

void resolveCombat(Unite* unite, Grille* grille) {
    // Initialisation des combattants à NULL
    UListe campfrelon = NULL;
    UListe campabeille = NULL;

    // Recherche des frelons sur la même case
    if (!caseLibrePourRecolte(unite, grille)) {
        for (Unite* currentUnit = grille->frelon; currentUnit != NULL; currentUnit = currentUnit->usuiv) {
            if (currentUnit->posx == unite->posx && currentUnit->posy == unite->posy) {
                affilie_insecte(&campfrelon, currentUnit, grille);
            }
        }
        for (Unite* currentUnit = grille->abeille; currentUnit != NULL; currentUnit = currentUnit->usuiv) {
            if (currentUnit->posx == unite->posx && currentUnit->posy == unite->posy) {
                affilie_insecte(&campabeille, currentUnit, grille);
            }
        }
    }

    // Combat seulement s'il y a des adversaires
    if (campfrelon != NULL && campabeille !=NULL) {
        // Trier les unités pour le combat
        trierUnitesPourCombat(&campfrelon, &campabeille);
        
        // Combat entre l'unité courante et les adversaires sur la même case
        combatUnites(campabeille, campfrelon, grille);
    }
}

char combat(Grille* grille) {
    UListe currentUnit;
    for (currentUnit = grille->abeille; currentUnit != NULL; currentUnit = currentUnit->usuiv) {
            resolveCombat(currentUnit, grille);
        }
    UpdateRecoltePollen(currentUnit, grille);
    printGrille(grille);
    if (PasdeTroupes(grille, ABEILLE)){
        return FRELON;
    }
    if (PasdeTroupes(grille, FRELON)){
        return ABEILLE;
    }
    return 'x'; // pas encore de gagnant
}

void ordresOuvriere(Unite* ouvriere, Grille* grille) {
    printf("Voulez-vous (a) déplacer votre ouvrière ? (b) récolter du pollen ? (c) passer le tour ?\n");
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
        default:
            // passer le tour
            break;
    }
}

void ordresGuerriereEscadronFrelon(Unite* insecte, Grille* grille) {
    printf("Voulez-vous (a) déplacer votre %c ? (b) passer le tour ?\n", insecte->type);
    char rep;
    scanf(" %c", &rep);

    switch(rep) {
        case 'a':
            deplacerUnite(insecte, grille);
            break;
        default:
            // passer le tour
            break;
    }
}

void ordresReine(Unite* reine, Grille* grille) {
    printf("Voulez-vous (a) déplacer votre reine ou (b) passer le tour ?\n");
    char rep;
    scanf(" %c", &rep);  // Utiliser un espace avant %c pour ignorer les caractères blancs

    switch(rep) {
        case 'a':
            deplacerUnite(reine, grille);
            break;
        default:
            // passer le tour
            break;
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

void ProductionNid(Unite* reine, Grille* grille) {
    if (reine->temps > 0) {
        reine->temps--; // Réduire le temps restant pour la production
    } else if (grille->ressourcesAbeille >= CNID) {
        Unite* nid = NULL; // Déclaration en dehors du bloc switch
        int rep;
        printf("Voulez-vous que la reine en (%d,%d) produise un nid pour %d de ressourcesAbeille ?\n 1 : Oui \n 2 : Non\n",reine->posx,reine->posy, CNID);
        scanf(" %d", &rep);
        switch(rep) {
            case 1:
                nid = initialiserUnite(FRELON, NID, 0, reine->posx, reine->posy);
                grille->ressourcesAbeille -= CNID;

                // Affilier le nid à la reine
                affilie_insecte(&nid, reine, grille);

                // Mettre le nid en attente d'un nouvel ordre de production
                nid->temps = -1;
                break;
            default:
                // Passer le tour
                break;
        }
    }
}

void ProductionRuche(Unite* reine, Grille* grille) {
    if (reine->temps > 0) {
        reine->temps--; // Réduire le temps restant pour la production
    } else if (grille->pollen >= CRUCHE) {
        Unite* ruche = NULL; // Déclaration en dehors du bloc switch
        int rep;
        printf("Voulez-vous que la reine en (%d,%d) produise une ruche pour %d de pollen ?\n 1 : Oui \n 2 : Non\n",reine->posx,reine->posy, CNID);
        scanf(" %d", &rep);
        switch(rep) {
            case 1:
                ruche = initialiserUnite(ABEILLE, RUCHE, 0, reine->posx, reine->posy);
                grille->pollen -= CRUCHE;

                // Affilier la ruche à la reine
                affilie_insecte(&ruche, reine, grille);

                // Mettre la ruche en attente d'un nouvel ordre de production
                ruche->temps = -1;
                break;
            default:
                // Passer le tour
                break;
        }
    }
}

void ProductionUnite(Unite* unite, Grille* grille) {
    // Logique de production générique pour les ruches et nids
    if (unite->temps > 0) {
        unite->temps--; // Réduire le temps restant pour la production
    }
    Unite* insecte;
    Case* case_libre=case_voisine_libre(unite, grille);
    if (!case_libre){
        ;   // Si aucune des cases autour n'est disponible ça bloque tous les else
    } else if (unite->camp == ABEILLE) {
        int rep;
        printf("1 : Produire une reine (%d pollen, %d tours)  \n2 : Produire une ouvrière (%d pollen, %d tours)\n3 : Produire une guerrière (%d pollen, %d tours)\n4 : Produire un escadron (%d pollen, %d tours)\n5 : Passer le tour\n", CREINEA, TREINEA, COUVRIERE, TOUVRIERE, CGUERRIERE, TGUERRIERE, CESCADRON, TESCADRON);
        scanf(" %d", &rep);
        switch (rep) {
           
            case 1:
     
                if (grille->ressourcesAbeille >= CREINEA) {
                    insecte= initialiserUnite(ABEILLE, REINEA, FREINE, case_libre->x, case_libre->y);
                    affilie_insecte(&unite, insecte);
                    grille->ressourcesAbeille -= CREINEA;
                    unite->temps = TREINEA; // Temps nécessaire à la production d'une ouvrière
                }
                break;                
            case 2:
                if (grille->ressourcesAbeille >= COUVRIERE) {
                    insecte= initialiserUnite(ABEILLE, OUVRIERE, FOUVRIERE, case_libre->x, case_libre->y);
                    affilie_insecte(&unite, insecte);
                    grille->ressourcesAbeille -= COUVRIERE;
                    unite->temps = TOUVRIERE; // Temps nécessaire à la production d'une ouvrière
                }
                break;
            case 3:
                if (grille->ressourcesAbeille >= CGUERRIERE) {
                    insecte = initialiserUnite(ABEILLE, GUERRIERE, FGUERRIERE, unite->posx, unite->posy);
                    affilie_insecte(&unite, insecte);
                    grille->ressourcesAbeille -= CGUERRIERE;
                    unite->temps = TGUERRIERE; // Temps nécessaire à la production d'une guerrière
                }
                break;
            case 4:
                if (grille->ressourcesAbeille >= CESCADRON) {
                   insecte = insecteinitialiserUnite(ABEILLE, ESCADRON, FESCADRON, unite->posx, unite->posy);
                    affilie_insecte(&unite, insecte);
                    grille->ressourcesAbeille -= CESCADRON;
                    unite->temps = TESCADRON; // Temps nécessaire à la production d'un escadron
                }
                break;
            default:
                // Passer le tour
                break;
        }
    } else {
        int rep;
        printf("1 : Produire une reine (%d ressourcesAbeille, %d tours)\n2 : Produire un frelon (%d ressourcesAbeille, %d tours)\n3 : Passer le tour\n", CREINEF, TREINEF, CFRELON, TFRELON);
        scanf(" %d", &rep);
        switch (rep) {
            case 1:
                // La production de la reine frelon est gérée dans ProductionNid, donc rien à faire ici
                break;
            case 2:
                if (grille->pollen >= CFRELON) {
                    insecte = initialiserUnite(FRELON, FREL, FFRELON, unite->posx, unite->posy);
                    affilie_insecte(&unite, insecte);
                    grille->pollen -= CFRELON;
                    unite->temps = TFRELON; // Temps nécessaire à la production d'un frelon
                }
                break;
            default:
                // Passer le tour
                break;
        }
    }
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
                printf("Choix de la production de la ruche en (%d,%d) :\n",currentUnit->posx,currentUnit->posy);
                ProductionUnite(currentUnit, grille);
                break;
            case NID:
                printf("Choix de la production du nid en (%d,%d) :\n",currentUnit->posx,currentUnit->posy);
                ProductionUnite(currentUnit, grille);
                break;
            default:
                break;
        }
    }
}

void tour(Grille* grille, char camp) {
    // Première étape : déterminer le premier camp qui jouera
    // Cela est déjà fait dans le main avant d'appeler cette fonction

    // Deuxième étape : donner les commandes de production des ruches ou nids
    printGrille(grille);
    printf("Tour des %c\n", camp);
    if (camp == ABEILLE) {
        printf("Pollen : %d\n",grille->pollen);
        donnerCommandesProduction(grille->abeille, grille);
    } else {
        printf("RessourcesAbeille : %d\n",grille->ressourcesAbeille);
        donnerCommandesProduction(grille->frelon, grille);
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
    // C'est également fait dans l'étape de la résolution des combats
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

void updateResources(Grille* grid, Unite* insecte_perdu) {

    if(insecte_perdu->camp == ABEILLE){
        if(insecte_perdu->type == REINEA)
           grid->ressourcesAbeille+= CREINEA;
        if(insecte_perdu->type == OUVRIERE)
           grid->ressourcesAbeille+= COUVRIERE;
        if(insecte_perdu->type == ESCADRON)
           grid->ressourcesAbeille+= CESCADRON;
        if(insecte_perdu->type == GUERRIERE)
           grid->ressourcesAbeille+= CGUERRIERE;
    }
    if(insecte_perdu->camp == FRELON){
        if(insecte_perdu->type == REINEF)
           grid->pollen+= CREINEF;
        if(insecte_perdu->type == FREL)
           grid->pollen += CFRELON;
    }

        
}


void resolveCombat(Grille* grid, Case* casee) {
    Unite* abeille_fighter = abeilles_fighter(casee);
    Unite* frelon_fighter = frelons_fighter(casee);
    Unite* insecte_gagnant = insecte_gagnante(abeille_fighter, frelon_fighter);
    Unite* unite_perdu = (insecte_gagnant == abeille_fighter) ? frelon_fighter : abeille_fighter;
    // si l'insecte perdu est une insectr
    if (unite_perdu->force){
        detruire_insecte(grid, &unite_perdu);
        return;
    }
    Conversion_ruche_nid(&grid, &unite_perdu, &insecte_gagnant);
    updateResources(grid, insecte_perdu);

}








int main() {
    FILE *fileW;
    fileW = fopen("ma_sauvegarde.jo", "a");
    if (!fileW)
    {
        printf("Erreur: %s\n", "ma_sauvegarde_jo");
        exit(1);
    }
    char firstCamp = (rand() % 2 == 0) ? ABEILLE : FRELON;
    char secondCamp = (firstCamp == ABEILLE) ? FRELON : ABEILLE;
    fprintf(fileW, "%c %c", firstCamp, secondCamp);
    srand(time(NULL));
    char fin = 'x';

    Grille* grille = (Grille*)malloc(sizeof(Grille));
    memset(grille, 0, sizeof(Grille));
    initialiserGrille(grille);

    Unite* ruche1 = initialiserUnite(ABEILLE, RUCHE, 0, 0, 0);
    grille->abeille = ruche1; // lien grille à abeille
    Unite* reine1 = initialiserUnite(ABEILLE, REINEA, FREINE, 3, 3); // pos(3,3)
    Unite* reine1_2 = initialiserUnite(ABEILLE, REINEA, FREINE, 4, 5); // pos(4,5)
    Unite* ouvriere1 = initialiserUnite(ABEILLE, OUVRIERE, FOUVRIERE, 8, 3);// pos(8,3)
    Unite* guerriere1 = initialiserUnite(ABEILLE, GUERRIERE, FGUERRIERE, 5, 4);// pos(5,4)
    Unite* escadron1 = initialiserUnite(ABEILLE, ESCADRON, FESCADRON, 10, 7);// pos(10,7)
    affilie_insecte(&ruche1,reine1);
    affilie_insecte(&ruche1,reine1_2);
    affilie_insecte(&ruche1,ouvriere1);
    affilie_insecte(&ruche1,guerriere1);
    affilie_insecte(&ruche1,escadron1);

    Unite* nid1 = initialiserUnite(FRELON, NID, 0, 11, 17);
    grille->frelon = nid1; // link grille à frelon
    Unite* reinef1 = initialiserUnite(FRELON, REINEF, FREINE, 13, 9);// pos(13,9)
    Unite* frelon1_1 = initialiserUnite(FRELON, FREL, FFRELON, 8, 9);// pos(8,9)
    Unite* frelon1_2 = initialiserUnite(FRELON, FREL, FFRELON, 9, 9);// pos(9,9)
    Unite* frelon1_3 = initialiserUnite(FRELON, FREL, FFRELON, 10, 9);// pos(10,9)
    affilie_insecte(&nid1,reinef1);
    affilie_insecte(&nid1,frelon1_1);
    affilie_insecte(&nid1,frelon1_2);
    affilie_insecte(&nid1,frelon1_3);
    // Simulation
    while (!fin) {


        tour(grille, firstCamp);
        tour(grille, secondCamp);
        fin = combat(grille);
        if (fin == FRELON){
            printf("Les frelons ont gagné !");
        } else if (fin == ABEILLE){
            printf("Les abeilles ont gagné !");
        }
    }

    // Libérer la mémoire
    // ...
    return 0;
}

//main our combat
int x,y;
for( x = 0; x< LIGNES; x++){
    for (y = 0; y< COLONNES;y++){
        Case* casee = grid->plateau[x][y];
    }
    while(combat_case(casee)){
        resolveCombat(grid, casee);
    }

}
