#!/bin/sh
# mklinks.sh <section name>
#
section=$1
echo Moving man pages to man$sections...

find . -name "*.$section" -exec setuplink.sh $section {} \;
echo Done.
