#ifndef VERSION
#define VERSION "1.0.5"
#endif 

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//          RETURN CODES            //
#define EXIT_TOO_MUCH_ARGS 1
#define EXIT_TOO_FEW_ARGS 2
#define EXIT_INVALID_ARGS 3
#define EXIT_GEN_ERR 4
//                                  //



static inline bool streq(const char *a, const char *b){
    return strcmp(a, b) == 0;
}


//          RNG Backends            //
#define RNG_BACKEND_BCRYPT 0 
#define RNG_BACKEND_GETRANDOM 1
#define RNG_BACKEND_ARC4RANDOM 2
#define RNG_BACKEND_URANDOM 3
//                                  //

#ifndef RNG_BACKEND

#ifdef _WIN32
#define RNG_BACKEND RNG_BACKEND_BCRYPT


#elif defined(__linux__) 
#define RNG_BACKEND RNG_BACKEND_GETRANDOM

#elif defined(__APPLE__) || defined(__OpenBSD__) || defined(__FreeBSD__)\
    || defined(__NetBSD__) || defined(__DragonFly__)
#define RNG_BACKEND RNG_BACKEND_ARC4RANDOM

#elif defined(__unix__)
#define RNG_BACKEND RNG_BACKEND_URANDOM 

#else
#error "Invalid backend (RNG_BACKEND)."

#endif 
#endif // #ifndef RNG_BACKEND

#if RNG_BACKEND == RNG_BACKEND_BCRYPT

#include <windows.h>
#include <bcrypt.h>
static inline uint32_t random_u32(){
    uint32_t rand_int;
    BCryptGenRandom(NULL, (PUCHAR)&rand_int, sizeof rand_int, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    return rand_int; 
}


#elif RNG_BACKEND == RNG_BACKEND_GETRANDOM
#include <sys/random.h>

static inline uint32_t random_u32(){
    uint32_t rand_int;

    if (getrandom((void*)&rand_int, sizeof rand_int, 0) != sizeof rand_int){
        perror("randbit: getrandom");
        fputs("randbit: random number generation failed.", stderr);
        exit(EXIT_GEN_ERR);
    }
    return rand_int;
}


#elif RNG_BACKEND == RNG_BACKEND_ARC4RANDOM
#include <stdlib.h>

static inline uint32_t random_u32(){
    uint32_t rand_int;
    arc4random_buf(&rand_int, sizeof rand_int);
    return rand_int;
}

#elif RNG_BACKEND == RNG_BACKEND_URANDOM

#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>
#include <unistd.h>

#ifndef URANDOM_LOCATION
#define URANDOM_LOCATION "/dev/urandom"
#endif



static inline uint32_t random_u32(void)
{
    int fd = open(URANDOM_LOCATION, O_RDONLY);
    if(fd == -1){
        perror("randbit: open");
        fputs( 
                "randbit: random number generation failed "
                "(couldn't open file \"" URANDOM_LOCATION "\").\n",
                stderr
        );
        exit(EXIT_GEN_ERR);
    }

    uint32_t rand_int;

    
    ssize_t n = read(fd, &rand_int, sizeof rand_int);
    if(n != sizeof rand_int)
    {
        perror("randbit: read");
        fputs(
                "randbit: random number generation failed \n" 
                "(couldn't read enough bytes from \"" URANDOM_LOCATION "\").\n",
                stderr
        );
        fprintf(stderr, "bytes read: %zd\n", n);
        exit(EXIT_GEN_ERR);
    }

    return rand_int;
}

#endif


static inline bool rand_bit(void){
    return random_u32() & 1;
}

static inline bool in_modes(const char *mode){
    
    return streq(mode, "--help") || streq(mode, "-h") ||
            streq(mode, "--version") || streq(mode, "-v") ||
            streq(mode, "--bool") || streq(mode, "-b")||
            streq(mode, "--coin") || streq(mode, "-c") ||
            streq(mode, "--int") || streq(mode, "-i") ||
            streq(mode, "--uint") || streq(mode, "-u") ||
            streq(mode, "--bitstream") || streq(mode, "-s") ||
            streq(mode, "--label") || streq(mode, "-l");      
}

static inline void print_help(const char *program_exec) {
    printf("Usage: %s [mode] [args...]\n", program_exec);

    puts(
        "Modes:\n"
        "Default (no mode):      outputs a random bit (0 or 1)\n"
        "--help, -h              Show this help message\n"
        "--version, -v           Show version\n"
        "--bool, -b              Output bit as true/false\n"
        "--coin, -c              Output bit as heads/tails\n"
        "--int, -i               Output signed 32-bit integer\n"
        "--uint,-u               Output unsigned 32-bit integer\n"
        "--bitstream, -s         Output 32-bit value as bit stream\n"
        "--label, -l [0] [1]     Custom labels for 0 and 1\n"
    );
}

static inline void print_version(){
    puts("randbit " VERSION);
}

void as_bitstream(char buf[static 33], uint32_t num){
    for(int i = 31; i >= 0; i--){
        buf[i] = (num & 1) ? '1' : '0';
        num >>= 1;
    }
    buf[32] = '\0';
}





int main(int argc, char **argv){

    #define ARG_ERR(EXIT_CODE)\
    switch (EXIT_CODE){\
        case EXIT_TOO_MUCH_ARGS:\
            fprintf(stderr, "Too many arguments for mode: \"%s\"\n", argv[1]);\
            print_help(argv[0]);\
            return EXIT_TOO_MUCH_ARGS;\
        case EXIT_TOO_FEW_ARGS:\
            fprintf(stderr, "Too few arguments for mode: \"%s\"\n", argv[1]);\
            print_help(argv[0]);\
            return EXIT_TOO_FEW_ARGS;\
        case EXIT_INVALID_ARGS:\
            fprintf(stderr, "Invalid mode: \"%s\"\n", argv[1]);\
            print_help(argv[0]);\
            return EXIT_TOO_FEW_ARGS;\
    }



    switch (argc)
    {
    // if no args (excluding argv[0]) passed. Example: ./randbit 
    case 1: 
        putchar('0' + rand_bit());
        putchar('\n');
        return EXIT_SUCCESS;

    // if only 1 arg (excluding argv[0]) passed. Example: ./randbit --help
    case 2:
        if(!in_modes(argv[1]))
        {
            ARG_ERR(EXIT_INVALID_ARGS)
        }
        if (streq(argv[1], "--help") || streq(argv[1], "-h"))
        {
            print_help(argv[0]);
            return EXIT_SUCCESS;
        }
        if (streq(argv[1], "--version") || streq(argv[1], "-v"))
        {
            print_version();
            return EXIT_SUCCESS;
        }
        if (streq(argv[1], "--bool") || streq(argv[1], "-b"))
        {
            puts(rand_bit() ? "true" : "false");
            return EXIT_SUCCESS;
        }
        if (streq(argv[1], "--coin") || streq(argv[1], "-c"))
        {
            puts(rand_bit() ? "heads" : "tails");
            return EXIT_SUCCESS;
        }
        if (streq(argv[1], "--int") || streq(argv[1], "-i"))
        {
            printf("%d\n", random_u32());
            return EXIT_SUCCESS;
        }
        if (streq(argv[1], "--uint") || streq(argv[1], "-u"))
        {
            printf("%u\n", random_u32());
            return EXIT_SUCCESS;
        }
        if (streq(argv[1], "--bitstream") || streq(argv[1], "-s"))
        {
            char stream[33];
            as_bitstream(stream, random_u32());
            puts(stream);
            return EXIT_SUCCESS;
        }
        if(streq(argv[1], "--label") || streq(argv[1], "-l")){
            ARG_ERR(EXIT_TOO_FEW_ARGS)
        }

    // if only 2 args (excluding argv[0]) passed. Example: ./randbit --label incorrect
    case 3:
        if(streq(argv[1], "--label") || streq(argv[1], "-l"))
        {
            puts(rand_bit() ? "1" : argv[2]);
            return EXIT_SUCCESS;
        }
        else if (in_modes(argv[1])) 
        {
            ARG_ERR(EXIT_TOO_MUCH_ARGS)
        }
        else{
            ARG_ERR(EXIT_INVALID_ARGS)
        }
        
    // if only 3 args (excluding argv[0]) passed. Example: ./randbit --label incorrect correct
    case 4:
        if(!in_modes(argv[1])){
            ARG_ERR(EXIT_INVALID_ARGS)
        }
        if (!(streq(argv[1], "--label") || streq(argv[1], "-l")))
        {
            ARG_ERR(EXIT_TOO_MUCH_ARGS)
        }
        puts(rand_bit() ? argv[3] : argv[2]);
        return EXIT_SUCCESS;
    
    // if more than 3 args (excluding argv[0]) passed. Example: ./randbit --label incorrect correct ...
    // this option is invalid by default
    default:
        if (in_modes(argv[1]))
        {
            fprintf(stderr, "Too many arguments for mode: \"%s\"", argv[1]);
            print_help(argv[0]);
            return EXIT_TOO_MUCH_ARGS;
        }
        fprintf(stderr, "Invalid mode: \"%s\"", argv[1]);
        print_help(argv[0]);
        return EXIT_INVALID_ARGS;
    }
}
