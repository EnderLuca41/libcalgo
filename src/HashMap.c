#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

unsigned int hash_uint(unsigned int uint){
    unsigned long result = uint + uint + 3;
    result += result + result;
    result += 7;
    result *= result;
    result += uint;
    while(result < 1000000)
        result *= 2;
    
    return result;
}

typedef struct visited {
    unsigned int number;
    bool visited;
    struct visited* next;
} visited;

bool visited_map_insert(visited** map, int size, visited* item){
    if(map == NULL || item == NULL) return false;
    unsigned int index =  hash_uint(item->number) % size;
    item->next = map[index];
    map[index] = item;
    return true;
}

visited* visited_map_lockup(visited** map, int size, unsigned int key){
    unsigned int index = hash_uint(key);
    visited* tmp = map[index];
    while(tmp != NULL && tmp->number != key){
        tmp = tmp->next;
    }

    return tmp;
}

visited* visited_map_delete(visited** map, int size, unsigned int key){
    unsigned int index = hash_uint(key);
    visited* tmp = map[index];
    visited* prev = NULL;
    while(tmp != NULL && tmp->number != key){
        prev = tmp;
        tmp = tmp->next;
    }

    if(tmp != NULL) return NULL;
    if(prev != NULL){
        prev->next = tmp->next;
    }else{
        map[index] = tmp->next;
    }

    return tmp;
}

