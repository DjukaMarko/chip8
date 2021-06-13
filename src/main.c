#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <SDL2/SDL.h>

#define memsize 0x1000
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32



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

uint8_t buffer_keys[0x10] = {
        SDLK_0,
        SDLK_1,
        SDLK_2,
        SDLK_3,
        SDLK_4,
        SDLK_5,
        SDLK_6,
        SDLK_7,
        SDLK_8,
        SDLK_9,
        SDLK_a,
        SDLK_b,
        SDLK_c,
        SDLK_d,
        SDLK_e,
        SDLK_f
    };


typedef struct chip_8 {
    FILE *game;

    uint8_t memory[memsize]; //0x1000 hex == 4096 dec
    uint8_t V[0x10]; // 16 V registers, the last one is Vf and it's used as a flag.
    uint16_t I; // I register for storing memory addresses
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint16_t pc;
    uint8_t sp;
    uint16_t stack[0x10];
    uint8_t display[SCREEN_WIDTH*SCREEN_HEIGHT];
    uint8_t keyboard[0x10];
    uint16_t opcode;


} chip_8;


void start_window(chip_8 *ch8);
int check_keys(SDL_Window *window, SDL_Event *event,chip_8 *ch8);
void chip8_opcodes(chip_8 *ch8);
void decrement_timers(chip_8 *chip);
void draw_screen(SDL_Window *window, SDL_Surface *surface);

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
    memcpy(ch8.keyboard, buffer_keys, sizeof(buffer_keys));

    ch8.game = fopen(filepath, "rb");
    
    if(ch8.game == NULL) {
        perror("Error: ");
        fflush(stdin);
    } else {
        fseek(ch8.game, 0, SEEK_END);
        long size = ftell(ch8.game);
        printf("Size of the game is 0x%lx\n", size);
        fseek(ch8.game, 0, SEEK_SET);
        fread(ch8.memory+0x200, 1, size, ch8.game);
        if(feof(ch8.game)) {
            printf("bruh %lx\n", ftell(ch8.game));
        }
    }
    /* while(1) { } */

    for(int i = 0; i < 80; i++) {
        ch8.memory[i] = font_set[i];
    }

    for(int j = 0; j < memsize; j++) {
        printf("%x ", ch8.memory[j]);
    }
    printf("NEW LINE : \n\n\n");
    start_window(&ch8);

}
void print_screen(chip_8 *ch8) {
    static int test = 0;
    if(test < 100) {
        printf("NEW LINE : \n\n\n");
        for(int i = 0; i < 2048; i++) {
            printf("%d ", ch8->display[i]);
        }
        test++;
    }
}

void start_window(chip_8 *ch8) {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    SDL_Event event;

    int quit = 0;
    while(!quit) {
        quit = check_keys(window, &event, ch8);
        chip8_opcodes(ch8);
        decrement_timers(ch8);
        print_screen(ch8);
    }
}

void draw_screen(SDL_Window *window, SDL_Surface * surface) {
    SDL_Renderer *renderer;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Rect rectangles[16];
    for(int i = 0 ; i < 16; i++) {
        rectangles[i].w = 10;
        rectangles[i].h = 10;

    }


}

int check_keys(SDL_Window *window, SDL_Event *event, chip_8 *ch8) {
    int quit = 0;
    while(SDL_PollEvent(event)) {
        switch(event->type) {
            case SDL_QUIT:
                quit = 1;
                return quit;
            case SDL_KEYDOWN:
                switch(event->key.keysym.sym) {
                    case SDLK_x:
                        ch8->keyboard[0x0] = 1;
                        break;
                    case SDLK_1:
                        ch8->keyboard[0x1] = 1;
                        break;
                    case SDLK_2:
                        ch8->keyboard[0x2] = 1;
                        break;
                    case SDLK_3:
                        ch8->keyboard[0x3] = 1;
                        break;
                    case SDLK_4:
                        ch8->keyboard[0xC] = 1;
                        break;
                    case SDLK_q:
                        ch8->keyboard[0x4] = 1;
                        break;
                    case SDLK_w:
                        ch8->keyboard[0x5] = 1;
                        break;
                    case SDLK_e:
                        ch8->keyboard[0x6] = 1;
                        break;
                    case SDLK_r:
                        ch8->keyboard[0xD] = 1;
                        break;
                    case SDLK_a:
                        ch8->keyboard[0x7] = 1;
                        break;
                    case SDLK_s:
                        ch8->keyboard[0x8] = 1;
                        break;
                    case SDLK_d:
                        ch8->keyboard[0x9] = 1;
                        break;
                    case SDLK_f:
                        ch8->keyboard[0xE] = 1;
                        break;
                    case SDLK_z:
                        ch8->keyboard[0xA] = 1;
                        break;
                    case SDLK_c:
                        ch8->keyboard[0xB] = 1;
                        break;
                    case SDLK_v:
                        ch8->keyboard[0xF] = 1;
                        break;
                }
            
            
        }
    }
    return quit;
}

int get_pixel(int x, int y, chip_8 *ch8) {
    return ch8->display[x + y*SCREEN_WIDTH];
}


void chip8_opcodes(chip_8 *ch8) {
    ch8->opcode = ch8->memory[ch8->pc] << 8 | ch8->memory[ch8->pc + 1];
    ch8->pc += 2;

    uint8_t Vx = (ch8->opcode & 0x0F00) >> 8;
    uint8_t Vy = (ch8->opcode & 0x00F0) >> 4;
    uint8_t height;
    switch(ch8->opcode & 0xF000) {
        case 0x0000:
            switch(ch8->opcode & 0x0FFF) {
                case 0x00E0: //CLS
                    memset(ch8->display, 0, sizeof(ch8->display));
                    break;
                case 0x00EE: //RET
                    ch8->pc = ch8->stack[ch8->sp];
                    ch8->sp--;
                    break;
            }
        case 0x1000: //JMP
            ch8->pc = ch8->opcode & 0x0FFF;
            break;
        case 0x2000: //CALL
            ch8->sp++;
            ch8->stack[ch8->sp] = ch8->pc;
            ch8->pc = ch8->opcode & 0x0FFF;
            break;
        case 0x3000: //SE Vx, byte
            if(ch8->V[Vx] == (ch8->opcode & 0x00FF)) ch8->pc += 2;
            break;
        case 0x4000: //SNE Vx, byte
            if(ch8->V[Vx] != (ch8->opcode & 0x00FF)) ch8->pc += 2;
            break;
        case 0x5000: // SE Vx, Vy
            if(ch8->V[Vx] == ch8->V[Vy]) ch8->pc += 2;
            break;
        case 0x6000: // LD Vx, byte
            ch8->V[Vx] = ch8->opcode & 0x00FF;
            break;
        case 0x7000: // ADD Vx, byte
            ch8->V[Vx] = ch8->V[Vx] + (ch8->opcode & 0x00FF);
            break;
        case 0x8000: 
            switch(ch8->opcode & 0x000F) {
                case 0x0000: // LD Vx, Vy
                    ch8->V[Vx] = ch8->V[Vy];
                    break;
                case 0x0001: // OR Vx, Vy
                    ch8->V[Vx] |= ch8->V[Vy];
                    break;
                case 0x0002: // AND Vx, Vy
                    ch8->V[Vx] &= ch8->V[Vy];
                    break;
                case 0x0003: // XOR Vx, Vy
                    ch8->V[Vx] ^= ch8->V[Vy];
                    break;
                case 0x0004: // ADD Vx, Vy
                    ch8->V[sizeof(ch8->V) - 1] = ch8->V[Vx] + ch8->V[Vy] > 0xFF;
                    ch8->V[Vx] += ch8->V[Vy];
                    break;
                case 0x0005: // SUB Vx, Vy
                    ch8->V[sizeof(ch8->V) - 1] = ch8->V[Vx] > ch8->V[Vy];
                    ch8->V[Vx] -= ch8->V[Vy];
                    break;
                case 0x0006: //SHR Vx {, Vy}
                    ch8->V[sizeof(ch8->V) - 1] = ch8->V[Vx] & 1;
                    ch8->V[Vx] >>= 1;
                    break;
                case 0x0007: // SUBN Vx, Vy
                    ch8->V[sizeof(ch8->V) - 1] = ch8->V[Vx] < ch8->V[Vy];
                    ch8->V[Vx] = ch8->V[Vy] - ch8->V[Vx];
                    break; 
                case 0x000E: // SHL Vx {, Vy}
                    ch8->V[sizeof(ch8->V) - 1] = ch8->V[Vx] >> 7;
                    ch8->V[Vx] <<= 1;
                    break;
        

            }
        case 0x9000: // SNE Vx, Vy
            if(ch8->V[Vx] != ch8->V[Vy]) ch8->pc += 2;
            break;
        case 0xA000: // LD I, addr
            ch8->I = ch8->opcode & 0x0FFF;
            break;
        case 0xB000: // JP V0, addr
            ch8->pc = (ch8->opcode & 0x0FFF) + ch8->V[0];
            break;
        case 0xC000: // RND, Vx, byte
            ch8->V[Vx] = (ch8->opcode & 0x00FF) & (rand() % 0x100);
            break;
        case 0xD000: // Dxyn - DRW Vx, Vy, nibble
            height = (ch8->opcode & 0x000F);
            ch8->V[0xF] = 0;
            for(int i = 0; i < height; i++) {
                uint8_t sprite = ch8->memory[ch8->I + i];

                for(int j = 0; j < 8; j++) {
                    if((sprite & (0x80 >> j)) != 0) {
                        if(get_pixel(ch8->V[Vx] + j, ch8->V[Vy] + i, ch8) != 0)
                            ch8->V[0xF] = 1; // collision
                        ch8->display[ch8->V[Vx] + j + (ch8->V[Vy] + i) * SCREEN_WIDTH] ^= 1;
                    }
                }
            }
            break;
        case 0xE000: // SKNP Vx
            if(ch8->keyboard[Vx]) ch8->pc += 2;
            break;



    }
}

void decrement_timers(chip_8 *chip) {
    if(chip->delay_timer > 0) chip->delay_timer--;
    if(chip->sound_timer > 0) chip->sound_timer--;
}



int main(int argc, char **argv) {

    srand(time(0));
    printf("Test Mode: \n");
    initialize_chip(argv[1]);
    return 0;
}