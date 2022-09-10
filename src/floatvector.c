#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

#include <floatvector.h>

#define DEFAULT_START_SIZE 50


static bool float_vector_extend(FloatVector *vector){
    float *newArr;
    int32_t newCapacity;

    if(vector->maxSize < vector->capacity * 2) {
        newArr = realloc(vector->arr, vector->maxSize * sizeof(float));
        newCapacity = vector->maxSize;
    }
    else {
        newArr = realloc(vector->arr, vector->capacity * 2 * sizeof(float));
        newCapacity = vector->capacity * 2;
    }

    if(newArr == NULL)
        return false;
    
    vector->arr = newArr;
    vector->capacity = newCapacity;
    return true;
}

int float_vector_shrink(FloatVector *vector, int32_t amount){
    if(vector->capacity - vector->size < amount || amount < 0)
        amount = vector->capacity - vector->size;

    int32_t newCapacity = vector->capacity - amount;
    float *newArr = realloc(vector->arr, sizeof(float) * newCapacity);
    if(newArr == NULL)
        return 0;
    vector->arr = newArr;
    vector->capacity = newCapacity;
    return amount;
}

float* float_vector_add(FloatVector *vector, float val){
    //If there is still enough space in the vector
    if(vector->size < vector->capacity) {
        vector->arr[vector->size] = val;
        vector->size++;
        return &vector->arr[vector->size - 1];
    }

    //If array is already full
    if(vector->size == vector->capacity){
        bool success = float_vector_extend(vector);
        if(!success)
            return NULL;
        vector->arr[vector->size] = val;
        vector->size++;
        return &vector->arr[vector->size - 1];
    }

    //Can only appear if the user modified the value of capacity
    return NULL;
}

int float_vector_remove(FloatVector *vector, int32_t amount){
    if(amount < 0)
        return 0;

    if(amount > vector->size)
        amount = vector->size;
    
    vector->size -= amount;
    return amount;
}

/*
 * Pass -1 at maxSize to have no max size and -1 at startSize to use the default start size.
 * Returns NULL on failure.
 */
FloatVector * float_vector_init(int32_t maxSize, int32_t startSize){
    if(maxSize < 0)
        maxSize = INT32_MAX;
    if(startSize < 0)
        startSize = DEFAULT_START_SIZE;

    if(startSize > maxSize)
        return NULL;
    
    FloatVector *vector = malloc(sizeof(FloatVector));
    if(vector == NULL)
        return NULL;

    float *arr = malloc(sizeof(float) * startSize);
    if(arr == NULL)
        return NULL;
    
    vector->arr = arr;
    vector->capacity = startSize;
    vector->size = 0;
    vector->maxSize = maxSize;
    return vector;
}

void float_vector_destroy(FloatVector *vector){
    free(vector->arr);
    free(vector);
}