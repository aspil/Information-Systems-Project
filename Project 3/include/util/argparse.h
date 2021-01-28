#define POS_DBG 1
#define NEG_DBG 2
#define ALL_DBG 0
#define ERR_DBG -1

#define TRN_MD 1
#define TST_MD 2
#define VAL_MD 3
#define ITR_MD 4

int parse_cmd_arguments(int		argc,
						char *	argv[],
						char ** datapath,
						char ** relationsfile,
						int *	max_features,
						double *learning_rate,
						int *	epochs,
						int *	batch_size,
						int *	n_threads,
						int *	stratify,
						int *	debug);

void check_multiple_option_redefinition(char **argv, char opt, int optflag);

void check_mandatory_options(char **argv, char opt, int optflag);