#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "../include/util.h"
#include "../include/map.h"
#include "../include/clique.h"


void strip_ext(char *fname) {
    char *end = fname + strlen(fname);
    while (end > fname && *end != '.')
        --end;
    
    if (end > fname)
        *end = '\0';
    return;
}


int count_files(char*path)
{
    struct dirent *pDirent,*iDirent;
    char *subdir;
    int counter=0;
    DIR *pDir,*iDir;
    //printf("%s\n",path );

    pDir = opendir (path); //anoigma arxeiou 
    if (pDir == NULL) 
    {
        perror("Error: ");
        printf ("Cannot open directory '%s'\n", path); //se periptwsh la8ous
        return -1;
    }

    while ((pDirent = readdir(pDir)) != NULL) 
    {
        if (pDirent->d_name[0] != '.') 
        {

            subdir=malloc (strlen(path)+ 2 + strlen(pDirent->d_name));
            
            strcat(subdir,path);
            
            strcat(subdir,"/"); //ftiaxe to neo path gia na metrhseis ta arxeia
            
            strcat(subdir,pDirent->d_name);     
            
            iDir = opendir (subdir);

            if (iDir == NULL) 
            {
                printf ("Cannot open directory '%s'\n", path);
                return -1;
            }

            while ((iDirent = readdir(iDir)) != NULL) 
            {
                if (iDirent->d_type==DT_REG) //an einai arxeio metrhse to
                {
                    counter++;
                }
            }
            closedir(iDir);
            free(subdir);
         }
    }

    closedir (pDir);

    return counter;

}

int pick_the_buckets(int arg_c,char **arg_v)
{

    int number_of_buckets;
    long ret;
    char *ptr;
    for (int i = 0; i < arg_c; ++i)
    {
        if (strcmp(arg_v[i],"-s")==0)
        {
            //there is -s argument
            i++;//sthn epomenh 8esh 8a uparxei poso megalo 8a einai
            if (i==arg_c)
            {
                //den uparxei tetoia timh kai as uparxei to -s
                //se auth th periptwsh kalese na metrhsei ta arxeia
                number_of_buckets=count_files(arg_v[1]);
                if (number_of_buckets<=0)
                {
                    printf("No data to be input \n");
                    return 0;
                }
                return number_of_buckets;
            }
            ret=strtol(arg_v[i], &ptr, 10);
            //printf("%ld \n",ret );
            if (ret==0)
            {
                number_of_buckets=count_files(arg_v[1]);
                return number_of_buckets;
                
            }
            number_of_buckets= atoi(arg_v[i]);
            if (number_of_buckets<=0)
            {
                printf("No data to be input \n");
                return 0;
            }
            return number_of_buckets; 
        }
    }
//den uparxei -s metrhse ta arxeia 
    number_of_buckets=count_files(arg_v[1]);
    if (number_of_buckets<=0)
    {
        printf("No data to be input \n");
        return 0;
    }
    return number_of_buckets;

}


int read_data_X(struct hash_map *ptr,int size,char *path)
{
    struct dirent *pDirent,*iDirent;
    char *subdir,*value=NULL,*path_to_file;
    struct clique *new_clique;
    //int counter=0;
    DIR *pDir,*iDir;

    pDir = opendir (path); //anoigma tou path
    if (pDir == NULL) 
    {
        printf ("Cannot open directory '%s'\n", path);
        return -1;
    }

    while ((pDirent = readdir(pDir)) != NULL) 
    {
        if (pDirent->d_name[0] != '.') //diabase ola ektos apo tis . 
        {
            //puts(pDirent->d_name);    
            subdir=malloc (strlen(path)+ 2 + strlen(pDirent->d_name));
          //  printf("%s\n",subdir );
           // printf("%s \n",path );
            strcpy(subdir,path);
            strcat(subdir,"/"); //create each path for each site
            strcat(subdir,pDirent->d_name);
           // printf("%s \n",subdir );        
            iDir = opendir (subdir); 

            if (iDir == NULL) 
            {
                printf ("Cannot open directory '%s'\n", subdir);
                return -1;
            }

            while ((iDirent = readdir(iDir)) != NULL) //open site folder
            {
                //printf("Name is %s\n",iDirent->d_name );
                //printf("allh mia epanalhpsh\n");
                if (iDirent->d_name[0] != '.') //open each product file
                {
                    char *path_help;
                    
                    path_help=malloc(strlen(iDirent->d_name)+3+strlen(pDirent->d_name));

                    strcpy(path_help,pDirent->d_name);

                    strcat(path_help,"//");
                    
                    strcat(path_help,iDirent->d_name);

                    strip_ext(path_help); //remove .json and keep the result of site/id as key for hashing

                    //create the clique to pass it as argument
                    new_clique=create_new();

                    //time for hashing 
                    //printf("The key is %s \n",path_help );
                    map_insert(ptr, path_help, new_clique);

                    // path for file
                    path_to_file=malloc(strlen(subdir)+2+strlen(iDirent->d_name));
                    strcpy(path_to_file,subdir);
                    strcat(path_to_file,"/");
                    strcat(path_to_file,iDirent->d_name);

                    //call the function to create the product and its info

                    hash_the_node(new_clique,path_to_file,iDirent->d_name,pDirent->d_name);




                    

                    //            

                   // hash_the_node(ptr,result_of_hash,json_path,token);
                   // printf("Path help is %s\n",path_help );
                    //free(path_help);
                    //free(token);
                    //free(path_help);
                   // printf("koitaxe kai edw\n");
                    //na koitaxw ta free gia leaks tou path help
                    //printf("Token is %s in  %s and the hash_result is %ld \n",token,subdir, result_of_hash);

                }   
            }

       // printf("bges apo thn epanalhpsh\n");
       // printf("The subdir is %s \n",subdir );
        closedir(iDir);
       // printf("TO subdir einai %s\n",subdir );
        memset(subdir, '\0' , strlen(subdir));
       // printf("TO subdir einai %s\n",subdir );
        free(subdir);
        subdir=value;
        //printf("TO subdir einai 2 %s\n",subdir );
        //printf("peras kai edw\n");
         }
    }

        // Close directory and exit.

    //printf("kai edw pera ga\n");
    closedir (pDir);


    return 1;
}

void hash_the_node(struct clique *ptr,char *path,char *id, char *site)
{
   // printf("THe path is : %s\n",path );
   // printf("The size of this %d and the pointer is in %p \n",ptr->size,ptr );
    strip_ext(id);
   // printf("the site is %s and the id is %s \n",site,id);

    //initialazation of the product

    product_init(ptr,atoi(id),site);
   // printf("The initialazation was good lets see: %d %s %d \n",ptr->size,ptr->first_product->website,ptr->first_product->id );

    
    FILE *fp;
    char * line = NULL,*temp_1=NULL,*temp2=NULL,*temp_3=NULL;
   // int flag;
    size_t len = 0;
    ssize_t read;
    char *str;
    fp = fopen (path,"r");
    if (fp == NULL) 
    {
        printf ("Cannot open directory '%s'\n", path);

    }
    while ((read = getline(&line, &len, fp)) != -1) 
    {
        //take the line of the file
        str=line;
        if (line[0]!='{' && line[0]!='}') //ignore { and }
        {
            int i, x; //code snippet to ignore whitespaces
            for(i=x=0; str[i]; ++i)
                if(!isspace(str[i]) || (i > 0 && !isspace(str[i-1])))
                 str[x++] = str[i];
            str[x] = '\0';

            //format sequence 
            if (str[0]=='"')
            {
                str=str+1;
                if (str[0]=='<')
                {
                    str=str+1; //important for the first spec : page title 
                }
               
                //we got rid of the first symbols

                char *helping_str=str;

                while (helping_str[0]!='"') 
                {
                    if (helping_str[0]=='>') //code snipet for page title end 
                    {
                        //helping_str[0]='\0';
                        //flag=1;
                        //printf("flag is %d \n",flag );
                        break;
                    }
                    else if (helping_str[0]=='\\') //code snipet for " that dont declare the end in this case 
                    {
                        if (helping_str[1]=='"')
                        {
                            helping_str++; 
                        }
                    }
                    helping_str++;
                }
                helping_str[0]='\0';

              //  printf("Now the result is %s\n",str );
                
                temp_1=malloc(strlen(str)+1); //the spec characteristic 
                strcpy(temp_1,str);
              //  printf("Temp is %s \n",temp_1);

                //now i will find its value by passing whitespaces,: 
                str=strlen(str) + 2 +str;
              //  printf("%c \n",str[0] );
                while (str[0] != '"' && str[0]!='[') 
                {
                    str=str+1;
                    //printf("%c \n",str[0] );
                }
               // printf("Result here is %s\n",str );
                if (str[0]=='"')
                {
                    str=str+1;  //pass the "
                    
                    helping_str=str;

                    while (helping_str[0]!='"')
                    {
                        if (helping_str[0]=='\\')
                        {
                            if (helping_str[1]=='"') //surpass this "
                            {
                                helping_str++; 
                            }
                        }
                        //printf("%c",helping_str[0] );
                        helping_str++;
                    }
                    helping_str[0]='\0';
                 //   printf("SECOND RESULT %s\n",str );
                    temp2=malloc(strlen(str)+1);
                    strcpy(temp2,str);
                  //  printf("The value of the item is %s \n",temp2 );
                  //  printf("\n");
                
                    //we have both values spec and value in temp 
                    //we can pass it
                    Array dyn_arr;

                    initArray(&dyn_arr,1,temp2); //create an array of the values
                    
                    push_specs(ptr,temp_1,(char**)dyn_arr.array);

                  //  printf("The initialazation was good lets see: %d %s %d %s %s  \n",ptr->size,ptr->first_product->website,ptr->first_product->id,ptr->first_product->next_spec->spec.name, ptr->first_product->next_spec->spec.value[0]);



                }
                else if (str[0]=='[')
                {
                    //there is a list of values we need to take care of

                    int counter=0;

                    Array dyn_arr;

                    str=str+1;

                    //the values  will be one on each  next line

                    while ((read = getline(&line, &len, fp)) != -1)
                    {
                        str=line;
                        for(i=x=0; str[i]; ++i)
                            if(!isspace(str[i]) || (i > 0 && !isspace(str[i-1]))) //delete the whitespaces 
                                str[x++] = str[i];
                        str[x] = '\0';
                        
                        if (str[0]=='"')
                        {
                            str=str+1;

                            helping_str=str;

                            while (helping_str[0]!='"')
                            {
                                if (helping_str[0]=='\\')
                                {
                                    if (helping_str[1]=='"') //surpass this "
                                    {
                                        helping_str++; 
                                    }
                                }
                                //printf("%c",helping_str[0] );
                                helping_str++;
                            }
                            helping_str[0]='\0';
                            counter++;

                            if (counter==1)
                            {
                                //initialazation of the array
                                temp_3=malloc(strlen(str)+1);
                                strcpy(temp_3,str);
                                initArray(&dyn_arr,1,temp_3);

                            }
                            else
                            {
                                temp_3=malloc(strlen(str)+1);
                                strcpy(temp_3,str);
                                insertArray(&dyn_arr,temp_3);

                            }

                            //printf("The result is:  %s \n",str );
                                    
                        }
                        else 
                        {
                            push_specs(ptr,temp_1,(char**)dyn_arr.array);
                            break;
                        }


                    }

                }


                //printf("The new thing u will see is %s \n", str);
               // flag=0;
                
                }

        
        } 

    }
    //fprintf (fp, "Hello, there.\n"); // if you want something in the file.
    fclose (fp);

    
    
    //printf("To teleiwses kai auto \n");
}

void dataset_y(struct hash_map *map,char *path)
{
    //open the csv file to take relations

    FILE *fp;
    char * line = NULL,*temp_1=NULL,*temp2=NULL,*temp_2;
    //int flag;
    size_t len = 0;
    ssize_t read;
    char *str;
    fp = fopen (path,"r");
    if (fp == NULL) 
    {
        printf ("Cannot open directory '%s'\n", path);

    }


    if ((read = getline(&line, &len, fp)) != -1)
    {
        get_line_without_end_line(line);
        if (strcmp(line,"left_spec_id,right_spec_id,label")==0) //it follows the coding i want
        {
            while ((read = getline(&line, &len, fp)) != -1) 
            {   
                //i need to take the left spec first
                //this will be until the first comma 
                printf("The line is : %s \n",line);
                if (line[strlen(line)-2]=='1') //no reason to break into products the 0 values 
                {
                    /* code */
                //printf("%s\n",line );

                str=line;

                while(str[0]!=',')
                {
                    str=str + 1;
                }

                str[0]='\0';

                temp_1=malloc(strlen(str)+1);

                strcpy(temp_1,line); //we got the first product 

                //printf("The first product is %s \n",temp_1 );
                temp2=str + 1 ; //get the second product 

                while(str[0]!=',')
                {
                    str=str + 1;
                }

                str[0]='\0';

               // printf("The second site is %s \n",temp2 );

                temp_2=malloc(strlen(temp2)+1);

                strcpy(temp_2,temp2);

                //we have both now

                // i will hash them to find where they are so i can merge them

                search_and_change_(temp_1,temp2,map);
                }
                    
            }
        }
        else
        {
            printf("Error in coding didnt find the appropriate fields \n");
        }
    }
}

void get_line_without_end_line(char *str)
{
    int i=0;
    
    while (str[i]!='\n')
    {
        i++;
    }

    str[i]='\0';

}


void print_results(struct hash_map *map)
{
    printf("mpes kai edw \n");
    //struct clique **clique_array=NULL;
    int counter=0;
    Array dyn_arr;
    struct map_node *ptr;
    struct clique *last,*print_ptr;
    struct product *iteration_first_product;
    struct product *print_product;
    //printf("the size  1 is %ld \n",sizeof(clique_array) );
    int result=0;

    for (int i = 0; i < map->size; ++i)
    {
        ptr=map->array[i];

        while (ptr!= NULL) //there are things to see
        {
            print_ptr=(struct clique *) ptr->value;
           // printf("Edw gamw to kArkino sou %s %d %d \n",print_ptr->first_product->website,print_ptr->first_product->id,i );
            if (counter==0)
            {
                initArray_2(&dyn_arr,1,map->array[i]->value);
                counter++;
                last=ptr->value;
                iteration_first_product=last->first_product;
               // struct product *print_product;
                //printf("The size of clique is : %d \n", last->size);
                /*for (int i = 0; i < last->size; ++i)
                {
                    for (int j = i+1; j < last->size; ++j)
                    {
                        printf("kai edw\n");
                        for (int k = 0; k < j; ++k)
                        {
                            print_product=print_product->next;
                        }
                        printf("%s//%d %s//%d \n",iteration_first_product->website,iteration_first_product->id,print_product->website,print_product->id );
                        print_product=iteration_first_product;
                    }
                    iteration_first_product=iteration_first_product->next;
                    print_product=iteration_first_product;
                }*/
                while (iteration_first_product!=NULL)
                {
                    print_product=iteration_first_product->next;
                    while (print_product!=NULL)
                    {
                        printf("Only here %s//%d %s//%d \n",iteration_first_product->website,iteration_first_product->id,print_product->website,print_product->id );
                        print_product=print_product->next;
                    }
                    iteration_first_product=iteration_first_product->next;
                }
            }
            else
            {
                //printf("Compare %s %d %d \n",print_ptr->first_product->website,print_ptr->first_product->id,i );
                result=compare_array(dyn_arr,ptr->value); //checks if the clique you want to print has already been printed 
            }
            if (result==-1) 
            {
               // you can print this clique

                last=ptr->value;
                iteration_first_product=last->first_product;

                /*printf("Once here%s %d \n",iteration_first_product->website,iteration_first_product->id );

                if (iteration_first_product->next !=NULL)
                {
                    printf("second here %s %d \n", iteration_first_product->next->website,iteration_first_product->next->id);
                    iteration_first_product=iteration_first_product->next;
                }
        
                if (iteration_first_product->next !=NULL)
                {
                    printf("third here %s %d \n", iteration_first_product->next->website,iteration_first_product->next->id);
                    iteration_first_product=iteration_first_product->next;
                }*/
                while (iteration_first_product!=NULL)
                {
                    print_product=iteration_first_product->next;
                    while (print_product!=NULL)
                    {
                        printf("%s//%d %s//%d \n",iteration_first_product->website,iteration_first_product->id,print_product->website,print_product->id );
                        print_product=print_product->next;
                    }
                    iteration_first_product=iteration_first_product->next;
                }
                //printf("done\n");
                insertArray_2(&dyn_arr,last);   
            }

           ptr=ptr->next;      //check the next bucket
         }
            
        }   

}

int compare_array(Array dyn_arr,struct clique *address)
{
    //int size=sizeof(clique_array)/sizeof(struct clique *);

   // printf("the size is %ld \n",sizeof(clique_array) );

    //printf("ena mhnuma\n");
    struct clique *ptr;

    for (int i = 0; i < dyn_arr.size; ++i)
    {
        //printf("%d \n",  dyn_arr.size);
        ptr=(struct clique *) dyn_arr.array[i];
        
        if (address->first_product==ptr->first_product)
        {

            return 1;
        }
    }

    return -1;
}