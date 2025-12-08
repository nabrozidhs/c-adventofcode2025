#!/usr/bin/env bash

cc -Wall -Wpedantic meta.c -o meta
./meta
rm meta

cc -lm -Wall -Wpedantic main.c -o adventofcode
./adventofcode
rm adventofcode