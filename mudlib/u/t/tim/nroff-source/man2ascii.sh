#!/bin/sh

echo Converting man pages...
find . -name "*.nroff" -maxdepth 1 -exec convert.sh {} \;
echo Done.
