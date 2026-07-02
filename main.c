#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


uint32_t random_u32(){

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

bool rand_bit(){
    return random_u32() & 1;
}
