#!/usr/local/bin/bash

#set -x
set -e

#echo etch $*
#exit

# Copyright 2008 Cisco Systems
# This script is for the support of the various launch configurations
# used with eclipse.

if test -z "$ETCH_DEPENDENT_JARS"; then
echo Environment variable ETCH_DEPENDENT_JARS not set.
exit 1
fi

ETCH_HOME=`dirname "$0"`/..

if test \! -d "$ETCH_HOME/bin/etch"; then
#echo Cannot find etch classes.
exit 1
fi

#echo java -cp "$ETCH_HOME/bin:$ETCH_DEPENDENT_JARS" etch.compiler.EtchMain ${1+"$@"}
echo etch ${1+"$@"}
java -cp "$ETCH_HOME/bin:$ETCH_DEPENDENT_JARS" etch.compiler.EtchMain ${1+"$@"}
