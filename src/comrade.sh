#!/bin/sh

if [ -e ./_comrade ]; then
    ./_comrade $@ & 2> /dev/null
else
    _comrade $@ & 2> /dev/null
fi
