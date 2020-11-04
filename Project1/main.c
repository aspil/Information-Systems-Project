#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include "node.h"

int for_me=0;

int count_files(char*path);

void hash_table_creation(hashtable **ptr,int size);

void read_data_X(hashtable *ptr,int size,char *path);

void hash_the_node(hashtable *ptr,unsigned long hash_value,char *path,char *id);

void pass_spec_values_to_node(char*,char*,spec**,spec**);

void initialization_of_node(Node *ptr);

int main(int argc, char *argv[])
{
	//sthn ergasia 8a xrhsimopoih8ei hash table
	//mia kalh proseggish 8a einai o load factor na einai 0.75 
	//epomenws prepei na upologisw arxika ton ari8mo twn arxeiwn pou 8a diabasw
	int files=count_files(argv[1]);

	int number_of_buckets=files/1;

	printf("The number of files is %d and the number_of_buckets needed is %d \n", files,number_of_buckets);

	hashtable *hash_table=NULL;

	printf("The value of pointer is %p \n",hash_table );

	hash_table_creation(&hash_table,number_of_buckets);

	printf("The value of pointer vol.2 is %p \n",hash_table );

	read_data_X(hash_table,number_of_buckets,argv[1]);

	printf("%d \n",for_me );


}

int count_files(char*path)
{
	struct dirent *pDirent,*iDirent;
	int counter=0;
    DIR *pDir,*iDir;

    pDir = opendir (path);
    if (pDir == NULL) 
    {
        printf ("Cannot open directory '%s'\n", path);
        return 1;
    }

        // Process each entry.

    while ((pDirent = readdir(pDir)) != NULL) 
    {
        if (pDirent->d_name[0] != '.') {
        	puts(pDirent->d_name);
    	}

    	char *subdir;
    	subdir=malloc (sizeof(path)+ 1 + sizeof(pDirent->d_name));
    	strcat(subdir,path);
    	strcat(subdir,"/");
    	strcat(subdir,pDirent->d_name);    	
    	iDir = opendir (subdir);

    	while ((iDirent = readdir(iDir)) != NULL) 
    	{
    		//printf("Name is %s\n",iDirent->d_name );
    		if (iDirent->d_type==DT_REG)
    		{
    			counter++;
    		}
    	}

    	closedir(iDir);
    	free(subdir);

    }

        // Close directory and exit.

    closedir (pDir);

    printf("The total files are %d \n", counter);

    return counter;

}


void hash_table_creation(hashtable **ptr,int size)
{
	*ptr=malloc(sizeof(hashtable)*size);

	hashtable *helping=*ptr;

	for (int i = 0; i < size; ++i)
	{
		helping->next=NULL;
		helping->last=NULL;
		helping=helping + 1;
		//printf("%p %d \n",*ptr,i );

	}

}

unsigned long hash_bernstein(char *string,int size)
{	
	//printf("The string to be hashed is %s \n",string );

  	unsigned long hash = 5381;

	char *hash_string=malloc(strlen(string)+1);

	int c;

    strcpy(hash_string,string);

    char *help=hash_string;

	while (c = *hash_string++)
	{
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}

    hash=hash%size;

    free(help);

    return hash;
}

void read_data_X(hashtable *ptr,int size,char *path)
{
	struct dirent *pDirent,*iDirent;
	char *subdir,*value=NULL;
	int counter=0;
    DIR *pDir,*iDir;

    pDir = opendir (path);
    if (pDir == NULL) 
    {
        printf ("Cannot open directory '%s'\n", path);
        //return 1;
    }

        // Process each entry.

    while ((pDirent = readdir(pDir)) != NULL) 
    {
        printf("teleutaia epanalhpsh\n");
        if (pDirent->d_name[0] != '.') 
        {
        	//puts(pDirent->d_name);    
    	printf("kai edw 1\n");
    	printf("kai edw 2\n");
    	subdir=malloc (sizeof(path)+ 1 + sizeof(pDirent->d_name));
    	printf("kai edw 3\n");
    	printf("%s\n",subdir );
  		printf("%s \n",path );
    	strcpy(subdir,path);
    	strcat(subdir,"/");
    	strcat(subdir,pDirent->d_name);
    	printf("%s \n",subdir );    	
    	iDir = opendir (subdir);

    	while ((iDirent = readdir(iDir)) != NULL) 
    	{
    		//printf("Name is %s\n",iDirent->d_name );
    		printf("allh mia epanalhpsh\n");
    		if (iDirent->d_name[0] != '.') 
        	{
        		const char s[2] = ".";
        		char *path_help;
        		path_help=malloc(sizeof(iDirent->d_name)+1);
        		strcpy(path_help,iDirent->d_name);
        		printf("Path help is %s\n",path_help );
   				char *token;
   				token = strtok(path_help, s);
   				//printf("Token is %s in  %s \n",token,subdir );
   				unsigned long result_of_hash=hash_bernstein(token,size);
   				//printf("Token is %s \n",token );
   				printf("Give me a message\n");
   				char *json_path;
		    	json_path=malloc (sizeof(subdir)+ 1 + sizeof(iDirent->d_name));
		    	//printf("No way is %s\n", no_way);
		    	strcpy(json_path,subdir);
		    	strcat(json_path,"/");
		    	strcat(json_path,iDirent->d_name); 

   				hash_the_node(ptr,result_of_hash,json_path,token);
   				free(json_path);
   				printf("Path help is %s\n",path_help );
   				//free(path_help);
   				//free(token);
   				free(path_help);
   				printf("koitaxe kai edw\n");
   				//na koitaxw ta free gia leaks tou path help
   				//printf("Token is %s in  %s and the hash_result is %ld \n",token,subdir, result_of_hash);

        	}	
    	}

    	printf("bges apo thn epanalhpsh\n");
    	printf("The subdir is %s \n",subdir );
    	closedir(iDir);
    	printf("TO subdir einai %s\n",subdir );
    	memset(subdir, '\0' , strlen(subdir));
    	printf("TO subdir einai %s\n",subdir );
    	free(subdir);
    	subdir=value;
    	//printf("TO subdir einai 2 %s\n",subdir );
    	//printf("peras kai edw\n");
   		 }
    }

        // Close directory and exit.

    printf("kai edw pera ga\n");
    printf("To teliko apotelesma einai %d \n",for_me );
    closedir (pDir);



}

void hash_the_node(hashtable *ptr,unsigned long hash_value,char *path,char *id)
{
	printf("THe path is : %s\n",path );
	int flag=0;
	char *temp,*temp2;
	printf("The value of pointer vol.2 is %p \n",ptr );
	ptr=ptr+hash_value;
	if (ptr->next==NULL) //prwth periptwsh pou bazei kapoio node
	{
		ptr->next=malloc(sizeof(Node));
		initialization_of_node(ptr->next);
		//wra na anoixeis to arxeio gia na peraseis ta merh toy node

	    FILE *fp;
	    char * line = NULL;
	    size_t len = 0;
	    ssize_t read;
	    char *str;
	    fp = fopen (path,"r");
	    if (fp == NULL) {
	        printf ("File not created okay, errno = %d\n", errno);

	    }
	    while ((read = getline(&line, &len, fp)) != -1) 
	    {
	        //printf("Retrieved line of length %zu:\n", read);
	        //printf("%s", line);
	        //edw kaloumai na doulepsw me thn grammh pou pairnw ka8e fora
	        str=line;
	        if (line[0]!='{' && line[0]!='}') //agnohse tis agkyles sto arxeio
	        {
				int i, x; //kwdikas gia na agnohsw ta whitespaces
  				for(i=x=0; str[i]; ++i)
    				if(!isspace(str[i]) || (i > 0 && !isspace(str[i-1])))
     				 str[x++] = str[i];
  				str[x] = '\0';
				//printf("%s", str);


				//twra 8a akolou8hsw to format 
				if (str[0]=='"')
				{
					//printf("edw\n");
					str=str+1;
					if (str[0]=='<')
					{
						//printf("edw 2\n");
						str=str+1;
						//printf("%s\n",str );
					}
					//printf("Str is: %s\n",str );
					//exw xefortw8ei ta arxika 

					char *helping_str=str;

					while (helping_str[0]!='"')
					{
						if (helping_str[0]=='>')
						{
							helping_str[0]='\0';
							flag=1;
							break;
						}
						else if (helping_str[0]=='\\')
						{
							if (helping_str[1]=='"')
							{
								helping_str++; //perna to san na mhn einai telos 
							}
						}
						helping_str++;
					}
					helping_str[0]='\0';

					//printf("Now the result is %s\n",str );
					temp=malloc(strlen(str)+1);
					strcpy(temp,str);
					printf("Temp is %s \n",temp);
					//sto helping str exw to spec tou proiontos 
					//twra 8a parw to value tou
					//printf("The string is %s and its lenth is %ld\n",helping_str,strlen(helping_str) );

					if (flag==1)
					{
						str=str + strlen(str) + 4;
					}
					else
					{
						str=str+strlen(str) +3;
					}

					//printf("The new thing u will see is %s \n", str);
					flag=0;
					

					if (str[0]=='"')
					{
						str=str+1;	
						
						helping_str=str;

						while (helping_str[0]!='"')
						{
							if (helping_str[0]=='>')
							{
								helping_str[0]='\0';
								flag=1;
								break;
							}
							else if (helping_str[0]=='\\')
							{
								if (helping_str[1]=='"')
								{
									helping_str++; //perna to san na mhn einai telos 
								}
							}
							//printf("%c",helping_str[0] );
							helping_str++;
						}
						helping_str[0]='\0';
						temp2=malloc(strlen(str)+1);
						strcpy(temp2,str);
						printf("The value of the item is %s \n",temp2 );
						printf("\n");
					
						printf("o ari8mos emfanizetai ws %s \n",id);
						//exw brei kai tis 2 times epomenws 8a perasw ta strings sto node 
						printf("the integer is %d \n",ptr->next->id);
						//printf("mhnuma\n");
						pass_spec_values_to_node(temp,temp2,&(ptr->next->last),&(ptr->next->rest_of_specs));
						//pass_values_to_node(temp,temp2,)
					}

				}

    		}

    	}
	    //fprintf (fp, "Hello, there.\n"); // if you want something in the file.
	    fclose (fp);

	}

	printf("To teleiwses kai auto \n");
}


void pass_spec_values_to_node(char *spect,char* value,spec** ref,spec** init)
{


	if ((*ref)==NULL)
	{
		printf("the initialization_of_node was ok\n");
		(*ref)=malloc(sizeof(spec));
		*init=(*ref);
		printf("u finished it\n");

	}
	else
	{
		(*ref)->next=malloc(sizeof(spec));
		*ref=(*ref)->next;
	}
}


void initialization_of_node(Node *ptr)
{
	ptr->id=10;
	ptr->name_of_site=NULL;
	ptr->rest_of_specs=NULL;
	ptr->last=NULL;
	ptr->next_1=NULL;
	printf("The initialization_of_node was completed\n");

}