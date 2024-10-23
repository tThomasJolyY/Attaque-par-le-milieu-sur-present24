import time
from multiprocessing import Pool

#liste Sbox contenant dans les indices pairs (0,2,4...) les etats (de 4 bits) et dans les indices imairs les nouveaux etats (1,3,5...)
Sbox = [[0,0,0,0], [1,1,0,0], [0,0,0,1], [0,1,0,1], [0,0,1,0], [0,1,1,0], [0,0,1,1], [1,0,1,1], [0,1,0,0],[1,0,0,1], [0,1,0,1], [0,0,0,0], [0,1,1,0], [1,0,1,0], [0,1,1,1], [1,1,0,1], [1,0,0,0],[0,0,1,1], [1,0,0,1], [1,1,1,0], [1,0,1,0], [1,1,1,1], [1,0,1,1], [1,0,0,0], [1,1,0,0], [0,1,0,0], [1,1,0,1], [0,1,1,1], [1,1,1,0], [0,0,0,1], [1,1,1,1], [0,0,1,0]]

#permutation : dictionnaire contenant en clé l'indice d'un bit et en valeur associé son nouvel indice
permutation= {0: 0, 1: 6, 2: 12, 3: 18, 4: 1, 5: 7, 6: 13, 7: 19, 8: 2, 9: 8, 10: 14, 11: 20, 12: 3, 13: 9, 14: 15, 15: 21, 16: 4, 17: 10, 18: 16, 19: 22, 20: 5, 21: 11, 22: 17, 23: 23}

#fonction qui vonverti un chiffre hexadecimal en binaire, chaque chiffre hexadecimal est converti sur 4 bits
def hex_to_bin(hex_string):
    hex_to_bin_dict = {'0': '0000', '1': '0001', '2': '0010', '3': '0011', '4': '0100', '5': '0101', '6': '0110', '7': '0111',
                       '8': '1000', '9': '1001', 'a': '1010', 'b': '1011', 'c': '1100', 'd': '1101', 'e': '1110', 'f': '1111'}
    binary_list = [0] * (len(hex_string) * 4)  # initialisation de la liste binaire avec des 0
    idx = 0  # indice pour parcourir la liste binaire
    for char in hex_string:
        bin_str = hex_to_bin_dict[char.lower()]
        for bit in bin_str:
            binary_list[idx] = int(bit)  # conversion des bits en entiers et affectation à la liste binaire
            idx += 1  # incrémentation de l'indice pour passer au bit suivant
    return binary_list

#fonction qui converti un entier en base 10 en binaire
def dec_to_bin_no_limit(decimal):
    binary_list = [int(bit) for bit in bin(decimal)[2:]]
    return binary_list

#fonction qui converti un entier en base 10 (entier inférieur à 31) et le converti en binaire sur 5 bits
def dec_to_bin(n):
    r = 0
    res = [0]*5
    while n != 0:
        i=-1
        while n >= r:
            i+=1
            r = 2**i
        r=0
        res[4-(i-1)] = 1
        n = n - 2**(i-1)
    return res

#fonction qui converti un entier binaire en base 10
def bin_to_dec(l):
    res = 0
    for i in range(len(l)):
        res += 2**(len(l)-i-1) * l[i]
    return res

#fonction qui prend en entrée un bit a et un bit b et qui renvoie le résultat de a XOR b
def xor(a,b):
    if a == 0 and b == 1:
        return 1
    elif a == 1 and b == 0:
        return 1
    else:
        return 0

#fonction qui prend en argument un registre dee 80 bits ou les 24 bits les plus a gauche sont les bits de la clé primaire
#et renvoie une liste l_ki qui contient les 11 clés générés a partir du registre
def cadencement(registre_caden):
    l_ki = []
    for i in range(1,12):
        l_ki.append(registre_caden[40:64])
        for j in range(61):
            registre_caden.insert(79,registre_caden.pop(0))
        res = sub(registre_caden[0:4])
        for j in range(len(res)):
            registre_caden[j] = res[j]

        i_bin = dec_to_bin(i)
        for j in range(5):
            registre_caden[60+j] = xor(registre_caden[60+j],i_bin[j])
    return l_ki

#fonction inverse de la substitution S
def inv_sub(cle):
    for i in range(1,len(Sbox),2):
        if Sbox[i] == cle:
            return Sbox[i-1]

#fonction de substitution S
def sub(cle):
    for i in range(0,len(Sbox),2):
        if Sbox[i] == cle:
            return Sbox[i+1]

#fonction inverse de la permutation
def inv_perm(l):
    lperm =[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    for key, value in permutation.items():
        lperm[key] = l[value]
    return lperm

#fonction effectuant la permutation
def perm(l):
    lperm =[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    for key, value in permutation.items():
        lperm[value] = l[key]
    return lperm

#fonction effectuant le chiffrement de present24
#arguments est une liste contentant 2 éléments : le message à chiffre, et la clé maitre
def present24(arguments):
    message = arguments[0]
    cle_maitre = dec_to_bin_no_limit(arguments[1])
    while len(cle_maitre) < 24:
        cle_maitre.insert(0,0)
    registre = [0]*80
    for i in range(24):
        registre[i] = cle_maitre[i]
    l_ki = cadencement(registre)
    for i in range(10):
        for j in range(len(message)):
            message[j] = xor(message[j],l_ki[i][j])
        for j in range(6):
            res_substitution = sub(message[4*j:4*j+4])
            for k in range(4):
                message[4*j+k] = res_substitution[k]
        message = perm(message)
    for j in range(len(message)):
        message[j] = xor(message[j],l_ki[10][j])
    return [message,cle_maitre]

#fonction effectuant le déchiffrement de present24
#de la meme maniere que la fonction de chiffrement, prend en argument
#une liste contenant le chiffré et la clé maitre
def inv_present24(arguments):
    chifre = arguments[0]
    cle_maitre = dec_to_bin_no_limit(arguments[1])
    while len(cle_maitre) < 24:
        cle_maitre.insert(0,0)
    registre = [0]*80
    for i in range(24):
        registre[i] = cle_maitre[i]
    l_ki = cadencement(registre)
    for j in range(len(chifre)):
        chifre[j] = xor(chifre[j],l_ki[10][j])
    for i in range(9,-1,-1):
        chifre = inv_perm(chifre)
        for j in range(6):
            res_substitution = inv_sub(chifre[4*j:4*j+4])
            for k in range(4):
                chifre[4*j+k] = res_substitution[k]
        for j in range(len(chifre)):
            chifre[j] = xor(chifre[j],l_ki[i][j])
    return [chifre,cle_maitre]

#fonction appelé lors du quickSort
def partition(array, start, end):
  pivot = array[end][0]
  i = start - 1
  for j in range(start, end):
    if array[j][0] <= pivot:
      i = i + 1
      (array[i], array[j]) = (array[j], array[i])

  (array[i + 1], array[end]) = (array[end], array[i + 1])

  return i + 1

#fonction qui trie une liste l, avec start le 1er élément de la liste et end le dernier
def quickSort(array, start, end):
  if start < end:
    pi = partition(array, start, end)
    quickSort(array, start, pi - 1)
    quickSort(array, pi + 1, end)

#fonction qui, pour un chiffrement double avec present24 tel que : c1 = present24[k2](present24[k1](m1)), c2 = present24[k2](present24[k1](m2))
#donne le(s) couple(s) k1k2 utilisé(s) pour le chiffrement de m1 et m2
def attaque_milieu():
    print("création de lm")
    args = [[hex_to_bin("72e7a6"), i] for i in range(2**24)]
    with Pool() as p:
        lm = p.map(present24,args)
    print("création de lc")
    args2 = [[hex_to_bin("c86711"), i] for i in range(2**24)]
    with Pool() as p:
        lc = p.map(inv_present24,args2)
    print("Création des liste lc et lm terminé")
    quickSort(lm,0,2**24-1)
    print("trie de lm terminé")
    quickSort(lc,0,2**24-1)
    print("trie des listes terminé")
    fini = False
    l_cle_pot = []
    jlm = 0
    ilc = 0
    while not fini:
        while lm[jlm][0] >= lc[ilc][0]:
            if lm[jlm][0] == lc[ilc][0]:
                with open("cle_pot.txt","a") as f:
                    l_cle_pot.append([bin_to_dec(lm[jlm][1]),bin_to_dec(lc[ilc][1])])
                f.close()
            if ilc < 2**24-1:
                ilc += 1
            else:
                print("calcul des couples k1 k2 potentiels terminé")
                return l_cle_pot
        if jlm < 2**24-1:
            jlm += 1
        else:
            print("calcul des couples k1 k2 potentiels terminé")
            return l_cle_pot

#cette fonction prend en argument une liste contenant k1 et k2 tel que :
#   c1inter = present24[k1](m1) 
#   m1inter = inv_present24[k2][c1]
#tel que c1inter = m1inter et vérifie si:
#   c2inter = present24[k1](m2)
#   m2inter = inv_present24[k2](c2)
#tel que c2inter = m2inter,
#si oui, alors k1k2 et un couple de clé valide et on l'affiche dans le terminal, et on l'écrit également dans un fichier .txt
def test_couple(arg):
    k1 = arg[0]
    k2 = arg[1]
    m2 = hex_to_bin("ab9c13")
    c2 = hex_to_bin("c0df7f")
    x1 = present24([m2,k1])[0]
    x2 = inv_present24([c2,k2])[0]
    if x1==x2:
        print("couple k1 k2 trouvé, k1 = ",k1," \n et k2 = ",k2,"\n (les clés sont données en base10)")
        with open("cle_valides.txt","a") as f:
            f.write("k1 :{} / k2 :{}\n".format(k1,k2))
            f.close()

#fonction qui appelle la fonction test_couple avec plusieurs processeurs avec une liste contenant des couples k1 k2 potentiels
def find_corresponding_couple(l_cle_pot):
    print("début du calcul du couple k1 k2 valide")
    with Pool() as p:
        p.map(test_couple,l_cle_pot)
           
#main ou on appelle le(s) fonctions désirés
if __name__ == "__main__":
    find_corresponding_couple(attaque_milieu())