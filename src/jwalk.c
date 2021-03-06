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
*/
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "jwalk.h"

#define PATH_DELIM "."

static json_walker_t js;

static void json_parse_value();
static void json_parse_object();
static void json_dump_keys();
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
 * \param c the character to be unread.
 */
static void json_unread(char c)
{
    ungetc(c, js.in);
}

/*! \brief Prints the given character.
 *
 * Prints the given character to the selected stream.
 *
 * \param c the character to be printed.
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
            /* Objects */
            json_unread(c);
            json_parse_object();
            return;

        case ':':
        case ' ':
        case '\r':
        case '\n':
        case '\t':
            /* Ignore */
            break;

        case '[':
            /* Arrays */
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
            js.key_func();
            break;
        default:
            break;
        }
    }
}

/*! \brief Skips input until array termination.
 *
 */
static void json_skip_array()
{
    char c;
    while ((c = json_getc()) != EOF) {
        if (c == ']') {
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
        switch (c) {
        case '[':
            json_skip_array();
            break;
        case '}':
            json_unread(c);
            return;
        }
    }
}

/*! \brief Skips input until string termination.
 *
 */
static void json_skip_string()
{
    int scape = 0;
    char c;
    while ((c = json_getc()) != EOF) {
        if (!scape && c == '\"') {
            break;
        }
        scape = c == '\\';
    }
}

/*! \brief Skips json values, wildly.
 *
 */
static void json_skip_value()
{
    char c;
    while ((c = json_getc()) != EOF) {
        switch (c) {
        case '\"':
            json_skip_string();
            break;
        case ',':
        case ']':
            return;
        case '{':
            js.depth++;
            return;
        case '}':
            js.depth--;
            return;
        default:
            break;
        }
    }
}

/*! \brief Prints the depth level as Markdown indentation.
 *
 */
static void json_indent_level()
{
    for (int i = 0; i < js.depth - 1; i++) {
        json_putc(' ');
        json_putc(' ');
    }

    char num[(sizeof(int) * CHAR_BIT - 1) / 3 + 3];
    sprintf(num, "%d", js.depth);
    int n = strlen(num);
    for (int i = 0; i < n; i++) {
        json_putc(num[i]);
    }
    json_putc('.');
    json_putc(' ');
}

/*! \brief Dumps JSON keys.
 *
 * Expects a stream pointer positioned at the begining of a property key.
 *
 */
static void json_dump_keys()
{
    char c;
    int scape = 0;

    if (js.count > 0) {
        json_putc('\n');
    }
    json_indent_level();

    while ((c = json_getc()) != EOF) {
        if (!scape && c == '\"') {
            js.count++;
            break;
        } else {
            json_putc(c);
        }
        scape = c == '\\';
    }

    while ((c = json_getc()) != EOF) {
        switch (c) {
        case ':':
            json_skip_value();
            break;

        case '\"':
            json_dump_keys();
            break;

        case '{':
            js.depth++;
            break;

        case '}':
            js.depth--;
            break;

        default:
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
    int depth = (js.offset < js.depth) ? js.offset : js.depth;

    if (js.depth < 1
        || js.levels < depth
        || (depth > 1 && !js.matches[depth - 2])) {
        json_skip_obj();
        return;
    }

    const char *target = js.path[depth - 1];
    int tlen = strlen(target);
    char *name = (char *)malloc(tlen);
    int all = 0;

    if (tlen == 2 && target[0] == '*' && target[1] == '*') {
        all = depth == js.levels;

        if (!all) {
            js.offset = depth + 1;
            js.matches[depth - 1] = 1;
            json_skip_string();
            return;
        } else {
            js.offset = depth;
            js.matches[depth] = 1;
        }
    }

    char c;
    int i = 0;
    int scape = 0;

    while ((c = json_getc()) != EOF) {
        if (!scape && c == '\"') {
            if (all || (tlen == 1 && target[0] == '*')
                || (i == tlen && strncmp(name, target, tlen) == 0)) {

                if (!all) {
                    js.matches[depth - 1] = 1;
                }
                if (js.levels == depth) {
                    json_parse_value();
                } else if (js.offset != INT_MAX) {
                    js.offset = depth + 1;
                }
            } else {
                js.matches[depth - 1] = 0;
            }
            break;
        } else if (i < tlen) {
            name[i] = c;
        }
        scape = c == '\\';
        i++;
    }

    free(name);
}

/*! \brief Parses a path expression.
 *
 * Tokenizes the given path and initializes the path levels array.
 *
 * \param path the path expression
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
 * Initializes the json_walker structure for a given expression path.
 *
 * \param path the path expression.
 */
static void json_init(const char *path)
{
    if (path != NULL) {
        init_json_path(path);
        js.matches = (int *)malloc(sizeof(int) * js.levels);
    }
    js.out = stdout;
    js.key_func = &json_match;
    js.offset = INT_MAX;
}

/*! \brief Parses a JSON input stream.
 *
 * \param filename the file name containing the JSON data. If NULL standard input will be used.
 * \return the number of printed items.
 */
static int json_parse_stream(const char *filename)
{
    FILE *file = NULL;

    if (filename == NULL) {
        file = stdin;
    } else {
        file = fopen(filename, "r");
    }

    if (file != NULL) {

        js.in = file;
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

/*! \brief Gets the underlying JSON walker.
 *
 * \sa jwalk.h for details.
 */
json_walker_t json_walker()
{
    return js;
}

/*! \brief Prints the JSON keys as a nested list.
 *
 * \sa jwalk.h for details.
 */
int json_inspect(const char *filename)
{
    json_init(NULL);
    js.key_func = &json_dump_keys;
    return json_parse_stream(filename);
}

/*! \brief Prints matching values from a JSON input for a given path expression.
 *
 * \sa jwalk.h for details.
 */
int json_walk(const char *filename, const char *path_expr, char delimiter)
{
    json_init(path_expr);
    js.delimiter = delimiter;
    return json_parse_stream(filename);
}
