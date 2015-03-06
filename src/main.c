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
    \brief The jwalk command-line interface.
*/
#include <stdlib.h>
#include <getopt.h>

#include "jwalk.h"

#define MIN_ARGS_REQUIRED 0

static int version_flag;

void usage()
{
    puts("Usage:\n"
         "\tjwalk [--inspect] [--delimiter <char>] [--file <file_name>] <expression>\n"
         "Examples:\n"
         "\tjwalk -i < file.json\n"
         "\tjwalk some.path.keys < file.json\n"
         "\tjwalk \"**.name\" < file.json\n"
         "\tgzcat big.json.gz | jwalk \"root.*.name\"\n"
         "Options:\n"
         "\t-i, --inspect\n\t Sets inspection mode. This mode dumps the JSON keys as a nested list,"
         " expression is not required.\n"
         "\t-d <char>, --delimiter <char>\n\t The <char> delimiter that separates matched values; e.g.: -d $'\\t'\n"
         "\t-f <file_name>, --file <file_name>\n\t The name of a file containing JSON text\n"
         "\t<expression>\n\t A path to traverse the JSON tree; e.g.: house.room.name, \"**.name\", \"a.*.c\"\n"
         "Notes:\n"
         "\t'stdin' is the default input source");
    exit(EXIT_FAILURE);
}

void version()
{
    printf("%s version %s\n", NAME, VERSION);
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    char *filename = NULL;
    char *path_expr = NULL;
    char delimiter = '\n';
    int option_index = 0;
    int inspect = 0;
    int c;

    if (argc < MIN_ARGS_REQUIRED) {
        usage();
    }
    static struct option long_options[] =
    {
        {"version", no_argument, &version_flag, 1},
        {"help", optional_argument, 0, 'h'},
        {"inspect", optional_argument, 0, 'i'},
        {"file", optional_argument, 0, 'f'},
        {"delimiter", optional_argument, 0, 'd'}
    };

    while ((c = getopt_long(argc, argv, "hif:d:",
                long_options, &option_index)) != -1) {

        switch (c) {
        case 'h':
            usage();
        case 'i':
            inspect = 1;
            break;
        case 'f':
            filename = optarg;
            break;
        case 'd':
            delimiter = optarg[0];
            break;
        }
    }

    if (version_flag) {
        version();
    }
    if (optind < argc) {
        path_expr = argv[optind++];
    } else if (!inspect) {
        usage();
    }
    if (inspect) {
        json_inspect(filename);
    } else {
        json_walk(filename, path_expr, delimiter);
    }

    exit(EXIT_SUCCESS);
}
