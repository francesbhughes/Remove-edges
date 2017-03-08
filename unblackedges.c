/*
 * Names: Frances Hughes 
 * Date: 2/10/2017
 *
 * unblackedges.c
 */

#include "pnmrdr.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "bit2.h"

/*########################FUNCTION DECLARATIONS#########################*/
Bit2_T get_image(int argc, char *argv[], int *width, int *height);
Bit2_T check_format(FILE *fp, int *width, int *height);
Bit2_T get_bit_array(Pnmrdr_T rdr, int width, int heigth);
void remove_black_edges(int width, int height, Bit2_T array, int bit, void *p1);
void remove_black_bit(int width, int height, int last_width, int last_height,
                Bit2_T array);   
bool validate_index(int width, int height, int last_width, int last_height,
                Bit2_T array);
void print_array(int i, int j, Bit2_T a, int b, void *p1);
void print_file(Bit2_T array, int width, int height);

int main(int argc, char *argv[])
{
        int width, height;
        Bit2_T array = get_image(argc, argv, &width, &height);

        Bit2_map_row_major(array, remove_black_edges, NULL);
        print_file(array, width, height);

        Bit2_free(&array);
        return EXIT_SUCCESS;
}

/*
 * Opens up file given as argument or reads from standard input, 
 * and converts file to a Bit2_T
 */
Bit2_T get_image(int argc, char *argv[], int *width, int *height)
{
        Bit2_T array;
        if (argc == 1) {
                if (!feof(stdin)){
                        array = check_format(stdin, width, height);
                }                       
        } else if (argc > 2){ 
                fprintf( stderr, "too many arguments\n");
                exit(EXIT_FAILURE);     
        } else {
                FILE *fp = fopen(argv[1], "rb");
                if (fp == NULL) {
                        fprintf(stderr, "unable to open file\n");
                        exit(EXIT_FAILURE);
                }
                array = check_format(fp, width, height);
                fclose(fp);
        }
        return array; 
}

/*
 * Checks file to confirm it is a portable bitmap
 */
Bit2_T check_format(FILE *fp, int *width, int *height)
{
        Pnmrdr_T rdr = Pnmrdr_new(fp);

        Pnmrdr_mapdata *map =  malloc(sizeof(Pnmrdr_mapdata));
        *map = Pnmrdr_data(rdr);

        *width = map->width;
        *height = map->height;

        if (map->type != 1){
                RAISE(Pnmrdr_Badformat);
        }

        Bit2_T array = get_bit_array(rdr, map->width, map->height);

        free(map);
        Pnmrdr_free(&rdr);
        return array;
}

/*
 * Creates a Bit2_T of the specified width and height with the 
 * Pnmrdr_T data
 */ 
Bit2_T get_bit_array(Pnmrdr_T rdr, int width, int height) 
{
        Bit2_T array = Bit2_new(width, height); 
        int current_h;
        int current_w;

        (void)rdr;

        for (current_h = 0; current_h < height; current_h++) {
                for (current_w = 0; current_w < width; current_w++) {
                        Bit2_put(array, current_w, current_h, Pnmrdr_get(rdr));
                }
        }
        return array;
}

/*
 * Checks if the edges of the portable bitmap are black, and calls function
 * to remove all of the related black bits from the image
 */
void remove_black_edges(int width, int height, Bit2_T array, int bit, void *p1) 
{
        (void) p1;
        if ((width == 0 || width == (Bit2_width(array) - 1) 
             || height == 0 || height == (Bit2_height(array) - 1))
             && bit == 1){

             remove_black_bit(width, height, width, height, array);   
        }
}

/*
 * Recursively removes all of the black bits from the image if they are 
 * connected to an edge 
 */
void remove_black_bit(int width, int height, int last_width, int last_height,
                Bit2_T array){
        if (Bit2_get(array, width, height) == 0){        
                return;
        }
        Bit2_put(array, width, height, 0);
   
        /* Goes left, right, down, up */     
        if (validate_index(width - 1, height, last_width, last_height, array)){
                remove_black_bit(width - 1, height, width, height, array);
            
        }
        if (validate_index(width + 1, height, last_width, last_height, 
                                array)){
                remove_black_bit(width + 1, height, width, height, array);
        
        }
        if (validate_index(width, height - 1, last_width, last_height,
                                array)){
                remove_black_bit(width, height - 1, width, height, array);
        
        }
        if (validate_index(width, height + 1, last_width, last_height, array)){
                remove_black_bit(width, height + 1, width, height, array);
        } 
}

/*
 * Checks that the width and height are within the bounds
 * Checks that the index passed in is not the previous index
 */
bool validate_index(int width, int height, int last_width, int last_height,
                Bit2_T array){
        if (width < 1 || width > (Bit2_width(array) - 2) ||
            height < 1 || height > (Bit2_height(array) - 2)){
               return false;
        } 
        if (width == last_width && height == last_height){
                return false;
        }
        return true;
}

/*
 * Prints out the contents of the Bit2_T array
 */
void print_array(int i, int j, Bit2_T a, int b, void *p1) 
{
        (void) b;
        (void)p1;
        int bit = Bit2_get(a, i, j);
        printf("%d", bit);
        if (i == (Bit2_width(a)-1)){
                printf("\n");
        }
}

/*
 * Prints out formatting for Bit2_T array and then calls print_array
 */
void print_file(Bit2_T array, int width, int height)
{
        printf("P1\n%d %d\n", width, height);
        Bit2_map_row_major(array, print_array, NULL);
}
