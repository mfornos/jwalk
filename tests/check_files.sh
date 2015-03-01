#!/usr/bin/env bash

sample=assets/outputs/1.out
check=assets/outputs/1.out.$$

function fail () {
	rm $check
	exit -1
}

function skip () {
	exit 77
}

../src/jwalk "web-app.servlet.servlet-name" < assets/inputs/servlet.json > $check
../src/jwalk "web-app.*.servlet-name" < assets/inputs/servlet.json >> $check
../src/jwalk "web-app" < assets/inputs/servlet.json >> $check
../src/jwalk "\t" < assets/inputs/servlet.json >> $check
../src/jwalk "web-app.servlet.*" < assets/inputs/servlet.json >> $check
../src/jwalk "simple" < assets/inputs/simple.json >> $check
../src/jwalk "simple.bla.*" < assets/inputs/simple.json >> $check
../src/jwalk "simple.*.bla" < assets/inputs/simple.json >> $check
../src/jwalk "widget.debug" < assets/inputs/widgets.json >> $check
../src/jwalk "widget.deb" < assets/inputs/widgets.json >> $check
../src/jwalk "*" < assets/inputs/widgets.json >> $check
../src/jwalk "" < assets/inputs/widgets.json >> $check
../src/jwalk "widget.*.name" < assets/inputs/widgets.json >> $check
../src/jwalk "widget.window.name" < assets/inputs/widgets.json >> $check
../src/jwalk "widget.window.na" < assets/inputs/widgets.json >> $check

cmp --silent $sample $check || fail

rm $check
exit 0
