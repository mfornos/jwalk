/*
    This file is part of JWalk.

    JWalk is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    JWalk is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with JWalk.  If not, see <http://www.gnu.org/licenses/>.
*/
/*! \file jwalk.c
    \brief The jwalk implementation code.

    JWalk extracts JSON values (literals, strings and array values) from standard input or a named file,
    matching a given expression. Matched values are printed to standard output concatenated by a character
    delimiter, using new line by default.
*/
#include <string.h>
#include <stdlib.h>

#include "jwalk.h"

#define PATH_DELIM "."

static json_walker_t js;

static void json_parse_value();
static void json_parse_object();
static void json_match();

/*! \brief Reads a character.
 *
 * Reads a character from the underlying input stream.
 *
 * \return the next character in the stream.
 */
static char json_getc()
{
    return fgetc(js.in);
}

/*! \brief Unreads the given character.
 *
 * Puts the given character in the input stream for
 * reprocessing.
 *
 * \param char c the character to be unread.
 */
static void json_unread(char c)
{
    ungetc(c, js.in);
}

/*! \brief Prints the given character.
 *
 * Prints the given character to the selected stream.
 *
 * \param char c the character to be printed.
 */
static void json_putc(char c)
{
    putc(c, js.out);
}

/*! \brief Prints the delimiter.
 *
 * Prints the delimiter if at least one value was printed before.
 */
static void json_putdelim()
{
    if (js.count > 0) {
        json_putc(js.delimiter);
    }
}

/*! \brief Parses JSON string.
 *
 * Extracts string values without quotes.
 */
static void json_parse_string()
{
    char c;
    while ((c = json_getc()) != EOF) {
        if (c == '\"') {
            break;
        } else {
            json_putc(c);
        }
    }
}

/*! \brief Parses JSON literals.
 *
 * Extracts boolean, numeric and, generally, anything unquoted.
 */
static void json_parse_literal()
{
    char c;
    while ((c = json_getc()) != EOF) {
        switch (c) {
        case ' ':
        case '\r':
        case '\n':
        case '\t':
        case ',':
        case '}':
        case ']':
            json_unread(c);
            return;
        default:
            json_putc(c);
            break;
        }
    }

}

/*! \brief Parses JSON values inside an array.
 *
 */
static void json_parse_array()
{
    char c;
    while ((c = json_getc()) != EOF) {
        if (c == ']') {
            break;
        } else {
            json_parse_value();
        }
    }
}

/*! \brief Parses a JSON value.
 *
 * Signals the extraction of values by type. If an object is found
 * a recursive call to json_parse_object is done.
 */
static void json_parse_value()
{
    char c;
    while ((c = json_getc()) != EOF) {
        switch (c) {
        case '{':
            json_unread(c);
            json_parse_object();
            return;

        case ':':
        case ' ':
        case '\r':
        case '\n':
        case '\t':
            break;

        case '[':
            json_unread(c);
            json_parse_array();
            return;

        case '\"':
            /* Strings */
            json_putdelim();
            json_parse_string();
            js.count++;
            return;

        default:
            /* Literals */
            json_unread(c);
            json_putdelim();
            json_parse_literal();
            js.count++;
            return;
        }
    }
}

/*! \brief Parses a JSON object structure.
 *
 * When the first opening quote is detected json_match will be called.
 */
static void json_parse_object()
{
    char c;
    while ((c = json_getc()) != EOF) {
        switch (c) {
        case '{':
            js.depth++;
            break;
        case '}':
            js.depth--;
            break;
        case '\"':
            json_match();
            break;
        default:
            break;
        }
    }
}

/*! \brief Skips input until object termination.
 *
 */
static void json_skip_obj()
{
    char c;
    while ((c = json_getc()) != EOF) {
        if (c == '}') {
            json_unread(c);
            break;
        }
    }
}

/*! \brief Performs the property matching.
 *
 * Matches the current path level token against a JSON property name.
 *
 */
static void json_match()
{
    if (js.depth < 1 || js.levels < js.depth || (js.depth > 1 && !js.matches[js.depth - 2])) {
        json_skip_obj();
        return;
    }
    const char *target = js.path[js.depth - 1];
    int tlen = strlen(target);
    char *name = (char *)malloc(tlen);
    char c;
    int i = 0;

    while ((c = json_getc()) != EOF) {
        if (c == '\"') {
            if (target[0] == '*'
                || (i == tlen && strncmp(name, target, tlen) == 0)) {
                js.matches[js.depth - 1] = 1;
                if (js.levels == js.depth) {
                    json_parse_value();
                }
            } else {
                js.matches[js.depth - 1] = 0;
            }
            break;
        } else if (i < tlen) {
            name[i] = c;
        }
        i++;
    }

    free(name);
}

/*! \brief Parses a path expression.
 *
 * Tokenizes the given path and initializes the path levels array.
 *
 * \param the path expression
 */
static void init_json_path(const char *path)
{
    char *token;
    js.path = (char **)malloc(sizeof(char *));

    char *s = (char *)malloc(strlen(path) + 1);
    strcpy(s, path);

    token = strtok(s, PATH_DELIM);
    if (token != NULL) {
        js.path[0] = malloc(strlen(token) + 1);
        strcpy(js.path[0], token);
    }
    int i = 1;
    while ((token = strtok(NULL, PATH_DELIM)) != NULL) {
        js.path = (char **)realloc(js.path, sizeof(char *) * (i + 1));
        js.path[i] = malloc(strlen(token) + 1);
        strcpy(js.path[i], token);
        i++;
    }
    free(s);

    js.levels = i;
}

/*! \brief Releases JSON walk allocated resources.
 *
 * Releases dynamic allocated paths and matches memory.
 */
static void json_free()
{
    for (int i = 0; i < js.levels; i++) {
        free(js.path[i]);
    }
    free(js.path);
    free(js.matches);
}

/*! \brief Initializes JSON walk.
 *
 * Initializes the structure for a given expression path and
 * stream. Sets new line as default delimiter.
 *
 * \param char* path the path expression.
 * \param FILE* the input stream.
 */
static void json_init(const char *path, FILE * stream)
{
    init_json_path(path);
    js.delimiter = '\n';
    js.in = stream;
    js.out = stdout;
    js.matches = (int *)malloc(sizeof(int) * js.levels);
}

/*! \brief Gets the underlying JSON walker.
 *
 * \sa jwalk.h for details.
 */
json_walker_t json_walker()
{
    return js;
}

/*! \brief Prints matching values from a JSON input for a given path expression.
 *
 * \sa jwalk.h for details.
 */
int json_walk(const char *filename, char delimiter, const char *path_expr)
{
    FILE *file = NULL;

    if (filename == NULL) {
        file = stdin;
    } else {
        file = fopen(filename, "r");
    }

    if (file != NULL) {
        json_init(path_expr, file);
        js.delimiter = delimiter;
        json_parse_object();
        json_free();

        if (filename != NULL) {
            fclose(file);
        }
    } else {
        perror("The following error occurred");
        exit(EXIT_FAILURE);
    }

    return js.count;
}
