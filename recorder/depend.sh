#!/bin/bash
gcc -MM -MG "$@" | sed -e 's@^\(.*\)\.o:@\1.d \1.o:@'
