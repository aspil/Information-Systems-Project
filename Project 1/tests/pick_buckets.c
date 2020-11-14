#include "../include/acutest.h"
#include "../include/util.h"
#include "../include/map.h"
#include "../include/clique.h"

void pick_the_buckets1(void)
{
    char **arg_v,*array[5];

    array[0]="./run";
    array[1]="./files";
    array[2]="./files/data.csv";
    array[3]="-s";
    array[4]="10";

    arg_v=array;

    //i should get 10 as the size of the hashtable

    int size=pick_the_buckets(5,arg_v);

    TEST_ASSERT(size == 10);


}
void pick_the_buckets2(void)
{
    char **arg_v,*array[5];

    array[0]="./run";
    array[1]="/home/fanmfh/Desktop/Project_1/camera_specs/example";
    //array[1]="~/Desktop/Project_1/camera_specs/example/";
    array[2]="./files/data.csv";
    array[3]="-s";
    array[4]="ok";

    arg_v=array;

    //i should get 10 as the size of the hashtable

    int size=pick_the_buckets(5,arg_v);

    TEST_ASSERT(size == 9);


}

void pick_the_buckets3(void)
{
    char **arg_v,*array[3];

    array[0]="./run";
    array[1]="/home/fanmfh/Desktop/Project_1/camera_specs/example";
    //array[1]="~/Desktop/Project_1/camera_specs/example/";
    array[2]="./files/data.csv";

    arg_v=array;

    //i should get 10 as the size of the hashtable

    int size=pick_the_buckets(3,arg_v);

    TEST_ASSERT(size == 9);


}

void read_data_X1(void)
{
    struct hash_map *map = map_init(10, hash_str, NULL, NULL, free);

    char *argv="/random/path"; 

    int variable=read_data_X(map,10,argv);

    TEST_ASSERT(variable == -1); //-1 for random paths 


}

void read_data_X2(void)
{
    struct hash_map *map = map_init(10, hash_str, NULL, NULL, free);

    char *argv="/home/fanmfh/Desktop/examples/Project_1/tests/read_data_x"; 

    char *key="buy.net//5411";

    unsigned int pos = map->hash(key) % map->size;

    int variable=read_data_X(map,10,argv);

    //printf("Variable is %d and position is %d \n",variable,pos);

    TEST_ASSERT(variable == 1); //1 for executing correct path

    //next test that it was hashed properly 

    int k=strcmp(map->array[pos]->key,key);

    TEST_ASSERT(k==0);


}


void passing_clique(void)
{
    struct hash_map *map = map_init(10, hash_str, NULL, NULL, free);

    char *argv="/home/fanmfh/Desktop/examples/Project_1/tests/read_data_x"; 

    char *key="buy.net//5411";

    unsigned int pos = map->hash(key) % map->size;

    int variable=read_data_X(map,10,argv);

    TEST_ASSERT(variable == 1); //1 for executing correct path

    //next test that it was hashed properly 

    int k=strcmp(map->array[pos]->key,key);

    TEST_ASSERT(k==0);

    //i need to see that the right specs were created 

    struct clique *ptr= (struct clique *) map->array[pos]->value;

    TEST_ASSERT(ptr->size==1);

    struct product *ptr_prod=ptr->first_product;

    TEST_ASSERT(ptr_prod->id==5411);

    TEST_ASSERT(strcmp(ptr_prod->website,"buy.net")==0);

    //first spec

    struct spec_list *list=ptr_prod->next_spec;

    TEST_ASSERT(strcmp(list->spec.name,"page title")==0);
    TEST_ASSERT(strcmp(list->spec.value[0],"Kodak PIXPRO AZ251 Bridge Digital Camera - 16MP 25X Optical Zoom HD720p -Black 16.15 Price Comparison at Buy.net")==0);

    //second spec
    list=list->next;
    TEST_ASSERT(strcmp(list->spec.name,"")==0);
    TEST_ASSERT(strcmp(list->spec.value[0],"sth")==0);

    //last spec
    list=ptr_prod->last_spec;
    
    TEST_ASSERT(strcmp(list->spec.name,"height")==0);
    TEST_ASSERT(strcmp(list->spec.value[0],"9.7 cm")==0);
    TEST_ASSERT(strcmp(list->spec.value[1],"3.8 in.")==0);


}

void dataset_y_1(void)
{
    char *x="./tests/example";

    char *y="./tests/datasetY/datasetY.csv";

    struct hash_map *map = map_init(10, hash_str, NULL, NULL, free);

    int variable=read_data_X(map,10,x);

    dataset_y(map,y);

    print_results(map);


}


TEST_LIST = {
   { "example1", pick_the_buckets1 },
   { "example2", pick_the_buckets2 },
   { "example3", pick_the_buckets3 },
   { "example4", read_data_X1 },
   { "example5", read_data_X2 },
   { "example6", passing_clique },
   { "example7", dataset_y_1 },
   { NULL, NULL }     /* zeroed record marking the end of the list */
};