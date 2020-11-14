#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/clique.h"
#include <stdlib.h>
#include <string.h>

struct product* create_product(int id, const char *website) {
	struct product *p = malloc(sizeof(struct product));
	p->id = id;
	p->website = malloc(strlen(website) * sizeof(char));
	strcpy(p->website, website);
	p->next = NULL;
	return p;
}

void merge_cliques(struct clique *c1, struct clique *c2) {
	/* Update the last product */
	//printf("The first product of clique is %s %d \n",c1->first_product->website,c1->firs );
	struct product *ptr=c2->first_product;

	printf("merge\n");

	if (ptr!=NULL)
	{
		printf("BIg letters is %s %d \n",ptr->website,ptr->id );
	}
	printf("ptr is %p \n",ptr);
	
	printf("Merging\n");
	c1->last_product->next = c2->first_product;
	c1->last_product = c2->last_product;
	printf("The second product of clique is %s %d \n",c2->first_product->website,c2->first_product->id );
	/* Now both cliques refer to the same product sequence */
	c2->first_product = c1->first_product;
	printf("The first product of clique is %s %d \n",c1->first_product->website,c1->first_product->id );
	printf("The second product of clique is %s %d \n",c2->first_product->website,c2->first_product->id );
	/* Update their sizes */
	c1->size += c2->size;
	c2->size = c1->size;
<<<<<<< HEAD
	
	return;
=======

	while (ptr!=NULL)
	{
		//printf("kai edw\n");
	
		/* code */	
		ptr->clique->first_product=c1->first_product;
		ptr->clique->last_product=c2->last_product;
		ptr->clique->size=c1->size +c2->size;
		printf("ok ok %s %d \n",ptr->clique->first_product->website,ptr->clique->first_product->id );
		ptr=ptr->next;
	}



}


struct clique * create_new()
{
	struct clique *c = malloc(sizeof(struct clique));
	c->size = 0;
	c->first_product = NULL;
	c->last_product = NULL;
	return c;
}


void product_init(struct clique *ptr,int id, char *website)
{
	
	ptr->size+=1; //increase the size 1 

	ptr->first_product=malloc(sizeof(struct product));

	ptr->last_product=ptr->first_product;

	ptr->first_product->clique=ptr;

	ptr->first_product->id=id;

	ptr->first_product->website=malloc(strlen(website)+1);

	strcpy(ptr->first_product->website=malloc(strlen(website)+1),website);
	
	ptr->first_product->next_spec=NULL;

	ptr->first_product->last_spec=NULL;

	ptr->first_product->next=NULL;


}

void push_specs(struct clique *ptr,char * spec , char **value)
{
	if (ptr->first_product->last_spec==NULL)
	{
		ptr->first_product->next_spec=malloc(sizeof(struct spec_list));

		ptr->first_product->last_spec=ptr->first_product->next_spec;

		ptr->first_product->next_spec->spec.name=malloc(strlen(spec)+1);

		strcpy(ptr->first_product->next_spec->spec.name,spec);

		ptr->first_product->next_spec->spec.value=value;
	}
	else
	{
		//printf("u go here \n");
		ptr->first_product->last_spec->next=malloc(sizeof(struct spec_list));

		//printf("mexri edw 1 \n");

		//printf("mexri edw 1 \n");

		ptr->first_product->last_spec->next->spec.name=malloc(strlen(spec)+1);

		//printf("mexri edw 1 \n");
		
		strcpy(ptr->first_product->last_spec->next->spec.name,spec);

		ptr->first_product->last_spec->next->spec.value=value;

		ptr->first_product->last_spec=ptr->first_product->last_spec->next;



	}



>>>>>>> 7ab3321b00e1763cad9a69643d6877edb91681f5
}