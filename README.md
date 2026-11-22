#Chip8_emulator_C
Un emulateur de chip8 codé en C, avec SDL2, commenté en français.
!Brick (brick_chip8.gif)

Le chip8 est un interpreteur d'instructions, développé par par Joseph Weisbecker en 1977, originellement pour le microordinateur COSMAC VIP. Son but etait de simplifier l'ecriture de programmes, en permettant d'utiliser des instructions hexadécimales, au lieu du language machine du processeur du VIP. Il fut plus tard répendu sur des ordinateurs plus récents comme le DREAM 6800.

Ceci est un émulateur complet du chip8, codé en C avec SDL2, que j'ai fais pour m'ameliorer en C et mettre un pied dans le monde de l'émulation. Le son n'est pas supporté, mais les roms fonctionnent normalement sans.

##Controles
traduction clavier chip8 -> azerty (fonctionne aussi en qwerty, seule la disposition des touches compte)
```
1	2	3	C       1   2   3   4
4	5	6	D  ->   A   Z   E   R
7	8	9	E       Q   S   D   F
A	0	B	F       Z   X   C   V
```

##Prérequis
Un terminal Linux, avec make installé pour utiliser le makefile.
Sous windows, l'emulateur fonctionne aussi, mais il faut faire la compilation à la main (ou utiliser un terminal linux dédié comme msys2).

##Compilation
```
$ make //compile le projet
$ make clean //supprime tous les fichiers objets crées
$ make clean_all //supprime tous les fichiers objets crées et l'executable
```

##Execution
Par défaut, l'émulateur utilise les instructions de l'Amiga, qui different légèrement mais sont devenus une norme pour les interpreteurs modernes. Pour utiliser des roms de l'original COSMAC VIP, il faut ajouter l'argument "vip".
Pour utiliser une ROM, il faut la placer dans le dossier rom à la racine du projet.
Bash :
```
$ ./chip8_emu roms/<nom de la rom> <vip si necessaire>
```

Powershell :
```
 .\chip8_emu '.\roms\<nom de la rom>' <vip si necessaire>
```

##References
Sources qui m'ont aidé à la réalisation de l'emulateur.

- https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
- http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
- https://wiki.libsdl.org/SDL2/FrontPage