#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "config.h"

static void append(int, char *);
static void last(char *);
static void edit(char *);
static void usage(void);

void
append(int d, char *fullpath)
{
	FILE *fp;
	FILE *read;
	char *line = NULL;
	char timebuf[100];
	size_t len = 0;
	time_t t;

	switch(d) {
	case 0:
		fputs("Enter note: ", stdout);
		if (getline(&line, &len, stdin) <= 1) {
			puts("\nNothing entered, exiting.");
			free(line);
			exit(0);
		}
		break;
	case 1:
		if (!(read = popen(dmenucmd, "r"))) {
			perror("popen");
			exit(1);
		} else if (getline(&line, &len, read) <= 1) {
			pclose(read);
			free(line);
			exit(0);
		} else if (pclose(read) < 0) {
			perror("pclose");
			exit(1);
		}
		break;
	default: abort();
	}

	if (!(fp = fopen(fullpath, "a"))) {
		perror("fopen");
		exit(1);
	}

	t = time(NULL);
	if (!strftime(timebuf, sizeof(timebuf), TIMEFORMAT, localtime(&t))) {
		fputs("strftime: result string exceeds buffer size\n", stderr);
		exit(1);
	}

	fprintf(fp, "[%s] %s\n", timebuf, line);

	free(line);
	fclose(fp);
}

void
last(char *fullpath)
{
	FILE *fp;
	int count = 0;
	long int pos;
	char s[100];

	if (!(fp = fopen(fullpath, "r"))) {
		perror("fopen");
		exit(1);
	}

	fseek(fp, 0, SEEK_END);
	pos = ftell(fp);

	while (1) {
		fseek(fp, --pos, SEEK_SET);
		if (!pos)
			break;
		else if ((fgetc(fp) == '\n') && (count++ == 20))
			break;
	}

	while (fgets(s, sizeof(s), fp) != NULL) {
		printf("%s", s);
	}

	fclose(fp);
}

void
edit(char *fullpath)
{
	char *editor;

	if ((editor = getenv("EDITOR")) == NULL) {
		editor = "vi";
	}

	char *cmd[] = { editor, fullpath, NULL };
	execvp(cmd[0], cmd);
}

void
usage(void)
{
	fputs("usage: note [option]\n"
		"  -a    append to the main file\n"
		"  -d    append using dmenu\n"
		"  -s    show last 10 notes\n"
		"  -e    edit the main file\n", stderr);
	exit(1);
}

int
main(int argc, char *argv[])
{
	int opt;
	char fullpath[100];

	if (argc != 2) {
		usage();
	}

	snprintf(fullpath, sizeof(fullpath), "%s/%s", getenv("HOME"), filepath);

	while ((opt = getopt(argc, argv, "adse")) != -1) {
		switch (opt) {
		case 'a':
			append(0, fullpath);
			break;
		case 'd':
			append(1, fullpath);
			break;
		case 's':
			last(fullpath);
			break;
		case 'e':
			edit(fullpath);
			break;
		default:
			usage();
		}
	}

	return 0;
}
