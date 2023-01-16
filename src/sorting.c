#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include <floatvector.h>

static inline void swap(size_t i1, size_t i2, int *arr){
    int buf = arr[i1];
    arr[i1] = arr[i2];
    arr[i2] = buf;
}

static inline void swap_float(size_t i1, size_t i2, float *arr){
    float buf = arr[i1];
    arr[i1] = arr[i2];
    arr[i2] = buf;
}

void bubble_sort(int *arr, size_t n){
    for(size_t i = n; i != 0; i--)
        for(size_t j = 0; j + 1 < i; j++)
            if(arr[j] > arr[j+1])
                swap(j, j+1, arr);
}

void selection_sort(int *arr, size_t n){
    for(size_t i = 0; i < n; i++){
        size_t smallest = i;
        for(size_t j = i; j < n; j++)
            if(arr[smallest] > arr[j])
                smallest = j;
        swap(smallest, i, arr);
    }
}

void insertion_sort(int *arr, size_t n){
    for(size_t i = 1; i < n; i++)
        for(size_t j = i; j > 0; j--)
            if(arr[j-1] > arr[j])
                swap(j, j-1, arr);
}

void insertion_sort_float(float *arr, size_t n){
    for(size_t i = 1; i < n; i++)
        for(size_t j = i; j > 0; j--)
            if(arr[j-1] > arr[j])
                swap_float(j, j-1, arr);
}

static void merge(int arr[], size_t l, size_t m, size_t r){
    size_t lSize = m - l + 1;
    size_t rSize = r - m;

    int L[lSize];
    int R[rSize];
    memcpy(L, &arr[l], (m - l + 1) * sizeof(int));
    memcpy(R, &arr[m+1], (r - m) * sizeof(int));

    size_t lPtr = 0;
    size_t rPtr = 0;

    for(size_t i = 0; i < r - l + 1; i++){
        if(rPtr == rSize){
            arr[i + l] = L[lPtr];
            lPtr++;
            continue;
        }

        if(lPtr == lSize){
            arr[i + l] = R[rPtr];
            rPtr++;
            continue;            
        }

        if(L[lPtr] < R[rPtr]){
            arr[i + l] = L[lPtr];
            lPtr++;
        }
        else{
            arr[i + l] = R[rPtr];
            rPtr++;
        }
    }
}

static void __merge_sort(int arr[], size_t l, size_t r){
    if(l < r){
        size_t m = (r - l) / 2 + l;
        __merge_sort(arr, l, m);
        __merge_sort(arr, m+1, r);
        merge(arr, l, m ,r);
    } 
}

void merge_sort(int arr[], size_t n){
    __merge_sort(arr, 0, n - 1);
}

static long long sort_after_pivot(int arr[], long long l, long long r, long long pivotIndex){
    int pivot = arr[pivotIndex];
    swap(r, pivotIndex, arr);
    long long i = (l - 1);
    for(long long int j = l; j <= r - 1; j++){
        if(arr[j] < pivot){
            i++;
            swap(i, j, arr);
        }
    }
    swap(i+1, r, arr);
    return (i + 1);
}

//Uses middle element as pivot
static void __quick_sort(int arr[], long long l, long long r){
    if(l < r){
        long long m = (r - l) / 2 + l;
        long long pi = sort_after_pivot(arr, l, r, m); // Position of pivot after sorting
        __quick_sort(arr, l, pi - 1);
        __quick_sort(arr, pi + 1, r);
    }
}

void quick_sort(int arr[], size_t n){
    __quick_sort(arr, 0, n - 1);
}

//Returns true on success
bool counting_sort(int arr[], size_t n){
    if(n == 0)
        return false;
    
    //Search for biggest element
    int max = arr[0];
    int min = arr[0];
    unsigned range = max - min;
    size_t i;
    for(i = 0; i < n; i++){
        if(max < arr[i]) {
            max = arr[i];
            continue;
        }
        if(min > arr[i])
            min = arr[i];
    }

    unsigned int* countArr = calloc(range, sizeof(int));
    if(countArr == NULL)
        return false;

    //Store count of each number
    for(i = 0; i < n; i++){
        countArr[arr[i] - min]++;
    }

    for(i = 1; i < max; i++){
        countArr[i] = countArr[i-1] + countArr[i];
    }

    int* result = malloc(n * sizeof(int));
    if(result == NULL){
        free(countArr);
        return false;
    }

    //Look through arr and fill the content at their positions in result
    for(i = n - 1; i >= 0; i--){
        result[countArr[arr[i] - min] - 1] = arr[i];
        countArr[arr[i] - min]--;
    }

    memcpy(arr, result, n);
    free(countArr);
    free(result);
    return true;
}

void deallocate_buckets(FloatVector **buckets, size_t n){
    for(int i = 0; i < n; i++){
        float_vector_destroy(buckets[i]);
    }
}


//Uses float array, because other than counting sort bucket sort can use floating point values
bool bucket_sort(float arr[], size_t n){
    float max = arr[0];
    float min = arr[0];
    for(int i = 0; i < n; i++){
        if(arr[i] > max){
            max = arr[i];
            continue;
        }
        if(arr[i] < min)
            min = arr[i];
    }
    
    int bucketAmount = (int) sqrt(n);
    float range = (max - min) / bucketAmount; //The range of every bucket
    
    FloatVector **buckets = malloc(bucketAmount * sizeof(FloatVector *));
    if(buckets == NULL)
        return false; 
    
    //Create buckets
    for(int i = 0; i < bucketAmount; i++){
        buckets[i] = float_vector_init(n, bucketAmount);
        //If init fails one time all other buckets get deallocated plus the array
        if(buckets[i] == NULL){
            deallocate_buckets(buckets, i);
            free(buckets);
            return false;
        }
    }

    //Loop through arr and insert values into buckets
    for(int i = 0; i < n; i++){
        float diff = (arr[i] - min) / range - ((int) ((arr[i] - min) / range));
        if(diff == 0 && arr[i] != min)
            float_vector_add(buckets[(int) ((arr[i] - min) / range) - 1], arr[i]);
        else
            float_vector_add(buckets[(int) ((arr[i] - min) / range)], arr[i]);
    }

    //Sort and concat the buckets
    for(int i = 0; i < bucketAmount; i++){
        insertion_sort_float(buckets[i]->arr, buckets[i]->size);
        
        //Copy into arr
        for(int j = 0; j < buckets[i]->size; j++){
            arr[0] = buckets[i]->arr[j];
            arr++;
        }
    }

    deallocate_buckets(buckets, bucketAmount);
    free(buckets);
    return true;
}