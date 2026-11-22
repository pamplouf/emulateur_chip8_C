#ifndef CHIP8_H 
#define CHIP8_H 

#include <stdint.h>

//Memoire
#define RAM 4096
#define FIRSTADDRESS 512 //0x200 hexa
#define STACKLEN 16
#define DELAY 60
#define NBREG 16

//Ecran
#define SCREENLENGTH 64
#define SCREENHEIGHT 32
#define SCALE 10
#define BLACK 0
#define WHITE 1

//types
//structure de base
enum quirk{
    VIP,
    MODERN
};


typedef struct chip8_struct{
    uint8_t memory[RAM];
    uint16_t pc; //adresse (case tab mémoire) de prochaine instruction
    uint16_t I; //index register : pointe une case memoire
    uint8_t registers[NBREG];
    uint16_t stack[STACKLEN]; 
    uint8_t sp; //numero fin de pile 

    //compteurs
    uint8_t sound_timer; //tant que > 0, on veut un son
    uint8_t delay_timer; //supposition : tant que > 0, ecran freeze

    //ecran (1 = blanc, 0 = noir)
    uint8_t screen[SCREENHEIGHT][SCREENLENGTH];

    //touches de 0 a F (1 enfonce, 0 lache)
    uint8_t keys[0xF];

    enum quirk version;
}chip8_t;

//fonctions

//initialisation
chip8_t* create_chip8(enum quirk ver);
void destroy_chip8(chip8_t* chip8_p);

//actions pile
int stack_push(chip8_t *chip8_p, uint16_t adress);
uint16_t stack_pop(chip8_t *chip8_p);

//boucle processeur
int emulate_cycle(chip8_t *chip8_p);

//copie ROM -> memoire
int loadROM(chip8_t *chip8_p, char* src);

//recuperation version chip8
enum quirk getVersion(char* ver);

//timers -1
void decrTimers(chip8_t *chip8_p);

#endif