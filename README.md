# Attaque-par-le-milieu-sur-present24


Ce projet a été réalisé durant ma 3eme année de licence dans le cadre d'un cours de cryptographie.



Le langage a utiliser pour réaliser cette attaque nous a été imposé (python) cependant, python est loin d'être adapté pour réaliser ce genre d'attaques.
TODO:
- [x] refaire l'attaque dans un autre langage pour comparer les temps d'exécution (en C)

Le programme en python prend environ 1h30 à s'exécuter et le programme en C prend environ 2 minutes (les 2 programmes ont été exécuté sur la même machine
avec un i5 11G).


A noter que le programme en python est très peu optimisé (utilisation de listes pour représenter les entiers en binaires par exemple) ce qui ralentit énormément le temps d'exécution. Tandis que le programme en C est bien optimisé (utilisation de décalage de bits).
