#!/usr/bin/env bash

cc_warning_options="-Weverything -Wextra -Wall -Wpedantic"
cc_skip_checks_options="-Wno-poison-system-directories -Wno-unused-parameter -Wno-declaration-after-statement -Wno-padded -Wno-unused-function"

cc $cc_warning_options -Wno-poison-system-directories meta.c -o meta
./meta
rm meta

cc $cc_warning_options $cc_skip_checks_options main.c -o adventofcode
./adventofcode
rm adventofcode