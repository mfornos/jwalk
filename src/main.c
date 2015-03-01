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
/*! \file main.c
    \brief The jwalk commandline interface.

    JWalk extracts JSON values (literals, strings and array values) from standard input or a named file,
    matching a given expression. Matched values are printed to standard output concatenated by a character
    delimiter, using new line by default.
*/
#include <stdlib.h>
#include <getopt.h>

#include "jwalk.h"

#define MIN_ARGS_REQUIRED 0


void usage(void)
{
    puts("Usage:\n"
    "\tjwalk [--delimiter <char>] [--file <file_name>] <expression>\n"
         "Examples:\n"
         "\tjwalk \"*.name\" < file.json\n"
         "\tgzcat big.json.gz | ./src/jwalk \"*.name\"\n"
         "Options:\n"
         "\t-d <char>, --delimiter <char>\n\t The <char> delimiter that separates collected values; e.g.: -d $'\\t'\n"
         "\t-f <file_name>, --file <file_name>\n\t The file name of a file containing JSON text\n"
         "\t<expression>\n\t A path to traverse the JSON tree; e.g.: house.room.name\n"
         "Notes:\n"
         "\t'stdin' is the default input stream");
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    char *filename = NULL;
    char *path_expr = NULL;
    char delimiter = '\n';
    int option_index = 0;
    int c;

    if (argc < MIN_ARGS_REQUIRED) {
        usage();
    }
    static struct option long_options[] =
    {
        {"help", optional_argument, 0, 'h'},
        {"file", optional_argument, 0, 'f'},
        {"delimiter", optional_argument, 0, 'd'}
    };

    while ((c = getopt_long(argc, argv, "hf:d:",
                long_options, &option_index)) != -1) {

        switch (c) {
        case 'h':
            usage();
        case 'f':
            filename = optarg;
            break;
        case 'd':
            delimiter = optarg[0];
            break;
        default:
            abort();
        }
    }

    if (optind < argc) {
        path_expr = argv[optind++];
    } else {
        usage();
    }

    json_walk(filename, delimiter, path_expr);

    exit(EXIT_SUCCESS);
}
