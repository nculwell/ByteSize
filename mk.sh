#!/bin/sh

SOURCES="main.c lexer.c parser.c interp.c"
gcc -o ByteSize --std=c99 -Wall -Werror $SOURCES

