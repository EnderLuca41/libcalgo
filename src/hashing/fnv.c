#include <stdint.h>

#define FNV_PRIME32 16777619u
#define FNV_PRIME64 (uint64_t) 1099511628211lu

#define FNV_OFFSET_BASIS32 2166136261lu
#define FNV_OFFSET_BASIS64 (uint64_t) 14695981039346656037lu

unsigned int fnv1a_32(char* arr, int n){
    uint32_t hash = FNV_OFFSET_BASIS32;

    for(int i = 0; i < n; i++){
        hash = hash ^ arr[i];
        hash *= FNV_PRIME32;
    }

    return hash;
}


unsigned long fnv1a_64(char* arr, int n){
    uint64_t hash = FNV_OFFSET_BASIS64;
    for(int i = 0; i < n; i++){
        hash = hash ^ arr[i];
        hash *= FNV_PRIME64;
    }

    return hash;
}

unsigned int fnv1_32(char* arr, int n){
    uint32_t hash = FNV_OFFSET_BASIS32;

    for(int i = 0; i < n; i++){
        hash *= FNV_PRIME32;
        hash = hash ^ arr[i];
    }

    return hash;
}


unsigned long fnv1_64(char* arr, int n){
    uint64_t hash = FNV_OFFSET_BASIS64;
    for(int i = 0; i < n; i++){
        hash *= FNV_PRIME64;
        hash = hash ^ arr[i];
    }

    return hash;
}

unsigned int Ffnv0_32(char* arr, int n){
    uint32_t hash = 0;

    for(int i = 0; i < n; i++){
        hash *= FNV_PRIME32;
        hash = hash ^ arr[i];
    }

    return hash;
}


unsigned long fnv0_64(char* arr, int n){
    uint64_t hash = 0;
    for(int i = 0; i < n; i++){
        hash *= FNV_PRIME64;
        hash = hash ^ arr[i];
    }

    return hash;
}