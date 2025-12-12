#!/usr/bin/env bash

cc_warning_options="-Weverything -Wextra -Wall -Wpedantic"
cc_skip_checks_options="-Wno-unused-parameter -Wno-declaration-after-statement"

cc $cc_warning_options meta.c -o meta
./meta
rm meta

cc $cc_warning_options $cc_skip_checks_options main.c -o adventofcode
./adventofcode
rm adventofcode