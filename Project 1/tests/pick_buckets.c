#include "../include/acutest.h"
#include "../include/util.h"
#include "../include/list.h"
#include "../include/map.h"
#include "../include/clique.h"
#include "../include/dataset_parsing.h"

void pick_the_buckets1(void)
{
    char **arg_v,*array[5];

    array[0]="./run";
    array[1]="./tests/example";
    array[2]="./tests/datasetY/datasetY.csv";
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
    array[1]="./tests/example";
    array[2]="./tests/datasetY/datasetY.csv";
    array[3]="-s";
    array[4]="ok";

    arg_v=array;

    //i should get 10 as the size of the hashtable

    int size=pick_the_buckets(5,arg_v);

    TEST_ASSERT(size == -1);
}

void pick_the_buckets3(void)
{
    char **arg_v,*array[3];

    array[0]="./run";
    array[1]="./tests/example";
    array[2]="./tests/datasetY/datasetY.csv";

    arg_v=array;

    //i should get 10 as the size of the hashtable

    int size=pick_the_buckets(3,arg_v);

    TEST_ASSERT(size == 9);


}

void read_data_files1(void)
{
    struct hash_map *map = map_init(10, hash_str, NULL, NULL, free);

    char *argv="/random/path"; 

    int variable=read_data_files(map,10,argv);

    TEST_ASSERT(variable == -1); //-1 for random paths 


}

void read_data_files2(void)
{
    struct hash_map *map = map_init(10, hash_str, NULL, NULL, free);

    char *argv="./tests/read_data_x"; 

    char *key="buy.net//5411";

    unsigned int pos = map->hash(key) % map->size;

    int variable=read_data_files(map,10,argv);

    //printf("Variable is %d and position is %d \n",variable,pos);

    TEST_ASSERT(variable == 1); //1 for executing correct path

    //next test that it was hashed properly 

    int k=strcmp(map->array[pos]->key,key);

    TEST_ASSERT(k==0);


}


void passing_clique(void)
{
    struct hash_map *map = map_init(10, hash_str, NULL, NULL, free);

    char *argv="./tests/read_data_x"; 

    char *key="buy.net//5411";

    unsigned int pos = map->hash(key) % map->size;

    int variable=read_data_files(map,10,argv);

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
    struct list *list = ptr_prod->specs;
	char *str = "page title";
	char *spec = "Kodak PIXPRO AZ251 Bridge Digital Camera - 16MP 25X Optical Zoom HD720p -Black 16.15 Price Comparison at Buy.net";
    TEST_ASSERT(list_find(list,str) == 0);
	struct spec *s1 = (struct spec*)list_get(list,0);
    TEST_ASSERT(strcmp((char*)s1->value[0],spec) == 0);

    // second spec
	str = "";
	spec = "sth";
    TEST_ASSERT(list_find(list,str) == 0);
	s1 = (struct spec*)list_get(list,1);
    TEST_ASSERT(strcmp((char*)s1->value[0],spec) == 0);

    //last spec
    str = "height";
	char **specs = malloc(2*sizeof(char*));
	specs[0] = "9.7 cm";
	specs[1] = "3.8 in.";
    TEST_ASSERT(list_find(list,str) == 0);

	s1 = (struct spec*)list_get(list,2);
    TEST_ASSERT(strcmp((char*)s1->value[0],specs[0]) == 0);

	s1 = (struct spec*)list_get(list,2);
	TEST_ASSERT(strcmp((char*)s1->value[1],specs[1]) == 0);
	free(specs);
}

void read_relations_1(void)
{
    char *x="./tests/example";

    char *y="./tests/datasetY/datasetY.csv";

    struct hash_map *map = map_init(10, hash_str, NULL, NULL, free);

    int variable=read_data_files(map,10,x);

    read_relations(map,y);

    print_results(map);


}


TEST_LIST = {
   { "example1", pick_the_buckets1 },
   { "example2", pick_the_buckets2 },
   { "example3", pick_the_buckets3 },
   { "example4", read_data_files1 },
   { "example5", read_data_files2 },
   { "example6", passing_clique },
   { "example7", read_relations_1 },
   { NULL, NULL }     /* zeroed record marking the end of the list */
};