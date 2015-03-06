jwalk - JSON Walker
===================

Print matching values from a JSON input for a given path expression.

Jwalk extracts JSON values (literals, strings and array values) from standard input or a named file, matching a given expression. Matched values are printed to standard output concatenated by a character delimiter, using new line by default.

[![Build Status](https://travis-ci.org/mfornos/jwalk.svg?branch=master)](https://travis-ci.org/mfornos/jwalk)

Examples
--------

Given the following JSON document:

```
```

### Extract values for properties named "my_prop"

```
```

Concatenate the values with underscores.

```
```

### Extract all values matching the path "a.b.c.my_prop"

```
```

### Inspect the JSON structure

```
```

Usage
-----

```
Usage:
    jwalk [--inspect] [--delimiter <char>] [--file <file_name>] <expression>
Examples:
    jwalk -i < file.json
    jwalk some.path.keys < file.json
    jwalk "**.name" < file.json
    gzcat big.json.gz | jwalk "root.*.name"
Options:
    -i, --inspect
     Sets inspection mode. This mode dumps the JSON keys as a nested list, expression is not required.
    -d <char>, --delimiter <char>
     The <char> delimiter that separates matched values; e.g.: -d $'\t'
    -f <file_name>, --file <file_name>
     The name of a file containing JSON text
    <expression>
     A path to traverse the JSON tree; e.g.: house.room.name, "**.name", "a.*.c"
Notes:
    'stdin' is the default input source
```

Expressions
-----------

Jwalk supports simple expressions with partial globbing to extract data from a JSON structure.

An expression specifies a path through a JSON object hierarchy. The path is composed by the name of each JSON property to traverse. To specify a path, each property along the route from the root object to the property that holds the target value must be included in the path, with each property in the sequence being separated by a dot.

Side note, there is no aim to support JSON-Path nor XPath syntax. The idea is to keep extraction straightforward, as simple as possible, aligned with an evolving set of use cases.


Building from source
--------------------

[Autotools](http://www.gnu.org/software/automake/manual/html_node/index.html#Top) are used to ease the building process.

1. Clone jwalk git repository: `git clone https://github.com/mfornos/jwalk.git`
2. Run `./autogen.sh`
3. Generate make files suited for your system:
     `./configure`
4. Compile:
     `make`
5. Install jwalk:
     `sudo make install`

That's all.


Curiosities & References
------------------------

* [JSON specification](http://json.org/)
* [J Walk @Urban dictionary](http://www.urbandictionary.com/define.php?term=j+walk)

