#!/bin/sh

sec=4
dir=../../../man/man$sec/

for name do
  bname=`basename $name`;\
  echo $bname;\
  rm $name;\
  ln -s $dir$bname.$sec $name;\
done
