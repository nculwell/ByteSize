#!/bin/sh

SOURCES="main.c lexer.c parser.c interp.c builtins.c"
gcc -o ByteSize --std=c99 -Wall -Werror $SOURCES

