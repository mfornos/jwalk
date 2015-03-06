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
/*! \file jwalk.h
    \brief The jwalk interface.
*/
#include <stdio.h>

#define NAME    "jwalk"
#define VERSION "0.0.1"

/*! \brief JSON Walker structure.
 *
 *  Holds the configuration and state to traverse a JSON object tree.
 */
typedef struct json_walker_s {
    int depth;
    int levels;
    int offset;
    int count;
    char delimiter;
    FILE *in;
    FILE *out;
    char **path;
    int *matches;
} json_walker_t;

/*! \brief Prints matching values from a JSON input for a given path expression.
 *
 * Prints JSON values (literals, strings and array values) matching the given expression.
 * Matched values are printed to standard output concatenated by the specified
 * delimiter.
 *
 * \param filename the file name containing the JSON data. If NULL standard input will be used.
 * \param delimiter the delimiter to concatenate output values.
 * \param path_expr the path traversal expression.
 *
 * \return the number of printed values.
 */
int json_walk(const char *filename, char delimiter, const char *path_expr);

/*! \brief Gets the underlying JSON walker.
 *
 * Gets the underlying JSON walker structure.
 *
 * \return the underlying JSON walker structure.
 */
json_walker_t json_walker();
