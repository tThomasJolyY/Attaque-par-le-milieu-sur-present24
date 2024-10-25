#include<stdio.h>
#include <stdlib.h>
//#include "const.h"

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

struct couple {
    int cypher;
    int key;
};

struct list {
    int k1;
    int k2;
    struct list *next;
};

const char *bit_rep[16] = {
    [0]="0000", [1]="0001", [2]="0010", [3]="0011",
    [4]="0100", [5]="0101", [6]="0110", [7]="0111",
    [8]="1000", [9]="1001", [10]="1010", [11]="1011",
    [12]="1100", [13]="1101", [14]="1110", [15]="1111"
};

const char *hex_rep[16] = {
    [0]="0", [1]="1", [2]="2", [3]="3",
    [4]="4", [5]="5", [6]="6", [7]="7",
    [8]="8", [9]="9", [10]="a", [11]="b",
    [12]="c", [13]="d", [14]="e", [15]="f"
};

const int sbox[16] = {
    [0]=12, [1]=5, [2]=6, [3]=11,
    [4]=9, [5]=0, [6]=10, [7]=13,
    [8]=3, [9]=14, [10]=15, [11]=8,
    [12]=4, [13]=7, [14]=1, [15]=2
};

const int inv_sbox[16] = {
    [0]=5, [1]=14, [2]=15, [3]=8,
    [4]=12, [5]=1, [6]=2, [7]=13,
    [8]=11, [9]=4, [10]=6, [11]=3,
    [12]=0, [13]=7, [14]=9, [15]=10
};

const int permutation[24] = {
    [0]=0, [1]=6, [2]=12, [3]=18,
    [4]=1, [5]=7, [6]=13, [7]=19,
    [8]=2, [9]=8, [10]=14, [11]=20,
    [12]=3, [13]=9, [14]=15, [15]=21,
    [16]=4, [17]=10, [18]=16, [19]=22,
    [20]=5, [21]=11, [22]=17, [23]=23
};

const int inv_permutation[24] = {
    [0]=0, [1]=4, [2]=8, [3]=12,
    [4]=16, [5]=20, [6]=1, [7]=5,
    [8]=9, [9]=13, [10]=17, [11]=21,
    [12]=2, [13]=6, [14]=10, [15]=14,
    [16]=18, [17]=22, [18]=3, [19]=7,
    [20]=11, [21]=15, [22]=19, [23]=23
};

int find_bin_len(__int128 number) {
    int i = 0;
    __int128 tmp = number;
    while (tmp > 0) {
        i++;
        tmp = tmp >> 1;
    }
    return i;
}

void print_couple(struct couple c) {
    printf("c=%d   clé=%d\n",c.cypher,c.key);
}

void merge(struct couple arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    // Create temp arrays
    struct couple *L = (struct couple*) malloc(n1 * sizeof(struct couple));
    struct couple *R = (struct couple*) malloc(n2 * sizeof(struct couple));

    // Copy data to temp arrays L[] and R[]
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    // Merge the temp arrays back into arr[l..r
    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (L[i].cypher <= R[j].cypher) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of L[],
    // if there are any
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copy the remaining elements of R[],
    // if there are any
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
    free(L);
    free(R);
}

// l is for left index and r is right index of the
// sub-array of arr to be sorted
void mergeSort(struct couple arr[], int l, int r)
{
    if (l < r) {
        int m = l + (r - l) / 2;

        // Sort first and second halves
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);

        merge(arr, l, m, r);
    }
}


void print_as_bin(__int128 number, int mode) { // ex avec number = 18 <=>0001 0010
    int taille = find_bin_len(number); // taille = 5
    //printf("taille = %d\n",taille);
    int size_hexa = 0; // size_hexa = 2
    if (taille%4==0) {
        size_hexa=taille/4;
    } else {
        size_hexa=taille/4 + 1;
    }
    for (int i=size_hexa;i>0;i--) {
        __int128 tmp = number >> (4*(i-1)); // tmp = number >> 4 => tmp = 0001 ||| tmp = number >> 0 => tmp = 0001 0010
        __int128 tmp2 = number >> (4*i); // tmp2 = number >> 8 => tmp2 = 0 ||| tmp2 = number >> 4 => tmp2 = 0001
        tmp2 = tmp2 << 4; // tmp2 = tmp2 << 4 => tmp2 = 0 ||| tmp2 = tmp2 << 4 => tmp2 = 0001 0000
        __int128 res = tmp ^ tmp2;
        if (mode == 0) {
            printf("%s ",bit_rep[res]);
        } else {
            //printf("Valeur de res:%d\n",res);
            printf("%s",hex_rep[res]);
        }
    }
    if (taille == 0) {
        if (mode ==0) {
            printf("0000 ");
        } else {
            printf("0");
        }
    }
}

__int128 circular_left_bit_shift(__int128 number,int decalage, int taille) {
    __int128 y1 = number << decalage;
    __int128 y2 = number >> (taille - decalage);
    __int128 res = y1 | y2;
    __int128 offset = res >> taille;
    offset = offset << taille;
    res = res ^ offset;
    return res;
}

__int128 make_registre(__int128 number) {
    int taille = find_bin_len(number);
    if (taille > 24) {
        printf("La clé entrée est trop grande.\n");
    }
    int decalage =  (80-taille) - (24-taille);
    __int128 registre = number << decalage;
    return registre;
}

void cadencement(int cle, __int128 *sous_cles) {
    __int128 registre = make_registre(cle);
    for (int i=0;i<11;i++) {
        //EXTRACTION DE Ki
        __int128 tmp3 = registre >> 16;
        __int128 off_tmp3 = tmp3 >> 24;
        off_tmp3 = off_tmp3 << 24;
        tmp3 = tmp3 ^ off_tmp3;
        sous_cles[i] = tmp3;

        //LEFT SHIFT DE 61
        registre = circular_left_bit_shift(registre, 32, 80);
        registre = circular_left_bit_shift(registre, 29, 80);

        //SBOX
        __int128 tmp1 = registre >> 76;
        __int128 s_tmp1 = sbox[tmp1];
        s_tmp1 = s_tmp1 << 76;
        tmp1 = tmp1 << 76;
        registre = registre ^ tmp1;
        registre = registre ^ s_tmp1;

        //XOR i
        __int128 tmp2 = registre >> 15;
        __int128 off_tmp2 = tmp2 >> 5;
        off_tmp2 = off_tmp2 << 5;
        tmp2 = tmp2 ^ off_tmp2;
        __int128 res_xor = tmp2 ^ (i+1);
        res_xor = res_xor << 15;
        tmp2 = tmp2 << 15;
        registre = registre ^ tmp2;
        registre = registre ^ res_xor;
    }
}

int perm(int etat, int depart, int cible) {
    int bit = etat >> depart;
    int off_bit = bit >> 1;
    off_bit = off_bit << 1;
    bit = bit ^ off_bit;
    if (bit == 0) {
        return 0;
    } else {
        bit = bit << cible;
        return bit;
    }

}

int present24_chiff(int cle_maitre, int message) {
    __int128 sous_cle[11];
    cadencement(cle_maitre, sous_cle);
    int etat = message;
    for (int i=0;i<10;i++) {
        //XOR avec Ki
        etat = etat ^ sous_cle[i];

        //SBOX
        int tmps[6];
        for (int j=1;j<7;j++) {
            int tmp = etat >> (24-(j*4));
            if (j >= 2) {
                int off_tmp = tmp >> 4;
                off_tmp = off_tmp << 4;
                tmp = tmp ^ off_tmp;
            }
            tmp = sbox[tmp];
            tmp = tmp << (24-(j*4));
            tmps[j-1] = tmp;
        }
        int res_sbox = 0;
        for (int j=0;j<6;j++) {
            res_sbox = res_sbox ^ tmps[j];
        }
        etat = res_sbox;

        //Permutations
        int perms[24];
        for (int j=0;j<24;j++) {
            perms[j] = perm(etat,j,permutation[j]);
        }
        int res_perm = 0;
        for (int j=0;j<24;j++) {
            res_perm = res_perm ^ perms[j];
        }
        etat = res_perm;
    }

    //XOR K11
    etat = etat ^ sous_cle[10];

    return etat;
}

int present24_dec(int cle_maitre, int c) {
    __int128 sous_cle[11];
    cadencement(cle_maitre, sous_cle);
    int etat = c;

    //XOR K11
    etat = etat ^ sous_cle[10];
    for (int i=9;i>=0;i--) {
        //Permutations inverse
        int perms[24];
        for (int j=0;j<24;j++) {
            perms[j] = perm(etat,j,inv_permutation[j]);
        }
        int res_perm = 0;
        for (int j=0;j<24;j++) {
            res_perm = res_perm ^ perms[j];
        }
        etat = res_perm;

        //Sbox inverse
        int tmps[6];
        for (int j=1;j<7;j++) {
            int tmp = etat >> (24-(j*4));
            if (j >= 2) {
                int off_tmp = tmp >> 4;
                off_tmp = off_tmp << 4;
                tmp = tmp ^ off_tmp;
            }
            tmp = inv_sbox[tmp];
            tmp = tmp << (24-(j*4));
            tmps[j-1] = tmp;
        }
        int res_sbox = 0;
        for (int j=0;j<6;j++) {
            res_sbox = res_sbox ^ tmps[j];
        }
        etat = res_sbox;

        //XOR Ki
        etat = etat ^ sous_cle[i];
    }
    return etat;
}

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

    return 1;

}

int main(int argc, char* argv[]) {
    int m1=0x72e7a6;
    int c1=0xc86711;

    int m2=0xab9c13;
    int c2=0xc0df7f;

    //attaque(m1, c1, m2, c2);


    int k1=9278071;
    int k2=11566410;

    int m1_test = present24_chiff(k1, m1);
    int c1_test = present24_chiff(k2, m1_test);

    int m2_test = present24_chiff(k1, m2);
    int c2_test = present24_chiff(k2, m2_test);

    printf("C1 vrai =%d\n",c1);
    printf("C1 test =%d\n",c1_test);
    printf("C2 vrai =%d\n",c2);
    printf("C2 test =%d\n",c2_test);

    return 1;
}


/*
couple message chifrés:
m1=72e7a6
c1=c86711

m2=ab9c13
c2=c0df7f


K1=9278071      K2=11566410
K1=7664298      K2=5156141
*/
