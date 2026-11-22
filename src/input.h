#ifndef INPUT_H 
#define INPUT_H 

#define SDL_MAIN_HANDLED
#include "chip8.h"
#include <SDL2/SDL.h>


//traduction touche pressee -> touche clavier chip_8
int key_translate(SDL_Scancode key_p);

//gestion des events, renvoie 1 si on ferme la fenetre de l'emulateur, 0 sinon
int event_handeling(chip8_t *chip8_p);

#endif