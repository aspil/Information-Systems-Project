typedef struct spec spec;
typedef struct Node Node;
struct spec
{
	char *name;
	char *value;
	spec *next;
};

struct Node
{
	int id;
	char *name_of_site;
	spec *rest_of_specs;
	spec *last;
	Node *next_1;
};

typedef struct hashtable
{
	Node *next,*last;
}hashtable;
