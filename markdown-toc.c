
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

static int make_toc(FILE *fp);
static int lets_toc(char *line);
static int print_toc(char *s);
static char *backtick_find(char *s, int *n);
static char *backtick_match(char *s, int n);
static int backtick_conclude(char *s);


int main(int argc, char **argv) 
{
	FILE	*fp;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <markdown-file>\n", argv[0]);
		return 1;
	}

	if ((fp = fopen(argv[1], "r")) != NULL) {
		make_toc(fp);
		fclose(fp);
	}
	return 0;
}


static int make_toc(FILE *fp)
{
	char	*s, line[MAX_LINE_LENGTH];
	int	backtick = 0;

	while (fgets(line, sizeof(line), fp)) { 
		if (backtick == 0) {
			backtick = lets_toc(line);
		} else {
			s = backtick_match(line, backtick);
			if (s) {
				backtick = backtick_conclude(s);
			}
		}
	}
	return 0;
}


static int lets_toc(char *line)
{
	int	i;

	for (i = 0; i < 4; i++, line++) {
		if (isspace(*line)) {
			continue;	/* heading text allow at most 3 spaces */
		} else if (*line == '#') {
			print_toc(line);
			return 0;
		} else {
			break;	/* not a heading text */
		}
	}
	/* not a heading text, so lets try find some backticks */
	return backtick_conclude(line);
}


/* 's' points to the first validated '#' */
static int print_toc(char *s)
{
	char	anchor[MAX_LINE_LENGTH];
	int	i, n;
	
	/* count the number of '#' to determine the heading level */
	for (n = 0; s[n] == '#'; n++);

	/* make sure it's a valid head */
	if (!isspace(s[n]) || !s[n+1]) {		/* need at least two characters after '#' */
		return 0;
	}

	s += n + 1;		/* move to the start point of the heading text */
	s[strlen(s)-1] = 0;	/* remove the newline character at the end */

	/* generate anchor from the heading text */
	strcpy(anchor, s);
	for (i = 0; anchor[i]; i++) {
		if (anchor[i] == ' ') anchor[i] = '-';
		else anchor[i] = tolower(anchor[i]);
	}

	/* print the TOC line */
	printf("%*s- [%s](#%s)\n", (n - 1) * 2, "", s, anchor);
	return n;
}

/* from 's' searching for backticks,  return NULL if not found
 * or return the pointer to the first character after backticks 
 * and 'n' with the number of the found backticks */
static char *backtick_find(char *s, int *n)
{
	char	*p;
	int	c;

	if ((s = strchr(s, '`')) == NULL) {
		if (n) *n = 0;
		return NULL;
	}

	for (c = 0, p = s; *p && *p == '`'; c++, p++);
	if (n) *n = c;		/* return the number of backticks */
	return p;		/* return the pointer to the first byte after backticks */
}

/* from 's' searching for matching backticks, return NULL if not found
 * or return the pointer to the first character after the matching backticks.
 * the matching number is specified by 'n' */
static char *backtick_match(char *s, int n)
{
	char	*p;
	int	c;

	while ((s = strchr(s, '`')) != NULL) {
		for (c = 0, p = s; *p && *p == '`'; c++, p++);
		if (n == c) {	/* find the matching backtick cluster */
			return p;
		}
		if (*p == 0) {
			break;
		}
		s = p;
	}
	return NULL;	/* no matching found */
}


/* from 's' searching for matching backticks pairs.
 * return number of backtick cluster if found */
static int backtick_conclude(char *s)
{
	int	n = 0;

	while (s) {
		if ((s = backtick_find(s, &n)) == NULL) {
			break;
		}
		if ((s = backtick_match(s, n)) == NULL) {
			break;
		}
	}
	return n;
}

