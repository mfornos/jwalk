#!/usr/bin/env bash

. test.sh

given=assets/outputs/2.out.$$
expect=assets/outputs/2.out

sample "**.useJSP" servlet.json
sample "**.servlet.*.useJSP" servlet.json
sample "**.servlet.**.useJSP" servlet.json
sample "**" servlet.json

sample "**.name" widgets.json

sample "**.c.**.e" rare.json
sample "**.e" rare.json
sample "**.z.**.e" rare.json
sample "**.b.c.d.e" rare.json
sample '*.\"1\"2\"3\"' rare.json
sample "**" rare.json

assert
