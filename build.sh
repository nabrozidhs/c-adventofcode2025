#!/usr/bin/env bash

cc_warning_options=(-Wextra -Wall -Wpedantic)
cc_skip_checks_options=(-Wno-unused-parameter -Wno-declaration-after-statement -Wno-padded -Wno-unused-function)

# Check is clang
if cc --version 2>&1 | head -n 1 | grep -qi clang; then
  cc_warning_options+=(-Weverything)
  cc_skip_checks_options+=(-Wno-poison-system-directories)
fi


cc "${cc_warning_options[@]}" -Wno-poison-system-directories meta.c -o meta
./meta
rm meta

cc "${cc_warning_options[@]}" "${cc_skip_checks_options[@]}" main.c -o adventofcode
./adventofcode
rm adventofcode