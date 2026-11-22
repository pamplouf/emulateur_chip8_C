#include "input.h"
#include <stdio.h>
#define SDL_MAIN_HANDLED //corrige l'erreur avec winmain sous windows
#include <SDL2/SDL.h>

/*trad touches chip 8 (touches lettres = hexa) (azerty ou qwerty peu importe car on utilise scancodes ) :
1	2	3	C       1   2   3   4
4	5	6	D  ->   A   Z   E   R
7	8	9	E       Q   S   D   F
A	0	B	F       Z   X   C   V
*/


int key_translate(SDL_Scancode key_p){
    int chip8_key = -1;

    //recherche de la touche pressee
    switch (key_p)
    {
    case SDL_SCANCODE_1: chip8_key=1; break;
    case SDL_SCANCODE_2: chip8_key=2; break;
    case SDL_SCANCODE_3: chip8_key=3; break;
    case SDL_SCANCODE_4: chip8_key=12; break;
    case SDL_SCANCODE_Q: chip8_key=4; break;
    case SDL_SCANCODE_W: chip8_key=5; break;
    case SDL_SCANCODE_E: chip8_key=6; break;
    case SDL_SCANCODE_R: chip8_key=13; break;
    case SDL_SCANCODE_A: chip8_key=7; break;
    case SDL_SCANCODE_S: chip8_key=8; break;
    case SDL_SCANCODE_D: chip8_key=9; break;
    case SDL_SCANCODE_F: chip8_key=14; break;
    case SDL_SCANCODE_Z: chip8_key=10; break;
    case SDL_SCANCODE_X: chip8_key=0; break;
    case SDL_SCANCODE_C: chip8_key=11; break;
    case SDL_SCANCODE_V: chip8_key=15; break;
    default: break;
    }

    return chip8_key;
}

int event_handeling(chip8_t *chip8_p){
    SDL_Event event; //copie des evenements SDL dans event et les supprime de la pile d'attente
    int pressed_key;
    int released_key;

    while(SDL_PollEvent(&event)) //parcourt des evenements recuperes
    {
        switch (event.type) //selection de ceux qu'on veut gerer
        {
        case SDL_QUIT:
            return 1;
        case SDL_KEYDOWN:
            pressed_key = key_translate(event.key.keysym.scancode); //utilisation scancodes, pour gerer differents types claviers
            if(pressed_key != -1)
                chip8_p->keys[pressed_key]=1;
            break;
        case SDL_KEYUP:
            released_key = key_translate(event.key.keysym.scancode);
            if(released_key != -1)
                chip8_p->keys[released_key]=0;
            break;
        default:
            break;
        }
    }
    return 0;
}

