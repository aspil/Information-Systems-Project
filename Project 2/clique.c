#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include  "../include/list.h"
#include "../include/clique.h"

int counteraki=0;

void merge_cliques(struct clique **clique_1, struct clique **clique_2) 
{

	//printf("gamw to xristo mou\n");
	struct product *ptr_2 = (*clique_2)->first_product;

	if ((*clique_1)->first_product == (*clique_2)->first_product)
		return;

	(*clique_1)->last_product->next = (*clique_2)->first_product;
	
	/* Update their sizes */
	(*clique_1)->size += (*clique_2)->size;

	/* Update the last product pointer of the first clique */
	(*clique_1)->last_product = (*clique_2)->last_product;

	if (((*clique_2)->first_negative!=NULL) && ((*clique_1)->first_negative==NULL)) 
	{
		(*clique_1)->first_negative=(*clique_2)->first_negative;
		(*clique_1)->last_negative=(*clique_2)->last_negative;

		//you need to change the negative relations that show clique 2 cause it will get freed 

		struct negative_relation *tranverse_1=(*clique_2)->first_negative,*tranverse_2;


		while (tranverse_1!=NULL)
		{
			tranverse_2=tranverse_1->neg_rel->first_negative;

			while (tranverse_2!=NULL)
			{
				if (tranverse_2->neg_rel==(*clique_2))
				{
					tranverse_2->neg_rel=(*clique_1);
					break;
				}
				tranverse_2=tranverse_2->next;
			}

			tranverse_1=tranverse_1->next;

		}
	}
	else if (((*clique_2)->first_negative!=NULL) && ((*clique_1)->first_negative!=NULL)) 
	{
		
		struct negative_relation *ngt=(*clique_2)->first_negative,*ngt_2,*found=NULL,*prev_cli,*one_more=NULL;
		int count_clique_1=0,count_clique_2=0;
		//printf("%s %d \n",(*clique_2)->first_product->website,(*clique_2)->first_product->id );
		//printf("%s %d \n",(*clique_1)->first_product->website,(*clique_1)->first_product->id );
		//printf("end\n");
		while (ngt!=NULL)
		{
			printf("Prwto proion gia epibebaiwsh %s %d \n",ngt->neg_rel->first_product->website,ngt->neg_rel->first_product->id );
			ngt_2=ngt->neg_rel->first_negative;

			prev_cli=ngt_2;

			while (ngt_2!=NULL)
			{
				printf("Deutero proion gia epibebaiwsh %s %d \n",ngt_2->neg_rel->first_product->website,ngt_2->neg_rel->first_product->id );
				if (ngt_2->neg_rel==(*clique_2))
				{
					found=ngt_2;
					one_more=prev_cli;
					count_clique_2=1;
					
				}
				if (ngt_2->neg_rel==(*clique_1))
				{
					printf("MIkro mhnuma : %s %d \n",ngt_2->neg_rel->first_product->website,ngt_2->neg_rel->first_product->id );
					count_clique_1=1;
				}
				prev_cli=ngt_2;
				ngt_2=ngt_2->next;
			}

			if (count_clique_2==1)
			{
				if (count_clique_1==1)
				{
					if (found==one_more)
					{
						ngt->neg_rel->first_negative=found->next;
						free(found);
					}
					else if (found==prev_cli)
					{
						ngt->neg_rel->last_negative=one_more;
						one_more->next=found->next;
						free(found);
					}
					else
					{
					one_more->next=found->next;
					printf("EDw ton diagrafw %p \n", found);
					printf("%s %d \n",found->neg_rel->first_product->website,found->neg_rel->first_product->id );
					free(found);
					}
				}
				else
				{
					found->neg_rel=(*clique_1);
				}
			}

			ngt=ngt->next;
			count_clique_1=0,count_clique_2=0;

		}
		//u should concatenate the 2 lists of negative relations and delete any double adress

		printf("1  %p \n",(*clique_2)->first_negative );
		printf("2 %p \n",(*clique_1)->last_negative );
		printf("%p \n",(*clique_1)->last_negative->next );
		(*clique_1)->last_negative->next=(*clique_2)->first_negative;

		(*clique_1)->last_negative=(*clique_2)->last_negative;

		struct negative_relation * tranverse_1=(*clique_1)->first_negative,*tranverse_2,*prev;

		while (tranverse_1!=NULL)
		{
			if (tranverse_1->next!=NULL)
			{
				tranverse_2=tranverse_1->next;
				prev=tranverse_1;

			}
			else
				break;
			while(tranverse_2!=NULL)
			{
				if (tranverse_2->neg_rel==tranverse_1->neg_rel)
				{
					prev->next=tranverse_2->next;
					printf("8a diagrafei %p \n",tranverse_2 );
					if (tranverse_2->next==NULL)
					{
					 	//printf("null vlue\n");
						(*clique_1)->last_negative=prev;
					}
					free(tranverse_2);
					break;
				}
				prev=tranverse_2;
				tranverse_2=tranverse_2->next;

			}

			tranverse_1=tranverse_1->next;
		}
		//the final result is a list with no doubles
		//change the addresses on the other cliques that pointed to the second one

	}

	printf("First product of clique is %s %d \n",(*clique_2)->first_product->website,(*clique_2)->first_product->id );
	printf("AKomh ena %p \n",*clique_2 );
	free(*clique_2);	/* Call free instead of delete_clique because we want to keep the products */
	free(clique_2);
	while (ptr_2 != NULL) {
		*(ptr_2->clique) = clique_1;
		ptr_2 = ptr_2->next;
	}
	return;

}


void negative_relation_func(struct clique **clique_1, struct clique **clique_2)
{

	//pass the clique 2 to clique 1 list if doesnt exist

	//printf("edw\n");
	if ((*clique_1)->last_negative==NULL)
	{
		//first node of negative relation
		//printf("once\n");
		(*clique_1)->first_negative=malloc(sizeof(struct negative_relation));
 		(*clique_1)->last_negative=(*clique_1)->first_negative;
		(*clique_1)->last_negative->neg_rel=(*clique_2);
		(*clique_1)->last_negative->next=NULL;
	}
	else
	{ //there is already a negative relation 
		//check the negative relation if its already there for another product 
		//if its not add it
		//printf("twice\n");
		struct negative_relation *tranverse=(*clique_1)->first_negative,*prev;
		printf("EDw ena mhnuma %p \n",tranverse );
		//printf("%p \n", tranverse->neg_rel);
		//printf("%s %d \n",tranverse->neg_rel->first_product->website,tranverse->neg_rel->first_product->id );
		while (tranverse!=NULL)
		{
			if (tranverse->neg_rel==(*clique_2))
			{
				break;
			}
			prev=tranverse;
			tranverse=tranverse->next;
		}

		if (tranverse==NULL)
		{
			//printf("logika kai edw\n");
			//printf("%p \n", prev->next);
			prev->next=malloc(sizeof(struct negative_relation));
			prev->next->neg_rel=(*clique_2);
			prev->next->next=NULL;
			(*clique_1)->last_negative=prev->next;

		}

	}
	if ((*clique_2)->last_negative==NULL)
	{
		//first node of negative relation
		//printf("kai mallon edw\n");
		(*clique_2)->first_negative=malloc(sizeof(struct negative_relation));
		(*clique_2)->last_negative=(*clique_2)->first_negative;
		(*clique_2)->last_negative->neg_rel=(*clique_1);
		(*clique_2)->last_negative->next=NULL;
		if ((strcmp((*clique_2)->first_product->website,"www.mypriceindia.com")==0) && (strcmp((*clique_1)->first_product->website,"www.gosale.com")==0))
		{
			printf("one messange I nened too ses\n");
			printf("%p \n",(*clique_2)->last_negative->neg_rel );
			printf("%p \n",(*clique_2));

		}
	}
	else
	{ //there is already a negative relation 
		//check the negative relation if its already there for another product 
		//if its not add it
		struct negative_relation *tranverse=(*clique_2)->first_negative,*prev;

		while (tranverse!=NULL)
		{
			if (tranverse->neg_rel==(*clique_1))
			{
				break;
			}
			prev=tranverse;
			tranverse=tranverse->next;
		}

		if (tranverse==NULL)
		{
			prev->next=malloc(sizeof(struct negative_relation));
			prev->next->neg_rel=(*clique_1);
			prev->next->next=NULL;
			(*clique_2)->last_negative=prev->next;
			
		}

	}

}



struct clique** create_clique()
{
	struct clique **ret = malloc(sizeof(struct clique*));
	*ret = malloc(sizeof(struct clique));
	(*ret)->size = 0;
	(*ret)->first_product = NULL;
	(*ret)->last_product = NULL;
	(*ret)->first_negative = NULL;
	(*ret)->last_negative = NULL;
	return ret;
}

void delete_clique(void *ptr) {
	struct clique **clique = (struct clique **) ptr;
	int i=1;
	

	//printf("Clique pointer is %p \n",clique );
	if (clique!=NULL)
	{
		struct product *temp = (*clique)->first_product, *next = NULL,*first_prod=(*clique)->first_product;

		struct clique ***triple_ptr=first_prod->clique;

		struct negative_relation *delete_ngtv=(*clique)->first_negative,*rest;

		while (delete_ngtv!=NULL)
		{
			//printf("how many times\n");
			rest=delete_ngtv->next;
			free(delete_ngtv);
			delete_ngtv=rest;
		}
		//printf("The second pointers is %p \n",*triple_ptr );

		while (temp != NULL) {
			next = (temp)->next;
			//printf("%d\n",i );
			product_delete(temp,i);
			temp = next;
			i++;
		}
		if (i>2)
		{
			printf("I is %d \n",i );
			printf("Clique pointer is %p \n",clique );
			printf("%p \n", **triple_ptr);
		}
		

		free(**triple_ptr);
		free(*triple_ptr);
		*triple_ptr=NULL;
	}
	else
	{
		counteraki++;
	}

	return;
}

struct spec* spec_init(char *spec_name, struct vector *vec) {
	struct spec *spec = malloc(sizeof(struct spec));
	spec->name = malloc(strlen(spec_name)+1);
	strcpy(spec->name, spec_name);

	spec->cnt = vector_size(vec);
	spec->value = malloc(vector_size(vec) * sizeof(struct spec*));

	for (int i = 0; i < spec->cnt; i++) {
		char *spec_val = (char*) vector_get(vec, i);
		spec->value[i] = malloc(strlen(spec_val) + 1);
		strcpy(spec->value[i], spec_val);
	}
	return spec;
}

void spec_delete(void *ptr) {
	struct spec *spec = (struct spec *) ptr;
	for (int i = 0; i < spec->cnt; i++)
		free(spec->value[i]);

	free(spec->value);
	free(spec->name);
	free(spec);
}

struct product* product_init(int id, char *website, struct clique **clique)
{
	struct product *p = calloc(1,sizeof(struct product));
	p->id = id;
	p->website = malloc(strlen(website)+1);
	p->clique = NULL;
	strcpy(p->website, website);
	p->specs = list_init(compare_str, spec_delete);
	p->next = NULL;
	return p;
}

void product_delete(struct product *p,int counter) {
	
	if (counter!=1)
	{	
		free(p->website);
		list_delete(p->specs);
		*(p->clique)=NULL;
		free(p);
		p = NULL;
	}
	else
	{
		//printf("edw\n");
		free(p->website);
		list_delete(p->specs);
		free(p);
		//printf("telos\n");
		p = NULL;
	}
}

void push_specs(struct clique *clique, char *spec_name, struct vector *vec)
{
	struct spec *spec = spec_init(spec_name, vec);

	list_append(clique->first_product->specs, spec);
	return;
}