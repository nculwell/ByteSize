#!/bin/sh

SOURCES="alloc.c main.c lexer.c parser.c interp.c builtins.c"
ALLOWED='--std=c99 -Wall -Werror'
OPT='-O0 -g'
if [ "$1" == "opt" ]; then
  OPT='-O2'
fi
gcc -o ByteSize $ALLOWED $OPT $SOURCES

