#!/bin/sh
# setuplink.sh <section> <file>
#
section=$1
name=$2
base=`basename $name`
dir=`echo $name | sed -e "s/\/[^\/]*$/\//"`
plainname=`basename $name .$section`
plainname=`echo $dir$plainname`
newname=`echo ../man$section/$base`
echo Moving $name to $newname
mv $name $newname
ln -s $newname $plainname
