#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "chip8.h"
#include "display.h"
#include "input.h"


int main(int argc, char* argv[]) {

    if(argc < 2){
        fprintf(stderr, "error: 1 argument (ROM) needed to start.\n You can run the original COSMAC VIP version (type vip as the 3rd parameter)\n");
        return EXIT_FAILURE;
    }

    enum quirk ver = MODERN; //version de base

    if(argc > 2){
        ver = getVersion(argv[2]);
    }

    chip8_t* chip8 = create_chip8(ver);
    if(chip8 == NULL){
        return EXIT_FAILURE; 
    }
    display_t* display = create_display();
    if(display == NULL){
        free(chip8);
        return EXIT_FAILURE;
    }

    loadROM(chip8, argv[1]);

    //details frequences : 
    //1 boucle = 1 frame donc 16 ms de latence en fin de boucle car emulateur tourne a 60 FPS
    //vitesse cpu = 10 instrutions par boucle (choisit arbitrairement vitesse correcte pour chip8), donc 60*10 600 instructions par seconde
    int main_loop = true; 
    while(main_loop){

        //gestion inputs/boucle
        int loop_cond = event_handeling(chip8);
        
        //cycle processeur
        for(int i=0; i<10; i++){ 
            emulate_cycle(chip8);
        }

        //decr timers
        decrTimers(chip8);
        
        if(loop_cond == 0){
            update_display(display, chip8);
        }
        else{
            free_display(display);
            main_loop = false;
        }

        SDL_Delay(16);
    }

    free(chip8);
    free(display);

    return EXIT_SUCCESS;
}


/*
        //gestion des evenements (inputs)
        SDL_Event event;

        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                free_affichage(&display);
                loop_jeu = 0;
                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                action_touche(chip8, event.key.keysym.scancode, event.type);
                break;

            default:
                break;
            }
        }
*/