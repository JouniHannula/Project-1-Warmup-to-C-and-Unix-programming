/********************************************************************
 *
 * Build:  gcc -std=c99 -Wall -Wextra -o reverse reverse.c
 *
 * Jouni Hannula
 *******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* ---------- helpers ------------------------------------------------ */

static void die(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

/* read one arbitrarily long line from FILE *fp
 * Returns a malloc‑allocated, NUL‑terminated string (without chopping '\n').
 * Returns NULL on EOF (after closing newline already read) or on error.
 */
static char *read_line(FILE *fp)
{
    const size_t CHUNK = 256;
    size_t cap = CHUNK, len = 0;
    int c;

    char *buf = malloc(cap);
    if (!buf) die("malloc failed");

    while ((c = fgetc(fp)) != EOF) {
        if (len + 1 >= cap) {            /* +1 for NUL */
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) die("malloc failed");
            buf = tmp;
        }
        buf[len++] = (char)c;
        if (c == '\n')
            break;
    }

    if (len == 0 && c == EOF) {          /* immediate EOF */
        free(buf);
        return NULL;
    }

    buf[len] = '\0';
    return buf;
}

/* ---------- main --------------------------------------------------- */

int main(int argc, char *argv[])
{
    /* 1. argument parsing */
    if (argc > 3)
        die("usage: reverse <input> <output>");

    FILE *in  = stdin;
    FILE *out = stdout;

    if (argc >= 2) {                       /* open input file */
        in = fopen(argv[1], "r");
        if (!in) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
    }

    if (argc == 3) {                       /* open output file */
        if (strcmp(argv[1], argv[2]) == 0) {
            if (in != stdin) fclose(in);
            die("Input and output file must differ");
        }
        out = fopen(argv[2], "w");
        if (!out) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[2]);
            if (in != stdin) fclose(in);
            exit(1);
        }
    }

    /* 2. read all lines into dynamic array */
    size_t cap = 1024, count = 0;
    char **lines = malloc(cap * sizeof(*lines));
    if (!lines) die("malloc failed");

    char *line;
    while ((line = read_line(in)) != NULL) {
        if (count == cap) {
            cap *= 2;
            char **tmp = realloc(lines, cap * sizeof(*lines));
            if (!tmp) die("malloc failed");
            lines = tmp;
        }
        lines[count++] = line;
    }

    /* 3. print in reverse order */
    for (size_t i = count; i-- > 0; ) {
        fputs(lines[i], out);
        free(lines[i]);
    }

    /* 4. cleanup */
    free(lines);
    if (in  != stdin)  fclose(in);
    if (out != stdout) fclose(out);
    return 0;
}