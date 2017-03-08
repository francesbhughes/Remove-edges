/*
 * Authors: Frances Hughes
 * Date Created: 2/5/2017
 */
#include "bit2.h"
#include "bit.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>

#define T Bit2_T

struct T {
        int width, height;
        Bit_T bytes; /* elements can only be 0 or 1 */
};

/*###########################FUNCTION DECLARATIONS####################*/
void Bit2_init(T array, int width, int height, void *bytes);
int Bit2_get_index(T array, int width, int height);

/* 
 * Creates a new Bit2_T with specified width and height 
 */ 
T Bit2_new(int width, int height) 
{
        T array = malloc(sizeof(struct T));

        array->bytes = Bit_new(width*height);
        array->width = width;
        array->height = height;

        return array;
}

/* 
 * Returns the width of the Bit2_T 
 */
int Bit2_width(T array) 
{
        assert(array != NULL);
        return array->width;
}

/* 
 * Returns the height of the Bit2_T 
 */
int Bit2_height(T array)
{
        assert(array != NULL);
        return array->height;
}

/* 
 * Places the specified bit into the Bit2_T are the gien width and height 
 */
int Bit2_put(T array, int width, int height, int bit) 
{
         
        int index = Bit2_get_index(array, width, height);
        return  Bit_put(array->bytes, index, bit);
}

/*
 * Returns the index in the Bit2_T with the specified width and height
 */
int Bit2_get_index(T array, int width, int height)
{
        assert(width >= 0 && width < Bit2_width(array));
        assert(height >= 0 && height < Bit2_height(array));
        return (height * Bit2_width(array)) + width;
}

/*
 * Returns the bit in the Bit2_T at the specified width and height
 */
int Bit2_get(T array, int width, int height)
{
        int index = Bit2_get_index(array, width, height);
        return Bit_get(array->bytes, index);
}

/*
 * Calls the function pointed to by apply for every element in column major
 * order
 */
void Bit2_map_col_major(T array,
                void apply(int width, int height, T array, int bit, void *p1),
                void *cl)
{
        for (int i = 0; i < Bit2_width(array); i++) {
                for (int j = 0; j < Bit2_height(array); j++) {
                        apply(i, j, array, Bit2_get(array, i, j), cl);
                }
        }
}

/*
 * Calls the function pointed to by apply for every element in row major order
 */
void Bit2_map_row_major(T array,
                void apply(int width, int height, T array, int bit, void *p1),
                void *cl)
{
        for (int i = 0; i < Bit2_height(array); i++) {
                for (int j = 0; j < Bit2_width(array); j++) {
                        apply(j, i, array, Bit2_get(array, j, i), cl);
                }
        }
}

/*
 * Frees every element in the Bit2_T and then frees Bit2_T
 */
void Bit2_free(T *array)
{        
        assert(array != NULL && *array != NULL);
        Bit_free(&((*array)->bytes));
        free(*array);
}
#undef T
