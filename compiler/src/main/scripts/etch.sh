#!/bin/sh
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements. See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership. The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License. You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied. See the License for the
# specific language governing permissions and limitations
# under the License.

#
# Run Etch compiler.
#

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

# Compute ETCH Home
export ETCH_HOME=`dirname "$0"`/..

# Compute classpath for compiler
ETCH_CP=
for x in `cd "${ETCH_HOME}"; echo lib/apache-etch-compiler-*.jar` ; do
    ETCH_CP="${ETCH_CP}:${ETCH_HOME}/$x"
done

# Add clover if installed ..
if [ -f "${CLOVER_HOME}/lib/clover.jar" ]; then
    ETCH_CP="${ETCH_CP}:${CLOVER_HOME}/lib/clover.jar"
fi

# Invoke Etch
exec "$JAVA_CMD" -cp "${ETCH_CP}" "-Detch.home=${ETCH_HOME}" org.apache.etch.compiler.EtchMain ${1+"$@"}
