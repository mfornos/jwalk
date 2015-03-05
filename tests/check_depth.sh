#!/usr/bin/env bash

sample=assets/outputs/2.out
check=assets/outputs/2.out.$$

function fail () {
	#rm $check
	exit -1
}

function skip () {
	exit 77
}

../src/jwalk "**.useJSP" < assets/inputs/servlet.json > $check
../src/jwalk "**.servlet.*.useJSP" < assets/inputs/servlet.json >> $check
../src/jwalk "**.servlet.**.useJSP" < assets/inputs/servlet.json >> $check
../src/jwalk "**" < assets/inputs/servlet.json >> $check
../src/jwalk "**.name" < assets/inputs/widgets.json >> $check
../src/jwalk "**.c.**.e" < assets/inputs/rare.json >> $check
../src/jwalk "**.e" < assets/inputs/rare.json >> $check
../src/jwalk "**.z.**.e" < assets/inputs/rare.json >> $check
../src/jwalk "**.b.c.d.e" < assets/inputs/rare.json >> $check
../src/jwalk '*.\"1\"2\"3\"' < assets/inputs/rare.json >> $check
../src/jwalk '**' < assets/inputs/rare.json >> $check

cmp --silent $sample $check || fail

rm $check
exit 0