// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <array.h>

// void initArray(Array *a, int initialSize,char *temp) {
//   //printf("%d \n",initialSize );
//   a->array = malloc(initialSize * sizeof(char*));
//   //printf("ftase edw \n");
//   a->array[0]=temp;
//   a->used = 0;
//   a->size = initialSize;
// }

// void insertArray(Array *a, char *temp) 
// {

//     a->size += 1;
//     a->array = realloc(a->array, a->size * sizeof(char*));
//     a->used+=1;
//     a->array[a->used] = temp;
// }

// void initArray_2(Array *a, int initialSize,struct clique *temp) {
//   //printf("%d \n",initialSize );
//   a->array = malloc(initialSize * sizeof(struct clique*));
//   //printf("ftase edw \n");
//   //printf("POinter is %p\n",temp );
//   a->array[0]=temp;
//   a->used = 0;
//   a->size = initialSize;
// }

// void insertArray_2(Array *a, struct clique *temp) 
// {
// 	//printf("%p \n",temp );
// 	//printf("Temp is %s \n",temp->first_product->website );
//     a->size += 1;
//     a->array = realloc(a->array, a->size * sizeof(struct clique*));
//   	//printf("Ari8mos %d \n",a->used++ );
//   	a->used+=1;
//   	a->array[(a->used)] = temp;

// }