#!/bin/sh
#
# etch.sh
#
# Run Etch compiler.
#
#  Copyright 2007 - Cisco Systems, Inc.

# Find Java
if [ "${JAVA_HOME}" != "" ]; then
    JAVA_CMD="${JAVA_HOME}"/bin/java
else
    JAVA_CMD=`which java`
fi
if [ ! -x "${JAVA_CMD}" ]; then
    echo "Cannot find Java"
    exit 1
fi

# Check Java Version
${JAVA_CMD} -version 2>&1 | grep -q "1.4"
if [ $? -eq 0 ]; then
    echo "Java version must be at least 1.5"
    exit 1
fi

# Compute ETCH Home
export ETCH_HOME=`dirname $0`/..

# Compute classpath for compiler
ETCH_CP=
for x in `/bin/ls ${ETCH_HOME}/lib/etch-compiler*.jar` ; do
    ETCH_CP="${ETCH_CP}:$x"
done

# HACK: add clover if installed ..
if [ -f "${CLOVER_HOME}/lib/clover.jar" ]; then
    ETCH_CP="${ETCH_CP}:${CLOVER_HOME}/lib/clover.jar"
fi

# Invoke Etch
exec $JAVA_CMD -cp "${ETCH_CP}" -Detch.home=${ETCH_HOME} etch.compiler.EtchMain ${1+"$@"}
