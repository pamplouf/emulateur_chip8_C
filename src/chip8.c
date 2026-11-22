#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "chip8.h"

//Tableau de la police des differents caracteres qu'on peut afficher
static uint8_t TabFont[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

//fonctions init
chip8_t* create_chip8 (enum quirk ver)
{
    chip8_t* new_chip8 = malloc(sizeof(struct chip8_struct));

    if(new_chip8 == NULL){
        fprintf(stderr, "[create_chip8] error: error during allocation");
        return NULL;
    }

    //init random
    srand(time(NULL));

    //init ram
    for(int i=0; i<RAM; i++)
    {
        new_chip8->memory[i]=0;
    }

    //stockage police
    for (int i=0; i<80; i++){
        new_chip8->memory[0x050 + i] = TabFont[i]; //par convention on la stock dans cette zone
    }

    //init registres
    for(int i=0; i<NBREG; i++)
    {
        new_chip8->registers[i]=0;
    }

    //init pile
    for(int i=0; i<STACKLEN; i++)
    {
        new_chip8->stack[i]=0;
    }

    new_chip8->pc = FIRSTADDRESS;
    new_chip8->sp = 0;
    new_chip8->I = 0;
    new_chip8->sound_timer = 0;
    new_chip8->delay_timer = 0;

    //init ecran
    for (int y=0; y<SCREENHEIGHT; y++){
        for(int x=0; x<SCREENLENGTH; x++){
            new_chip8->screen[y][x] = BLACK;
        }
    }

    //init touches
    for(int i=0; i<16; i++)
    {
        new_chip8->keys[i]=0;
    }

    //passage version
    new_chip8->version = ver;

    return new_chip8;
}

void destroy_chip8(chip8_t* chip8_p){
    free(chip8_p);
}

//Fonctions manipulation de la pile
int stack_push(chip8_t *chip8_p, uint16_t adress){
    if(chip8_p == NULL) return 0; 

    if (chip8_p->sp+1 >= STACKLEN){
        fprintf(stderr, "[stack_push] error: stack overflow\n");
        return -1;
    }
    else{
        chip8_p->stack[chip8_p->sp++] = adress;
        return 0;
    }
}

uint16_t stack_pop(chip8_t *chip8_p){
    if(chip8_p == NULL) return 0; 

    if (chip8_p->sp <= 0){
        fprintf(stderr, "[stack_pop] error: stack empty\n");
        return -1;
    }
    else{
        chip8_p->sp--;
        return chip8_p->stack[chip8_p->sp];
    }
}

//Fonction boucle fetch/decode/execute
int emulate_cycle(chip8_t *chip8_p){
    
    if(chip8_p->pc+1 > RAM){
        fprintf(stderr, "[emulate_cycle] error: pc is out of bounds");
        exit(EXIT_FAILURE);
    }

    //fetch (les instructions sont sur 2 octets)
    uint8_t str_byte = chip8_p->memory[chip8_p->pc]; //recup octet fort
    uint8_t weak_byte = chip8_p->memory[chip8_p->pc+1]; //recup octet faible
    uint16_t opcode = ((uint16_t)str_byte << 8) | weak_byte; //on met octet fort sur 1er octet opcode, 
                                                             //| permet d'ajouter logiquement les bits de l'octet faible sur 2e octet opcode
    chip8_p->pc += 2; //decalage à l'ad suivante

    //decode/execute (on fusionne les 2 c'est plus simple)
    //on utilise un & logique pour isoler les bits qui m'interesse et on decale les bits pour qu'ils soient au bon endroit
    uint8_t instr_type = (opcode & 0xF000) >> 12; 
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0X00F0) >> 4;
    uint8_t N = opcode & 0X000F;
    uint8_t NN = opcode & 0X00FF;
    uint16_t NNN = opcode & 0X0FFF;
    uint8_t flag; //pour manipuler le registre

    switch (instr_type)
    {
        case 0x00: //ecran noir
            switch (NN)
            {
                case 0xE0:
                    for (int y=0; y<SCREENHEIGHT; y++){
                        for(int x=0; x<SCREENLENGTH; x++){
                            chip8_p->screen[y][x] = 0;
                        }
                    }
                    break;
                case 0xEE:
                    chip8_p->pc = stack_pop(chip8_p);
                    break;
                default:
                    fprintf(stderr, "[emulate_cycle] error: unknown instruction %X\n", opcode);
                    return -1;
                    break;
            }
            break;
        case 0x01:  //JUMP
            chip8_p->pc = NNN;
            break;
        case 0x02: //SKIP 
            if(stack_push(chip8_p, chip8_p->pc) == -1)
                return -1;
            else
                chip8_p->pc = NNN;
            break;
        case 0x03: //SKIP ==
            if(chip8_p->registers[X] == NN)
                chip8_p->pc += 2;
            break;
        case 0x04: //SKIP !=
            if(chip8_p->registers[X] != NN)
                chip8_p->pc += 2;
            break;
        case 0x05: //SKIP reg=reg
            if(chip8_p->registers[X] == chip8_p->registers[Y])
                chip8_p->pc += 2;
        case 0x06: //SET registre = valeur
            chip8_p->registers[X] = NN;
            break;  
        case 0x07: //ADD registre += valeur
            chip8_p->registers[X] += NN;
            break;
        case 0x08: //operateurs binaires
            switch (N)
            {
                case 0x00: //SET registre = registre
                    chip8_p->registers[X] = chip8_p->registers[Y];
                    break;
                case 0x01: //OR
                    chip8_p->registers[X] |= chip8_p->registers[Y];
                    break;
                case 0x02: //AND
                    chip8_p->registers[X] &= chip8_p->registers[Y];
                    break;
                case 0x03: //XOR
                    chip8_p->registers[X] ^= chip8_p->registers[Y];
                    break;
                case 0x04: //ADD X+Y
                    if(chip8_p->registers[X] + chip8_p->registers[Y] > 0xFF)
                        flag = 1;
                    else
                        flag = 0;

                    chip8_p->registers[X] += chip8_p->registers[Y];
                    chip8_p->registers[0xF] = flag;
                    break;
                case 0x05: //SUB X-Y
                    if(chip8_p->registers[X] >= chip8_p->registers[Y])
                        flag = 1;
                    else
                        flag = 0;

                    chip8_p->registers[X] -= chip8_p->registers[Y];
                    chip8_p->registers[0xF] = flag;
                    break;
                case 0x06: //SHIFT DROITE registre X
                    if(chip8_p->version == VIP)
                        chip8_p->registers[X] = chip8_p->registers[Y];

                    flag = chip8_p->registers[X] & 1; //recup premier bit
                    chip8_p->registers[X] = chip8_p->registers[X] >> 1;

                    chip8_p->registers[0xF] = flag; //le flag s'active si le bit shifte valait 1
                    break;
                case 0x07: //SUB Y-X
                    if(chip8_p->registers[Y] >= chip8_p->registers[X])
                        flag = 1;
                    else
                        flag = 0;

                    chip8_p->registers[Y] -= chip8_p->registers[X];
                    chip8_p->registers[0xf] = flag;
                    break;
                case 0x0E:
                    if(chip8_p->version == VIP)
                        chip8_p->registers[X] = chip8_p->registers[Y];

                    flag = chip8_p->registers[X] >> 7; //recup bit fort
                    chip8_p->registers[X] = chip8_p->registers[X] << 1;

                    chip8_p->registers[0xF] = flag;
                    break;
                default:
                    fprintf(stderr, "[emulate_cycle] error: unknown instruction %X\n", opcode);
                    return -1;
                    break;
            }
            break;
        case 0X09: //SKIP reg != reg
            if(chip8_p->registers[X] != chip8_p->registers[Y]) {
                chip8_p->pc += 2;
            }
            break;
        case 0x0A: //SET index register = valeur
            chip8_p->I = NNN;
            break;
        case 0x0B: //JUMP + offset
            if(chip8_p->version == VIP)
                chip8_p->pc = NNN + chip8_p->registers[0];
            else
                chip8_p->pc = NN + X + chip8_p->registers[X];
            break;
        case 0x0C: //RANDOM
            chip8_p->registers[X] = NN & (rand()%0xFF); //on s'assure que le nombre aleatoire soit sur 8 bits
            break;
        case 0X0D: //DISPLAY (la pire)
            int x = chip8_p->registers[X] % SCREENLENGTH; //modulo sur les positions de depart 
            int y = chip8_p->registers[Y] % SCREENHEIGHT;
            int max_x;
            int max_y;
            flag = 0;

            //Ajustement pour eviter les debordements de l'ecran
            if(x+8 < SCREENLENGTH)
                max_x = 8;
            else
                max_x = SCREENLENGTH-x;

            if(y+N < SCREENHEIGHT)
                max_y = N;
            else
                max_y = SCREENHEIGHT-y;
                
            for(int spriteY=0; spriteY<max_y; spriteY++){ //spriteX = hauteur actuelle ligne sprite a dessiner
                //& 0xFFF sert a boucler sur la memoire pour ne pas faire crash le prog en cas d'erreur de la ROM. Pour autant, on se met a ecraser la font, ce qui va casser les chiffres affichés.
                uint8_t spriteLine = chip8_p->memory[(chip8_p->I + spriteY) & 0xFFF]; //extrcation de l'octet a transformer en pixels 
                
                for(int bitPos=0; bitPos<max_x; bitPos++){ //boucle pour extraire chaque bit de l'octet
                    uint8_t pixel = (spriteLine >> (7-bitPos)) & 1; //decalage du bit qui nous interesse en debut d'octet, masque pour l'isoler

                    if(chip8_p->screen[y+spriteY][x+bitPos]==WHITE && pixel==WHITE)
                        flag = 1;
                    chip8_p->screen[y+spriteY][x+bitPos] ^= pixel; //changement du pixel du tableau (XOR) en fonction du bit extrait 
                }
            }
                
            chip8_p->registers[0xF] = flag;
            break;
        case 0x0E: //SKIP si touche
            switch (NN)
            {
                case 0x9E:
                    if(chip8_p->keys[chip8_p->registers[X]] == 1)
                        chip8_p->pc += 2;
                    break;
                case 0xA1:
                    if(chip8_p->keys[chip8_p->registers[X]] == 0)
                        chip8_p->pc += 2;
                    break;
                default:
                    break;
            }
            break;
        case 0x0F: 
            switch (NN)
            {
                case 0x07: //TIMER registre = delay
                    chip8_p->registers[X] = chip8_p->delay_timer;
                    break;
                case 0x15: //TIMER delay = registre
                    chip8_p->delay_timer = chip8_p->registers[X];
                    break;
                case 0x18: //TIMER sound = registre
                    chip8_p->sound_timer = chip8_p->registers[X];
                    break;
                case 0x1E: //ADD I + registre
                    flag = 0;
                    chip8_p->I += chip8_p->registers[X]; //on se base sur le quirk de l'amiga donc pas de & 0xFFF

                    if(chip8_p->version != VIP && chip8_p->I > 0xFFF){ 
                        flag = 1;
                        chip8_p->registers[0xF]=flag;
                    }

                    break;
                case 0x0A: //LOOP jusque input
                    chip8_p->pc -= 2;

                    for(int i=0; i<=0xF; i++){
                        if(chip8_p->keys[i] == 1){
                            chip8_p->registers[X] = i;
                            chip8_p->pc += 2;
                            break;
                        }
                    }
                    break;
                case 0x29: //GET addresse police
                    chip8_p->I = 0x50 + ((chip8_p->registers[X] & 0x0F)*5);
                    break;
                case 0x33:
                    chip8_p->memory[chip8_p->I & 0xFFF] = chip8_p->registers[X] / 100; //recup centaines
                    chip8_p->memory[(chip8_p->I+1) & 0xFFF] = (chip8_p->registers[X] / 10) % 10; //recup centaines, dizaines isolation dizaines
                    chip8_p->memory[(chip8_p->I+2) & 0xFFF] = chip8_p->registers[X] % 10; 
                    break;
                case 0x55:
                    for(int i=0; i<=X; i++){
                        if(chip8_p->version == VIP){
                            chip8_p->memory[chip8_p->I++ & 0xFFF] = chip8_p->registers[i];
                        }
                        else{
                            chip8_p->memory[(chip8_p->I+i) & 0xFFF] = chip8_p->registers[i];
                        }
                    }
                    break;
                case 0x65:
                    for(int i=0; i<=X; i++){
                        if(chip8_p->version == VIP){
                            chip8_p->registers[i] = chip8_p->memory[chip8_p->I++ & 0xFFF];
                        }
                        else{
                            chip8_p->registers[i] = chip8_p->memory[(chip8_p->I+i) & 0xFFF];
                        }
                    }
                    break;
                default:
                    fprintf(stderr, "[emulate_cycle] error: unknown instruction %X\n", opcode);
                    break;
            }
            break;
        default:
            fprintf(stderr, "[emulate_cycle] error: unknown instruction %X\n", opcode);
            return -1;
            break;
    }
    return 0;
}

//bcp de gestion d'erreur pour pas entrer rom corrompue en memoire
int loadROM(chip8_t *chip8_p, char* src){
    int ret;
    int rom_size;

    FILE* rom = fopen(src, "rb"); //ouverture en mode binaire
    if(rom == NULL){
        fprintf(stderr, "[loadROM] error: cannot open ROM");
        return -1;
    } 
    ret = fseek(rom, 0, SEEK_END); //recherche dernier octet
    if(ret != 0){
        fprintf(stderr, "[loadROM] error: fseek failed");
        fclose(rom);
        return -1;
    }
    rom_size = ftell(rom); //extraction taille ROM
    if(rom_size > RAM - FIRSTADDRESS){
        fprintf(stderr, "[loadROM] error: ROM size too big for memory\n");
        fclose(rom);
        return -1;
    }
    rewind(rom); //retour pointeur fichier au debut
    ret = fread(&chip8_p->memory[0x200], sizeof(uint8_t), rom_size, rom); //copie ROM en memoire (apres police)
    if(ret != rom_size){
        fprintf(stderr, "[loadROM] error: ROM reading failed");
        fclose(rom);
        return -1;
    }
    fclose(rom);

    return 0;
}

//version par defaut == moderne
enum quirk getVersion(char* ver){
    if(strcmp(ver, "vip")==0)
        return VIP;

    if(strcmp(ver, "modern")==0)
        return MODERN;

    return MODERN;
}

void decrTimers(chip8_t *chip8_p){
    if(chip8_p->delay_timer > 0)
        chip8_p->delay_timer --;
    
    if(chip8_p->sound_timer > 0)
        chip8_p->sound_timer --;
}