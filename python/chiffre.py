#liste Sbox contenant dans les indices pairs (0,2,4...) les etats (de 4 bits) et dans les indices imairs les nouveaux etats (1,3,5...)
Sbox = [[0,0,0,0], [1,1,0,0], [0,0,0,1], [0,1,0,1], [0,0,1,0], [0,1,1,0], [0,0,1,1], [1,0,1,1], [0,1,0,0],[1,0,0,1], [0,1,0,1], [0,0,0,0], [0,1,1,0], [1,0,1,0], [0,1,1,1], [1,1,0,1], [1,0,0,0],[0,0,1,1], [1,0,0,1], [1,1,1,0], [1,0,1,0], [1,1,1,1], [1,0,1,1], [1,0,0,0], [1,1,0,0], [0,1,0,0], [1,1,0,1], [0,1,1,1], [1,1,1,0], [0,0,0,1], [1,1,1,1], [0,0,1,0]]

#permutation : dictionnaire contenant en clé l'indice d'un bit et en valeur associé son nouvel indice
permutation= {0: 0, 1: 6, 2: 12, 3: 18, 4: 1, 5: 7, 6: 13, 7: 19, 8: 2, 9: 8, 10: 14, 11: 20, 12: 3, 13: 9, 14: 15, 15: 21, 16: 4, 17: 10, 18: 16, 19: 22, 20: 5, 21: 11, 22: 17, 23: 23}


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

def cadencement(registre_caden):
    l_ki = []
    print()
    for i in range(1,12):
        l_ki.append(registre_caden[40:64])
        print("sous cle",i,":")
        print(hex(bin_to_dec(l_ki[i-1])))
        for j in range(61):
            registre_caden.insert(79,registre_caden.pop(0))
        #print("registre apres left shift:")
        #print(hex(bin_to_dec(registre_caden)))
        res = sub(registre_caden[0:4])
        for j in range(len(res)):
            registre_caden[j] = res[j]

        i_bin = dec_to_bin(i)
        for j in range(5):
            registre_caden[60+j] = xor(registre_caden[60+j],i_bin[j])
    return l_ki

def present24(arguments):
    message = arguments[0]
    cle_maitre = dec_to_bin_no_limit(arguments[1])
    while len(cle_maitre) < 24:
        cle_maitre.insert(0,0)
    registre = [0]*80
    for i in range(24):
        registre[i] = cle_maitre[i]
    print_registre(registre)
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

def print_registre(registre):
    for i in range(len(registre)):
        if i>0 and (i)%4==0:
            print(" ",end="")
        print(registre[i],end="")

m1=hex_to_bin("72e7a6")
c1=hex_to_bin("c86711")

m2=hex_to_bin("ab9c13")
c2=hex_to_bin("c0df7f")

'''
c1_inter=present24([m1,7664298])[0]
c1_test=present24([c1_inter,5156141])[0]
print(c1_test==c1)

c2_inter=present24([m2,9278071])[0]
c2_test=present24([c2_inter,11566410])[0]
print(c2_test==c2)


m1=hex_to_bin("72e7a6")
c1=hex_to_bin("c86711")

c1_inter=present24([m1,9278071])[0]
c1_test=present24([c1_inter,11566410])[0]
print(c1_test==c1)
'''

cle_test=0
c=present24([m1,cle_test])
