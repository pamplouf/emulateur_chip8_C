#include "display.h"
#include <stdint.h>
#include <stdbool.h>
#define SDL_MAIN_HANDLED //corrige l'erreur avec winmain sous windows
#include <SDL2/SDL.h>

void free_display(display_t *display){
    SDL_DestroyTexture(display->texture); //pas besoin de verif si parametre est init, fonction accepte NULL
    SDL_DestroyRenderer(display->renderer);
    SDL_DestroyWindow(display->window);
    SDL_Quit();
}

display_t* create_display(){
    display_t* new_display = malloc(sizeof(struct display_struct));
    if(new_display == NULL){
        printf("[create_diplay] error: error during allocation\n");
        return NULL;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) { //init la partie graphique de SDL et verif que ça a marché
        printf("[create_diplay] error: error during SDL initialization\n");
        free(new_display);
        return NULL;
    }

    //faudra mettre le son aussi

    //Init fenêtre
    //dimensions ecran multipliees par scale, car 64x32 est illisible sur un ecran moderne
    new_display->window = SDL_CreateWindow("chip8_emu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREENLENGTH*SCALE, SCREENHEIGHT*SCALE, 0);

    if (!new_display->window) {
        printf("[create_diplay] error: %s\n ", SDL_GetError());
        free_display(new_display);
        free(new_display);
        return NULL;
    }

    //Init renderer
    new_display->renderer = SDL_CreateRenderer(new_display->window, -1, SDL_RENDERER_ACCELERATED); //SRA force utilisation gpu

    if (!new_display->renderer) {
        printf("[create_diplay] error: %s\n ", SDL_GetError());
        free_display(new_display);
        free(new_display);
        return NULL;
    }

    //renderer travaille toujours avec du 64x32, et ajuste en fonction de la fenetre (avec SDL_RenderPresent)
    SDL_RenderSetLogicalSize(new_display->renderer, SCREENLENGTH, SCREENHEIGHT);

    //init texture
    new_display->texture = SDL_CreateTexture(new_display->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREENLENGTH, SCREENHEIGHT);

    if (!new_display->texture) {
        printf("[create_diplay] error: %s\n ", SDL_GetError());
        free_display(new_display);
        free(new_display);
        return NULL;
    }

    return new_display;
}


void update_display(display_t *display, chip8_t *chip_8){
    int pixel_number = SCREENHEIGHT*SCREENLENGTH;
    uint32_t pixels[pixel_number]; //creation tableau 1 dimension de couleurs pour SDL_UpdateTexture

    arrayIntToARGB(chip_8->screen, pixels);
    

    SDL_UpdateTexture(display->texture, NULL, pixels, SCREENLENGTH*sizeof(uint32_t)); //met a jour la texture
    SDL_RenderClear(display->renderer); //vide le renderer
    SDL_RenderCopy(display->renderer, display->texture, NULL, NULL); //donne la texture au renderer
    SDL_RenderPresent(display->renderer); //le renderer donne l'image a la fenetre
}

void arrayIntToARGB(uint8_t src[SCREENHEIGHT][SCREENLENGTH], uint32_t* dst){
    for(int y=0; y<SCREENHEIGHT; y++){
        for(int x=0; x<SCREENLENGTH; x++){
            if(src[y][x] == WHITE)
                dst[(y*SCREENLENGTH)+x] = 0xFFFFFFFF;
            else
                dst[(y*SCREENLENGTH)+x] = 0xFF000000;
        }
    }
}
