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

void retirerUnite(UListe* liste, Unite* unite) {
    if (*liste == unite) {
        // L'unité à retirer est en tête de liste
        *liste = unite->usuiv;
        if (*liste) {
            (*liste)->uprec = NULL;
        }
    } else {
        // L'unité à retirer n'est pas en tête de liste
        if (unite->uprec) {
            unite->uprec->usuiv = unite->usuiv;
        }
        if (unite->usuiv) {
            unite->usuiv->uprec = unite->uprec;
        }
    }

    // Mettre à jour les pointeurs de la liste
    unite->usuiv = NULL;
    unite->uprec = NULL;
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
            printf("Déplacement vers le nord effectué.\n"); // Un insecte peut aller sur une case déjà occupé, c'est le principe
        } else {
            printf("Déplacement impossible vers le nord.\n");
        }
    } else if (strcmp(direction, "ne") == 0) {
        // Déplacement vers le nord-est
        if (unite->posy > 0 && unite->posx < COLONNES - 1) {
            unite->posy--;
            unite->posx++;
            printf("Déplacement vers le nord-est effectué.\n");
        } else {
            printf("Déplacement impossible vers le nord-est.\n");
        }
    } else if (strcmp(direction, "e") == 0) {
        // Déplacement vers l'est
        if (unite->posx < COLONNES - 1) {
            unite->posx++;
            printf("Déplacement vers l'est effectué.\n");
        } else {
            printf("Déplacement impossible vers l'est.\n");
        }
    } else if (strcmp(direction, "se") == 0) {
        // Déplacement vers le sud-est
        if (unite->posy < LIGNES - 1 && unite->posx < COLONNES - 1) {
            unite->posy++;
            unite->posx++;
            printf("Déplacement vers le sud-est effectué.\n");
        } else {
            printf("Déplacement impossible vers le sud-est.\n");
        }
    } else if (strcmp(direction, "s") == 0) {
        // Déplacement vers le sud
        if (unite->posy < LIGNES - 1) {
            unite->posy++;
            printf("Déplacement vers le sud effectué.\n");
        } else {
            printf("Déplacement impossible vers le sud.\n");
        }
    } else if (strcmp(direction, "so") == 0) {
        // Déplacement vers le sud-ouest
        if (unite->posy < LIGNES - 1 && unite->posx > 0) {
            unite->posy++;
            unite->posx--;
            printf("Déplacement vers le sud-ouest effectué.\n");
        } else {
            printf("Déplacement impossible vers le sud-ouest.\n");
        }
    } else if (strcmp(direction, "o") == 0) {
        // Déplacement vers l'ouest
        if (unite->posx > 0) {
            unite->posx--;
            printf("Déplacement vers l'ouest effectué.\n");
        } else {
            printf("Déplacement impossible vers l'ouest.\n");
        }
    } else if (strcmp(direction, "no") == 0) {
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
    Case* case_libre=case_voisine_libre(unite, grille);
    if (!case_libre){
        ;   // Si aucune des cases autour n'est disponible ça bloque tous les else
    } else if (unite->camp == ABEILLE) {
        int rep;
        printf("1 : Produire une reine (%d pollen, %d tours)  \n2 : Produire une ouvrière (%d pollen, %d tours)\n3 : Produire une guerrière (%d pollen, %d tours)\n4 : Produire un escadron (%d pollen, %d tours)\n5 : Passer le tour\n", CREINEA, TREINEA, COUVRIERE, TOUVRIERE, CGUERRIERE, TGUERRIERE, CESCADRON, TESCADRON);
        scanf(" %d", &rep);
        switch (rep) {
            case 1:
                // La production de la reine abeille est gérée dans ProductionRuche, donc rien à faire ici
                break;
            case 2:
                if (grille->ressourcesAbeille >= COUVRIERE) {
                    initialiserUnite(ABEILLE, OUVRIERE, FOUVRIERE, case_libre->x, case_libre->y);
                    grille->ressourcesAbeille -= COUVRIERE;
                    unite->temps = TOUVRIERE; // Temps nécessaire à la production d'une ouvrière
                }
                break;
            case 3:
                if (grille->ressourcesAbeille >= CGUERRIERE) {
                    initialiserUnite(ABEILLE, GUERRIERE, FGUERRIERE, unite->posx, unite->posy);
                    grille->ressourcesAbeille -= CGUERRIERE;
                    unite->temps = TGUERRIERE; // Temps nécessaire à la production d'une guerrière
                }
                break;
            case 4:
                if (grille->ressourcesAbeille >= CESCADRON) {
                    initialiserUnite(ABEILLE, ESCADRON, FESCADRON, unite->posx, unite->posy);
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
                    initialiserUnite(FRELON, FREL, FFRELON, unite->posx, unite->posy);
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



































int main() {
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
    affilie_insecte(&ruche1,reine1, grille);
    affilie_insecte(&ruche1,reine1_2, grille);
    affilie_insecte(&ruche1,ouvriere1, grille);
    affilie_insecte(&ruche1,guerriere1, grille);
    affilie_insecte(&ruche1,escadron1, grille);

    Unite* nid1 = initialiserUnite(FRELON, NID, 0, 11, 17);
    grille->frelon = nid1; // link grille à frelon
    Unite* reinef1 = initialiserUnite(FRELON, REINEF, FREINE, 13, 9);// pos(13,9)
    Unite* frelon1_1 = initialiserUnite(FRELON, FREL, FFRELON, 8, 9);// pos(8,9)
    Unite* frelon1_2 = initialiserUnite(FRELON, FREL, FFRELON, 9, 9);// pos(9,9)
    Unite* frelon1_3 = initialiserUnite(FRELON, FREL, FFRELON, 10, 9);// pos(10,9)
    affilie_insecte(&nid1,reinef1, grille);
    affilie_insecte(&nid1,frelon1_1, grille);
    affilie_insecte(&nid1,frelon1_2, grille);
    affilie_insecte(&nid1,frelon1_3, grille);
    // Simulation
    while (fin == 'x') {
        char firstCamp = (rand() % 2 == 0) ? ABEILLE : FRELON;
        char secondCamp = (firstCamp == ABEILLE) ? FRELON : ABEILLE;

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
