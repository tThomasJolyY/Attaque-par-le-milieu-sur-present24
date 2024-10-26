#include "cadencement.h"
#include "const.h"
#include "bit_operations.h"

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

    //etat XOR K11
    return etat ^ sous_cle[10];
}
