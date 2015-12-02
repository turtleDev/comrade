#!/bin/sh

if [ -e ./_comrade ]; then
    ./_comrade & 
else
    _comrade & 
fi
