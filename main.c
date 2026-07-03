#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


static inline streq(const char *a, const char *b){
    return strcmp(a, b) == 0;
}

static inline uint32_t random_u32(){

#if defined(__linux__) 
#include <sys/random.h>

uint32_t rand_int;
if (getrandom(rand_int, sizeof(rand_int), 0) != sizeof(rand_int)){
    perror("randbit");
    exit(1);
}
return rand_int;

#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <stdlib.h>

uint32_t rand_int;
arc4random_buf(&rand_int, sizeof(rand_int));
return rand_int;

#elif defined(_WIN32)
#include <windows.h>
#include <bcrypt.h>

uint32_t rand_int;
BCryptGenRandom(NULL, (PUCHAR)&rand_int, sizeof(rand_int), BCRYPT_USE_SYSTEM_PREFERRED_RNG);

#endif
}

static inline bool rand_bit(){
    return random_u32() & 1;
}

static inline void print_help(const char *program_exec) {
    printf("Usage: %s [mode] [args...]\n", program_exec);

    puts(
        "Modes:\n"
        "Default (no mode): outputs a random bit (0 or 1)\n"
        "--help, -h              Show this help message\n"
        "--version, -v           Show version and build info\n"
        "--bool, -b              Output bit as true/false\n"
        "--coin, -c              Output bit as heads/tails\n"
        "--int, -i               Output unsigned 32-bit integer\n"
        "--bitstream, --bs       Output 32-bit value as bit stream\n"
        "--label, -l [0] [1]     Custom labels for 0 and 1\n"
    );
}




int main(int argc, char **argv){
    switch (argc)
    {
    case 0:
        putchar('0' + rand_bit());
        putchar('\n');
        break;
    case 1:
    case 2:
        if(!strcmp(argv[1], '--label') || !strcmp(argv[1], '-l'))
        {
            puts(rand_bit() ? 1 : argv[2]);
            return 0;
        }
        else if (
            !strcmp(argv[1], "--help") || !strcmp(argv[1], "-h") ||
            !strcmp(argv[1], "--version") || !strcmp(argv[1], "-v") ||
            !strcmp(argv[1], "--bool") || !strcmp(argv[1], "-b")||
            !strcmp(argv[1], "--coin") || !strcmp(argv[1], "-c") ||
            !strcmp(argv[1], "--int") || !strcmp(argv[1], "-i") ||
            !strcmp(argv[1], "--bitstream") || !strcmp(argv[1], "-s")        
        ) 
        {
            fprintf(stderr, "Too few arguments for mode: \"%s\"", argv[1]);
        }
        else{
            
        }
        
    case 4:
        break;
    default:
        fputs("Too many arguments.\n", stderr);
        print_help(argv[0]);
        return 3;
    }
}
