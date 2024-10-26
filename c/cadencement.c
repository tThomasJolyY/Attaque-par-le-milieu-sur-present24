#include <stdio.h>
#include "bit_operations.h"
#include "const.h"

__int128 make_registre(__int128 number) {
    int taille = find_bin_len(number);
    if (taille > 24) {
        printf("La clé entrée est trop grande.\n");
    }
    int decalage =  (80-taille) - (24-taille);
    return number << decalage;
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
