#!/bin/sh

name=$1
newname=`basename $1 .nroff`
echo $newname
nroff -man $name | uniq > $newname
