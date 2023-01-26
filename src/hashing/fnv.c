#define FNV_PRIME32 16777619u
#define FNV_PRIME64 1099511628211u

#define FNV_OFFSET_BASIS32 2166136261u
#define FNV_OFFSET_BASIS64 14695981039346656037u

unsigned int fnv1a_32(char* arr, int n){
    unsigned int hash = FNV_OFFSET_BASIS32;

    for(int i = 0; i < n; i++){
        hash = hash ^ arr[i];
        hash *= FNV_PRIME32;
    }

    return hash;
}


unsigned long fnv1a_64(char* arr, int n){
    unsigned long hash = FNV_OFFSET_BASIS64;
    for(int i = 0; i < n; i++){
        hash = hash ^ arr[i];
        hash *= FNV_PRIME64;
    }

    return hash;
}

unsigned int fnv1_32(char* arr, int n){
    unsigned int hash = FNV_OFFSET_BASIS32;

    for(int i = 0; i < n; i++){
        hash *= FNV_PRIME32;
        hash = hash ^ arr[i];
    }

    return hash;
}


unsigned long fnv1_64(char* arr, int n){
    unsigned long hash = FNV_OFFSET_BASIS64;
    for(int i = 0; i < n; i++){
        hash *= FNV_PRIME64;
        hash = hash ^ arr[i];
    }

    return hash;
}

unsigned int Ffnv0_32(char* arr, int n){
    unsigned int hash = 0;

    for(int i = 0; i < n; i++){
        hash *= FNV_PRIME32;
        hash = hash ^ arr[i];
    }

    return hash;
}


unsigned long fnv0_64(char* arr, int n){
    unsigned long hash = 0;
    for(int i = 0; i < n; i++){
        hash *= FNV_PRIME64;
        hash = hash ^ arr[i];
    }

    return hash;
}