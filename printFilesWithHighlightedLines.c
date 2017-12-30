// https://github.com/nsajko/printFileWithHighlightedLines

// Take as input file and line listings as made by Sam 'f' and '=' commands, display files
// with given lines highlighted (prepended with line number).

#include <ctype.h>  // isdigit
#include <stdio.h>  // fgetc fputc fprintf fscanf
#include <string.h> // strcpy strcmp

enum {
	// File names.
	bufsz = 2048,
};

// TODO: more state-automatish
static char *
getFileName(void) {
	// char c1, c2, c3, c4;
	static char buf[bufsz];
	int i;

	for (i = 0; i < 4; i++) {
		if (fgetc(stdin) < 0) {
			if (i != 0) {
				fprintf(stderr, "%d %s\n", i, "Not a file name line.");
			}
			return NULL;
		}
	}
	for (i = 0;; i++) {
		int rv = fgetc(stdin);
		if (rv == '\n') {
			buf[i] = '\0';
			break;
		}
		if (i == bufsz - 1) {
			fprintf(stderr, "%s\n", "File name too long.");
			return NULL;
		}
		buf[i] = rv;
	}

	return buf;
}

// Parses the first number out of lines of the following form, where only the three numbers
// are arbitrary: 430; #12622,#12635
//
// Returns 0 in the case of a parsing error.
//
// TODO: more state-automatish
static int
getLineNumber(void) {
	int n, rv;
	if (fscanf(stdin, "%6d", &n) <= 0 || n <= 0 || fgetc(stdin) != ';' ||
	    fgetc(stdin) != ' ' || fgetc(stdin) != '#') {
		return 0;
	}
	for (;;) {
		rv = fgetc(stdin);
		if (!isdigit(rv)) {
			break;
		}
	}
	if (rv != ',' || fgetc(stdin) != '#') {
		return 0;
	}
	for (;;) {
		rv = fgetc(stdin);
		if (!isdigit(rv)) {
			break;
		}
	}
	if (rv != '\n') {
		return 0;
	}

	return n;
}

// I had decided I am going to do it in a less buffered way.
/*
static void
printHighlightedLinesFile(FILE *f, int *lines, int i, int j) {}
*/

// Print n lines of a file prepended with tabs, the last line printed being prepended with
// a number and a tab.
static void
printFileLines(FILE *file, int n, int interesting) {
	int i;
	for (i = 1; i <= n; i++) {
		// This may be nicer as a parameter.
		char *nonInteresting = "	";

		if (i == n) {
			fprintf(stdout, "%d	", interesting);
		} else {
			fprintf(stdout, "%s", nonInteresting);
		}
		for (;;) {
			int rv = fgetc(file);
			if (rv == EOF) {
				if (i != n) {
					fprintf(stderr, "%d %d %s\n", i, n,
						"End of file too soon.");
				}
				return;
			}
			fputc(rv, stdout);
			if (rv == '\n') {
				break;
			}
		}
	}
}

static void
fileOpenErr(char *s) {
	fprintf(stderr, "%s%s\n", "Could not open ", s);
}

static void
readInputAndPrintFiles(void) {
	static char s1[bufsz];
	char *s2;
	int l, a, b;
	FILE *file;

	s2 = getFileName();
	if (s2 == NULL) {
		return;
	}

	a = getLineNumber();
	if (a == 0) {
		return;
	}
	file = fopen(s2, "r");
	if (file == NULL) {
		fileOpenErr(s2);
		return;
	}
	fprintf(stdout, "- %s\n", s2);
	printFileLines(file, a, a);
	strcpy(s1, s2);

	for (;;) {
		s2 = getFileName();
		if (s2 == NULL) {
			break;
		}
		if (strcmp(s1, s2) != 0) {
			// Another file

			fprintf(stdout, "\n\n");

			a = getLineNumber();
			if (a == 0) {
				break;
			}
			file = fopen(s2, "r");
			if (file == NULL) {
				fileOpenErr(s2);
				break;
			}
			fprintf(stdout, "- %s\n", s2);
			printFileLines(file, a, a);
			strcpy(s1, s2);

			continue;
		}
		b = getLineNumber();
		if (b == 0) {
			break;
		}
		l = b - a;
		a = b;
		if (l < 0) {
			fprintf(stderr, "%s\n", "Negative line number.");
			break;
		}
		printFileLines(file, l, b);
	}
	fclose(file);
}

static void
readInputAndPrintFile(char *s) {
	int l, a, b;
	FILE *file;

	a = getLineNumber();
	if (a == 0) {
		return;
	}

	file = fopen(s, "r");
	if (file == NULL) {
		fileOpenErr(s);
		return;
	}
	fprintf(stdout, "- %s\n", s);
	printFileLines(file, a, a);

	for (;;) {
		b = getLineNumber();
		if (b == 0) {
			break;
		}
		l = b - a;
		a = b;
		if (l < 0) {
			fprintf(stderr, "%s\n", "Negative line number.");
			break;
		}
		printFileLines(file, l, b);
	}
	fprintf(stdout, "\n\n");
	fclose(file);
}

int
main(int argc, char *argv[]) {
	if (argc < 2) {
		readInputAndPrintFiles();
	} else {
		readInputAndPrintFile(argv[1]);
	}
	return 0;
}
