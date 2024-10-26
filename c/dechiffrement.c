#include "cadencement.h"
#include "const.h"
#include "bit_operations.h"


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
