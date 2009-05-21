#!/bin/sh

eol=`svn pg svn:eol-style $1`
if [ $? -ne 0 ]; then
  exit 1
fi

cr=$'\r'

#echo "x${eol}x" | od -c
#echo "xnative${cr}x" | od -c

if [ "x${eol}x" = "xnative${cr}x" ]; then
  exit 0
fi

if [ "x${eol}x" = "xnativex" ]; then
  exit 0
fi

exit 1
