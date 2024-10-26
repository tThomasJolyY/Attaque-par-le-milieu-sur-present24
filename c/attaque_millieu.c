#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "couple.h"
#include "chiffrement.h"
#include "dechiffrement.h"
#include "tri.h"

/*
commande pour compiler : gcc -o attaque attaque_millieu.c -lm

boite S :
x    | 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15
S[x] | 12 5  6  11 9  0  10 13 3  14 15 8  4  7  1  2


Permutation:
i    | 0  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15  16  17  18  19  20  21  22 23
P(i) | 0  6  12 18 1  7  13 19 2  8  14  20  3   9   15  21  4   10  16  22  5   11  17 23


algo cadencement de clé present24:
    - la clé maitre est sur 24 bits (ou moins)
    - l'algo de cadencements prend en argument un registre de 80 bits (les 24 bits les plus a gauche sont la clé maitre)
    - 11 tours pour i de 1 a 11
    - a chaque tour :
        registre = registre << 61 (circulaire)
        r[79]r[78]r[77]r[76] = S(r[79]r[78]r[77]r[76])
        r[19]r[18]r[17]r[16]r[15] = r[19]r[18]r[17]r[16]r[15] XOR i
        Ki = r39r38...r17r16

algo chiffrement present24:
    - message m de 24 bits
    - 11 sous clé Ki de 24 bits
    - etat = m
    - 10 tours pour i de 1 a 10
    - a chaque tour :
        etat = etat XOR Ki
        etat = S(etat) => etat est divisé en 6 mots de 4 bits puis concaténation
        etat = P(etat)
    etat = etat XOR K11
    c = etat
*/

int attaque(int m1, int c1, int m2, int c2) {
    printf("Début de l'attaque\n");
    int taille_max = 1;
    taille_max = taille_max << 24;

    struct list *clepot = NULL;

    struct couple* inter_m1 = (struct couple*) malloc(taille_max * sizeof(struct couple));

    printf("Début du calcul de la première liste (present24[Ki,m1])\n");

    for (int i=0;i<taille_max;i++) {
        int c = present24_chiff(i, m1);
        inter_m1[i].cypher = c;
        inter_m1[i].key = i;
    }

    struct couple* inter_c1 = (struct couple*) malloc(taille_max * sizeof(struct couple));

    printf("Début du calcul de la deuxième liste (present24_dec[Ki,c1])\n");

    for (int i=0;i<taille_max;i++) {
        int c = present24_dec(i, c1);
        inter_c1[i].cypher = c;
        inter_c1[i].key = i;
    }

    printf("Tri de la première liste...\n");

    mergeSort(inter_m1, 0, taille_max-1);

    printf("Tri de la deuxième liste...\n");

    mergeSort(inter_c1, 0, taille_max-1);

    int k=0;
    int h=0;

    printf("Parcours des listes pour trouver les couples de clés potentiels...\n");
    while ((k < taille_max) && (h < taille_max)) {
        if (inter_m1[k].cypher == inter_c1[h].cypher) {
            //add to list here
            struct list *node = malloc(sizeof(*node));
            node->k1=inter_m1[k].key;
            node->k2=inter_c1[h].key;
            node->next=clepot;
            clepot=node;
        }

        if (inter_m1[k].cypher > inter_c1[h].cypher) {
            h++;
        } else {
            k++;
        }
    }

    printf("Calcul des couples (K1,K2) valide en vérifiant avec le deuxieme couple (m2,c2)...\n");
    while (clepot) {
        int inter_m2=present24_chiff(clepot->k1, m2);
        int inter_c2=present24_dec(clepot->k2, c2);

        if (inter_m2 == inter_c2) {
            printf("Couple de clé valide trouvée:\n");
            printf("K1=%d      K2=%d\n",clepot->k1,clepot->k2);
        }

        clepot = clepot->next;
    }

    free(inter_m1);
    free(inter_c1);

    return 0;

}

int main() {
    int m1=0x72e7a6;
    int c1=0xc86711;

    int m2=0xab9c13;
    int c2=0xc0df7f;

    attaque(m1, c1, m2, c2);
    return 0;
}


/*
essayer de compiler avec valgrind pour voir les leaks

couple message chifrés:
m1=72e7a6
c1=c86711

m2=ab9c13
c2=c0df7f


K1=9278071      K2=11566410
K1=7664298      K2=5156141
*/
