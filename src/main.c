#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#define memsize 0x1000

uint8_t font_set[16*5] = {
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


typedef struct chip_8 {
    FILE *game;

    uint8_t memory[memsize]; //0x1000 hex == 4096 dec
    uint8_t V[0x10]; // 16 V registers
    uint16_t I; // I register for storing memory addresses
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint16_t pc;
    uint8_t sp;
    uint16_t stack[0x10];
    uint8_t display[64*32];
    uint8_t keyboard[0x10];
    uint16_t opcode;

} chip_8;

void initialize_chip(char *filepath) {
    chip_8 ch8;
    ch8.pc = 0x200;
    ch8.sp = 0;
    ch8.opcode = 0x200;
    ch8.I = 0;
    ch8.delay_timer = 0;
    ch8.sound_timer = 0;
    memset(ch8.V, 0, sizeof(ch8.V));
    memset(ch8.stack, 0, sizeof(ch8.stack));
    memset(ch8.display, 0, sizeof(ch8.display));
    memset(ch8.keyboard, 0, sizeof(ch8.keyboard));
    memset(ch8.memory, 0, sizeof(ch8.memory));

    ch8.game = fopen(filepath, "rb");
    fseek(ch8.game, 0, SEEK_END);
    long size = ftell(ch8.game);
    fseek(ch8.game, SEEK_END, 0);
    fread(ch8.memory+0x200, 1, size, ch8.game);

    for(int i = 0; i < 80; i++) {
        ch8.memory[i] = font_set[i];
    }

    for(int j = 0; j < memsize; j++) {
        printf("%x ", ch8.memory[j]);
    }
    
}

void decrement_timers(chip_8 *chip) {
    if(chip->delay_timer > 0) chip->delay_timer--;
    if(chip->sound_timer > 0) chip->sound_timer--;
}



int main(int argc, char **argv) {

    printf("Test Mode: \n");
    initialize_chip(argv[1]);
    return 0;
}