#!/usr/bin/env bash

function fail () {
    #rm $check
    exit -1
}

function skip () {
    exit 77
}

function assert () {
  cmp --silent $expect $given || fail
  rm $given
  exit 0
}

function sample () {
    ../src/jwalk "$1" < assets/inputs/"$2" >> $given
}
