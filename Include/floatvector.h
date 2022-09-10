#ifndef FLOATVECTOR_H
#define FLOATVECTOR_H

#include <stdint.h>
/*
 * NEVER modify values other then the content of the array.
 */
typedef struct 
{
   float *arr;
   int32_t capacity;
   int32_t size;
   int32_t maxSize;
} FloatVector;
/*
 * Shrinks arr by the amount, if amount is bigger then empty space the method will shrink as much as he can.
 * Pass -1 to shrink the maximum possible.
 * Returns by how much elements the array got shrinked.a
 */
int float_vector_shrink(FloatVector*, int32_t);

/*
 * Adds a value to the vector and returns a pointer to that value.
 * Return NULL if value can't be added.
 */
float* float_vector_add(FloatVector*, float);

/*
 * Deletes amount elements from the end of the vector.
 * Return amount of deleted elements.
 */
int float_vector_remove(FloatVector*, int32_t);
FloatVector* float_vector_init(int32_t, int32_t);
void float_vector_destroy(FloatVector*);
#endif