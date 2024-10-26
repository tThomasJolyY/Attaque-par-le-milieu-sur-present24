#include <stdio.h>
#include "const.h"

#include "bit_operations.h"


int find_bin_len(__int128 number) {
    int i = 0;
    __int128 tmp = number;
    while (tmp > 0) {
        i++;
        tmp = tmp >> 1;
    }
    return i;
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
    return res ^ offset;
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
