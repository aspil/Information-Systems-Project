#include "../../include/util/text_preprocessing.h"

#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void strip_ext(char *fname)
{
	char *end = fname + strlen(fname);
	while (end > fname && *end != '.')
		--end;

	if (end > fname)
		*end = '\0';
	return;
}

void get_line_without_end_line(char *str)
{
	int i = 0;
	while (str[i] != '\n' && str[i] != '\0')
		i++;

	str[i] = '\0';
	return;
}

void skip_whitespace(char *str)
{
	int i, x;	 // code snippet to ignore whitespaces
	for (i = x = 0; str[i]; ++i) {
		if (!isspace(str[i]) || (i > 0 && !isspace(str[i - 1])))
			str[x++] = str[i];
	}
	str[x] = '\0';
	// return str;
}

char *strrem_special_characters(char *str)
{
	str = strrem_occurence(str, "\\u00d7");
	str = strrem_occurence(str, "\\u00b0");
	str = strrem_occurence(str, "\\u00e2");
	str = strrem_occurence(str, "\\u0080");
	str = strrem_occurence(str, "\\u0099");
	str = strrem_occurence(str, "\\u201d");
	str = strrem_occurence(str, "\\u03a6");
	str = strrem_occurence(str, "\'s");

	for (size_t i = 0; i < strlen(str); ++i) {
		if ((str[i] == '(') || (str[i] == ')') || (str[i] == ':') || (str[i] == '[') ||
			(str[i] == ']') || (str[i] == '\\') || (str[i] == '+') ||
			(strcmp((str + i), "\\n") == 0) || (str[i] == ',') || (strcmp((str + i), "n/a") == 0)) {
			str[i] = ' ';
		}
		str[i] = tolower(str[i]);
	}
	if (str[strlen(str) - 1] == '.')
		str[strlen(str) - 1] = '\0';
	return str;
}

char *strrem_occurence(char *str, const char *sub)
{
	size_t len = strlen(sub);
	if (len > 0) {
		char *p = str;
		while ((p = strstr(p, sub)) != NULL) {
			memmove(p, p + len, strlen(p + len) + 1);
		}
	}
	return str;
}