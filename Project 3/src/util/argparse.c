#include "../../include/util/argparse.h"

#include <dirent.h>
#include <errno.h>
#include <float.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
// #include <unistd.h>
#define SAFE_ATOI(arg) safe_atoi(arg)
#define SAFE_ATOD(arg) safe_atod(arg)

static int safe_atoi(const char *nptr)
{
	char *endptr = NULL;
	errno = 0;
	long number = strtol(nptr, &endptr, 10);

	if (nptr == endptr)
		fprintf(stderr, "safe_atoi : no digits found\n");
	else if (errno == ERANGE && number == LONG_MIN)
		fprintf(stderr, "safe_atoi : underflow occurred\n");
	else if (errno == ERANGE && number == LONG_MAX)
		fprintf(stderr, "safe_atoi : overflow occurred\n");
	else if (errno == EINVAL)
		fprintf(stderr, "safe_atoi : base contains unsupported value\n");
	else if (errno != 0 && number == 0)
		fprintf(stderr, "safe_atoi : unspecified error occurred\n");
	else if (errno == 0 && nptr && *endptr != 0)
		fprintf(stderr, "safe_atoi : additional characters remain\n");
	else if (errno == 0 && nptr && !*endptr)
		return (int) number;

	exit(EXIT_FAILURE);
}

static double safe_atod(const char *nptr)
{
	char *endptr = NULL;
	errno = 0;
	double number = strtod(nptr, &endptr);

	if (nptr == endptr)
		fprintf(stderr, "safe_atod : no digits found\n");
	else if (errno == ERANGE && number == DBL_MIN)
		fprintf(stderr, "safe_atod : underflow occurred\n");
	else if (errno == ERANGE && number == DBL_MAX)
		fprintf(stderr, "safe_atod : overflow occurred\n");
	else if (errno != 0 && number == 0)
		fprintf(stderr, "safe_atod : unspecified error occurred\n");
	else if (errno == 0 && nptr && *endptr != '\0')
		fprintf(stderr, "safe_atod : additional characters remain\n");
	else if (*endptr != '\0')
		fprintf(stderr, "safe_atod : additional characters remain\n");
	else if (number != number)
		fprintf(stderr, "safe_atod : got nan value\n");
	else if (errno == 0 && nptr && !*endptr)
		return number;

	exit(EXIT_FAILURE);
}

static struct option long_options[] = {
	{"mode", required_argument, NULL, 'm'},
	{"input_data", required_argument, NULL, 'i'},
	{"relations", required_argument, NULL, 'r'},

	{"max_features", required_argument, NULL, 'f'},
	{"learning_rate", required_argument, NULL, 'l'},

	{"nthreads", required_argument, NULL, 't'},
	{"batch_size", required_argument, NULL, 'b'},
	{"epochs", required_argument, NULL, 'e'},
	{"debug", required_argument, NULL, 'd'},
	{"help", no_argument, NULL, 'h'},
	{NULL, 0, NULL, 0},
};

static const char *usage =
	"\e[1mUSAGE\e[0m\n\t ./bin/app -m \e[4mMODE\e[0m -i \e[4mDATAPATH\e[0m "
	"-r \e[4mRELATIONSFILE\e[0m -f \e[4mFEATURES\e[0m -l \e[4mRATE\e[0m "
	"-t \e[4mNUMTHREADS\e[0m -b \e[4mBATCHSIZE\e[0m [-d \e[4mDEBUG\e[0m]\n\n"

	"\e[1mDISCLAIMERS\e[0m\n"
	"\n\tBefore running the program with \e[4mMODE\e[0m=\e[1mtest\e[0m, ensure that you have "
	"already\n"
	"\tran it with \e[4mMODE\e[0m=\e[1mtrain\e[0m. Otherwise the execution will fail.\n\n"

	"\tIf \e[4mMODE\e[0m=\e[1mtest\e[0m|\e[1mvalidate\e[0m, only provide -m -i and -r options. The "
	"rest (valid ones) will be ignored.\n\n"

	"\e[1mOPTIONS\e[0m\n"

	"\n\t-m \e[4mMODE\e[0m, --mode=\e[4mMODE\e[0m\n\t\tthe mode of the app that will be executed, "
	"\e[4mMODE\e[0m is one of \e[1mtrain\e[0m, \e[1mtest\e[0m, \e[1mvalidate\e[0m.\n"

	"\n\t-i \e[4mDATAPATH\e[0m, --input_data=\e[4mDATAPATH\e[0m\n\t\t"
	"\e[4mDATAPATH\e[0m is the path to the json files that the app needs to parse.\n"

	"\n\t-r \e[4mRELATIONSFILE\e[0m, --relations=\e[4mRELATIONSFILE\e[0m\n\t\t"
	"\e[4mRELATIONSFILE\e[0m is the csv file's name that contains all the labels.\n"

	"\n\t-f \e[4mFEATURES\e[0m, --max_features=\e[4mFEATURES\e[0m\n\t\t"
	"\e[4mFEATURES\e[0m(int) is the number of features that will be used to reduce the "
	"dimensionality of the word vectors used by the model.\n"

	"\n\t-l \e[4mRATE\e[0m, --learning_rate=\e[4mRATE\e[0m\n\t\t"
	"Provide learning rate into \e[4mRATE\e[0m(double) to be used in gradient descent.\n"

	"\n\t-t \e[4mNUMTHREADS\e[0m, --nthreads=\e[4mNUMTHREADS\e[0m\n\t\t"
	"The number of threads to parallelize some procedures.\n"

	"\n\t-t \e[4mBATCHSIZE\e[0m, --nthreads=\e[4mBATCHSIZE\e[0m\n\t\t"
	"The size of the sample batches in gradient descent.\n"

	"\n\t-d \e[4mDEBUG\e[0m, --debug=\e[4mDEBUG\e[0m\n\t\t"
	"Controls whether the program's output will contain results for the cliques while parsing the "
	"initial labels.\n\t\t\e[4mDEBUG\e[0m is one of "
	"\e[1mpositive\e[0m,\e[1mnegative\e[0m,\e[1mall\e[0m.\n\n"

	"\e[1mEXAMPLE\e[0m\n\n"
	"\t./bin/app -m train -i data/camera_specs/ -r "
	"data/initial_labels/sigmod_large_labelled_dataset.csv -f 1500 -l 0.001\n"
	"\n\t\tTrain the model with data from data/camera_specs/ directory, labels from "
	"data/initial_labels/sigmod_large_labelled_dataset.csv,\n\t\treduce the dataset's features "
	"down to 1500 and perform gradient descent with learning rate = 0.001.\n\n"
	"\t./bin/app -m test -i data/camera_specs/ -r "
	"data/initial_labels/sigmod_large_labelled_dataset.csv";

void check_multiple_option_redefinition(char **argv, char opt, int optflag)
{
	if (optflag == 1) {
		fprintf(stderr, "%s: warning:  -%c is set multiple times.\n", argv[0], opt);
		fprintf(stderr, "Use ./bin/app -h or ./bin/app --help for more info.\n");
		exit(EXIT_FAILURE);
	}
}

void check_mandatory_options(char **argv, char opt, int optflag)
{
	if (optflag == 0) {
		fprintf(stderr, "%s: missing -%c option.\n", argv[0], opt);
		fprintf(stderr, "Use ./bin/app -h or ./bin/app --help for more info.\n");
		exit(EXIT_FAILURE);
	}
}

int parse_cmd_arguments(int		argc,
						char *	argv[],
						char ** datapath,
						char ** relationsfile,
						int *	max_features,
						double *learning_rate,
						int *	n_threads,
						int *	batch_size,
						int *	epochs,
						int *	debug)
{
	int	  mflag = 0, iflag = 0, rflag = 0, fflag = 0, lflag = 0, dflag = 0, tflag = 0, bflag = 0, eflag = 0;
	int	  option_index = 0, flag = 0, c, prev_index;
	char *mode = NULL;

	struct stat path_stat;
	opterr = 0;

	while (prev_index = optind,
		   (c = getopt_long(argc, argv, ":m:i:r:f:l:d:t:b:e:h", long_options, &option_index)) != -1) {
		if (optind == prev_index + 2 && *optarg == '-') {
			c = ':';
			--optind;
			flag = 1;
		}
		switch (c) {
			case 1:
				fprintf(stderr, "%s: found '%s'\n", argv[0], optarg);
				break;

			case 'm':
				check_multiple_option_redefinition(argv, 'm', mflag);
				mflag = 1;
				mode = optarg;
				break;

			case 'i':
				check_multiple_option_redefinition(argv, 'i', iflag);
				iflag = 1;
				stat(optarg, &path_stat);
				if (S_ISDIR(path_stat.st_mode)) {
					*datapath = optarg;
				}
				else if (S_ISREG(path_stat.st_mode)) {
					fprintf(stderr, "%s: Error at -r value '%s' : ", argv[0], optarg);
					fprintf(stderr, "value must be a valid directory\n");
					fprintf(stderr, "Use ./bin/app -h or ./bin/app --help for more info.\n");
					exit(EXIT_FAILURE);
				}
				else {
					fprintf(stderr, "%s: Error at -r value '%s'", argv[0], optarg);
					perror(" ");
					fprintf(stderr, "Use ./bin/app -h or ./bin/app --help for more info.\n");
					exit(EXIT_FAILURE);
				}

				break;

			case 'r':
				check_multiple_option_redefinition(argv, 'r', rflag);
				rflag = 1;
				stat(optarg, &path_stat);
				if (S_ISREG(path_stat.st_mode)) {
					*relationsfile = optarg;
				}
				else if (S_ISDIR(path_stat.st_mode)) {
					fprintf(stderr, "%s: Error at -r value '%s' : ", argv[0], optarg);
					fprintf(stderr, "value must be a valid path to file\n");
					fprintf(stderr, "Use ./bin/app -h or ./bin/app --help for more info.\n");
					exit(EXIT_FAILURE);
				}
				else {
					fprintf(stderr, "%s: Error at -r value '%s'", argv[0], optarg);
					perror(" ");
					fprintf(stderr, "Use ./bin/app -h or ./bin/app --help for more info.\n");
					exit(EXIT_FAILURE);
				}
				break;

			case 'f':
				check_multiple_option_redefinition(argv, 'f', fflag);
				fflag = 1;
				*max_features = SAFE_ATOI(optarg);
				break;

			case 'l':
				check_multiple_option_redefinition(argv, 'l', lflag);
				lflag = 1;
				*learning_rate = SAFE_ATOD(optarg);
				break;

			case 'e':
				check_multiple_option_redefinition(argv, 'l', eflag);
				eflag = 1;
				*epochs = SAFE_ATOI(optarg);
				break;
			case 'b':
				check_multiple_option_redefinition(argv, 'b', bflag);
				bflag = 1;
				*batch_size = SAFE_ATOI(optarg);
				break;

			case 't':
				check_multiple_option_redefinition(argv, 't', tflag);
				tflag = 1;
				*n_threads = SAFE_ATOI(optarg);
				break;
			case 'd':
				check_multiple_option_redefinition(argv, 'd', dflag);
				dflag = 1;
				if (strcmp(optarg, "Positive") == 0)
					*debug = POS_DBG;
				else if (strcmp(optarg, "Negative") == 0)
					*debug = NEG_DBG;
				else if (strcmp(optarg, "All") == 0)
					*debug = ALL_DBG;
				else {
					fprintf(stderr, "%s: Error: -d argument value must be one of Positive, Negative, All.\n", argv[0]);
					fprintf(stderr, "Use ./bin/app -h or ./bin/app --help for more info.\n");
					exit(EXIT_FAILURE);
				}
				break;

			case 'h':
				if (dflag == 1) {
					fprintf(stderr, "%s: warning:  -h is set multiple times.\n", argv[0]);
					fprintf(stderr, "Use ./bin/app -h or ./bin/app --help for more info.\n");
					exit(EXIT_FAILURE);
				}
				dflag = 1;
				printf("%s\n", usage);
				exit(EXIT_SUCCESS);

			case '?':
				fprintf(stderr, "Unknown option -%c.\n", optopt);
				fprintf(stderr, "Use ./bin/app -h or ./bin/app --help for more info.\n");
				exit(EXIT_FAILURE);

			case ':':
				if (flag == 1) {
					fprintf(stderr, "%s: option '%s' requires an argument\n", argv[0], argv[optind - 1]);
				}
				else {
					fprintf(stderr, "%s: option '-%c' requires an argument.\n", argv[0], optopt);
				}
				fprintf(stderr, "Use ./bin/app -h or ./bin/app --help for more info.\n");
				exit(EXIT_FAILURE);

			default:
				abort();
		}
	}

	/* Print any remaining command line arguments (not options). */
	// for (index = optind; index < argc; index++)
	if (optind < argc) {
		fprintf(stderr, "Non-option argument %s.\n", argv[optind]);
		fprintf(stderr, "Use ./bin/app -h or ./bin/app --help for more info.\n");
		exit(EXIT_FAILURE);
	}

	check_mandatory_options(argv, 'm', mflag);
	check_mandatory_options(argv, 'i', iflag);
	check_mandatory_options(argv, 'r', rflag);
	// check_mandatory_options(argv, 'h', hflag);
	check_mandatory_options(argv, 'f', fflag);
	check_mandatory_options(argv, 'l', lflag);
	check_mandatory_options(argv, 't', tflag);
	check_mandatory_options(argv, 'b', bflag);

	if ((strcmp(mode, "train") == 0)) {
		check_mandatory_options(argv, 'f', fflag);
		check_mandatory_options(argv, 'l', lflag);
	}

	if (strcmp(mode, "train") == 0)
		return TRN_MD;
	else if (strcmp(mode, "test") == 0)
		return TST_MD;
	else
		return VAL_MD;
}