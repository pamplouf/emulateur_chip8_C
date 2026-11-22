#ifndef DISPLAY_H 
#define DISPLAY_H 

#include "chip8.h"
#define SDL_MAIN_HANDLED //corrige l'erreur avec winmain sous windows
#include <SDL2/SDL.h>


//type de l'affichage
typedef struct display_struct{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
}display_t;


//fonctions
//nettoyage de l'affichage et fermeture de SDL
void free_display(display_t *display);
//creation/allocation structure display
display_t* create_display();
//maj de l'affichage
void update_display(display_t *display, chip8_t *chip_8);
//transforme tableau screen chip8 en tableau de couleurs compatible avec SDL_UpdateTexture
void arrayIntToARGB(uint8_t src[SCREENHEIGHT][SCREENLENGTH], uint32_t* dst);

#endif