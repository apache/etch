#!/bin/sh
#
# etch.sh
#
# Run Etch compiler.
#
#  Copyright 2007 - Cisco, Inc.

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
${JAVA_CMD} -version 2>&1 | grep -q "1.6"
if [ $? -ne 0 ]; then
    echo "Java version must be at least 1.6"
    exit 1
fi


# Compute ETCH Home
ETCH_HOME=`dirname $0`/..
ETCH_JAR_PATH=${ETCH_HOME}/bin/etch.jar
if [ ! -f "${ETCH_JAR_PATH}" ]; then
    echo "Cannot find etch.jar"
    exit 1
fi

# Invoke Etch
exec $JAVA_CMD -jar "${ETCH_JAR_PATH}" ${1+"$@"}
