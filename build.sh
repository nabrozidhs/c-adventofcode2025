#!/usr/bin/env bash

cc -Wall -Wpedantic meta.c -o meta
./meta
rm meta

cc -Wall -Wpedantic main.c -o adventofcode
./adventofcode
rm adventofcode