
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 80

static int generate_toc(char *fname, int flag);
static int file_merge(FILE *fout, FILE *fhead, FILE *ftoc, FILE *fbody);
static int file_append(FILE *fp, FILE *fn);
static int make_toc(char *line, FILE *fout, int backtick);
static int lets_toc(char *line, FILE *fout);
static int print_toc(char *s, FILE *fout);
static char *backtick_find(char *s, int *n);
static char *backtick_match(char *s, int n);
static int backtick_conclude(char *s);


int main(int argc, char **argv) 
{
	int	owrt = 0;

	while (--argc && ((**++argv == '-') || (**argv == '+'))) {
                if (!strcmp(*argv, "-V") || !strcmp(*argv, "--version")) {
                        puts("md_toc 1.0");
                        return 0;
                } else if (!strcmp(*argv, "-H") || !strcmp(*argv, "--help")) {
			puts("Usage: md_toc [-o] <markdown-file ...>");
                        return 0;
		} else if (!strcmp(*argv, "-o") || !strcmp(*argv, "--overwrite")) {
			owrt = 2;
		} else if (!strcmp(*argv, "-s") || !strcmp(*argv, "--show")) {
			owrt = 1;
		} else {
                        fprintf(stderr, "%s: unknown parameter.\n", *argv);
                        return -1;
                }
	}

	/* input from stdin */
        if ((argc == 0) || !strcmp(*argv, "--")) {
		generate_toc(NULL, 0);
                return 0;
        }

	for ( ; argc; argc--, argv++) {
		generate_toc(*argv, owrt);
	}
	return 0;
}

static int generate_toc(char *fname, int flag)
{
	FILE	*fin, *ftoc, *fhead, *fbody;
	char    line[MAX_LINE_LENGTH];
	int	tocsm, btsm;

	if (fname == NULL) {
		fin = stdin;
	} else if ((fin = fopen(fname, "r+")) == NULL) {
		perror(fname);
		return 1;
	}

	ftoc  = tmpfile();
	fhead = tmpfile();
	fbody = tmpfile();

	tocsm = btsm = 0;
	while (fgets(line, sizeof(line), fin)) {
		switch (tocsm) {
		case 0:		/* before TOC */
			fputs(line, fhead);
			if (!strncmp(line, "<!--toc-->", 10)) {
				tocsm = 1;
			}
			break;
		case 1:		/* inside TOC */
			if (!strncmp(line, "<!--toc-->", 10)) {
				fputs(line, fbody);
				tocsm = 2;
			}
			break;
		case 2:		/* after TOC */
			fputs(line, fbody);
			break;
		}
		/* ignore the contents inside TOC (state == 1) */
		if (tocsm != 1) {
			btsm = make_toc(line, ftoc, btsm);
		}
	}

	if (fname == NULL) {
		fin = stdout;
	} else {
		rewind(fin);
	}
	switch (flag) {
	case 0:		/* only print the TOC part */
		file_merge(stdout, NULL, ftoc, NULL);
		break;
	case 1:		/* print the full markdown page */
		file_merge(stdout, fhead, ftoc, fbody);
		break;
	case 2:		/* overwrite the orignal markdown page */
		file_merge(fin, fhead, ftoc, fbody);
		break;
	}
	if (fname) {
		fclose(fin);
	}
	return 0;
}		

static int file_merge(FILE *fout, FILE *fhead, FILE *ftoc, FILE *fbody)
{
	if ((fbody == NULL) || (ftell(fbody) == 0)) {
		/* if there's no TOC mark, the TOC would be outputed to the head */
		if (ftoc) {
			fputs("<!--toc-->\n", fout);
			fputs("## Table of Contents\n", fout);
			file_append(fout, ftoc);
			fputs("<!--toc-->\n", fout);
		}
		if (fhead) {
			file_append(fout, fhead);
		}
		if (fbody) {
			fclose(fbody);
		}
	} else {
		/* if there's no TOC mark, we just fill in the TOC content */
		if (fhead) {
			file_append(fout, fhead);
		}
		if (ftoc) {
			fputs("## Table of Contents\n", fout);
			file_append(fout, ftoc);
		}
		if (fbody) {
			fclose(fbody);
		}
	}
	return 0;
}

static int file_append(FILE *fp, FILE *fn)
{
	char	line[MAX_LINE_LENGTH];
	
	rewind(fn);
	while (fgets(line, sizeof(line), fn)) {
		fputs(line, fp);
	}
	fclose(fn);
	return 0;
}

#if 0
static int make_toc(FILE *fin, FILE *fout)
{
	char	*s, line[MAX_LINE_LENGTH];
	int	backtick = 0;

	while (fgets(line, sizeof(line), fin)) { 
		if (backtick == 0) {
			backtick = lets_toc(line, fout);
		} else {
			s = backtick_match(line, backtick);
			if (s) {
				backtick = backtick_conclude(s);
			}
		}
	}
	return 0;
}
#endif
static int make_toc(char *line, FILE *fout, int backtick)
{
	char	*s;

	if (backtick == 0) {
		backtick = lets_toc(line, fout);
	} else if ((s = backtick_match(line, backtick)) != NULL) {
		backtick = backtick_conclude(s);
	}
	return backtick;
}


static int lets_toc(char *line, FILE *fout)
{
	int	i;

	for (i = 0; i < 4; i++, line++) {
		if (isspace(*line)) {
			continue;	/* heading text allow at most 3 spaces */
		} else if (*line == '#') {
			print_toc(line, fout);
			break;
		} else {
			break;	/* not a heading text */
		}
	}
	/* lets try find some backticks */
	return backtick_conclude(line);
}


/* 's' points to the first validated '#' */
static int print_toc(char *s, FILE *fout)
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
	fprintf(fout, "%*s- [%s](#%s)\n", (n - 1) * 2, "", s, anchor);
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
		} else {
			n = 0;	/* reset after matched */
		}
	}
	return n;
}

