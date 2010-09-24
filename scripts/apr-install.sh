#!/bin/bash
# Licensed to the Apache Software Foundation (ASF) under one   *
# or more contributor license agreements.  See the NOTICE file *
# distributed with this work for additional information        *
# regarding copyright ownership.  The ASF licenses this file   *
# to you under the Apache License, Version 2.0 (the            *
# "License"); you may not use this file except in compliance   *
# with the License.  You may obtain a copy of the License at   *
#                                                              *
#   http://www.apache.org/licenses/LICENSE-2.0                 *
#                                                              *
# Unless required by applicable law or agreed to in writing,   *
# software distributed under the License is distributed on an  *
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY       *
# KIND, either express or implied.  See the License for the    *
# specific language governing permissions and limitations      *
# under the License.                                           * 

# set install prefix
if [ $INSTALL_PREFIX = "" ]; then 
    export INSTALL_PREFIX=/tmp
fi
echo using INSTALL_PREFIX: $INSTALL_PREFIX

#
# download apr sources
#
download(){

rm -rf apr
mkdir apr
cd apr
# apr
wget http://apache.abdaal.com/apr/apr-1.3.12.tar.gz
tar xzf apr-1.3.12.tar.gz
mv apr-1.3.12 apr
rm -rf apr-1.3.12.tar.gz

# apr-util
wget http://mirror.serversupportforum.de/apache/apr/apr-util-1.3.9.tar.gz
tar xzf apr-util-1.3.9.tar.gz
mv apr-util-1.3.9 apr-util
rm -rf apr-util-1.3.9.tar.gz

# apr-iconv
wget http://mirror.serversupportforum.de/apache/apr/apr-iconv-1.2.1.tar.gz
tar xzf apr-iconv-1.2.1.tar.gz
mv apr-iconv-1.2.1 apr-iconv
rm -rf apr-iconv-1.2.1.tar.gz

cd ..
}

#
# build apr sources
#
build_apr(){
# build apr
cd apr/apr
mkdir linux
cd linux
../configure --prefix=$INSTALL_PREFIX/apr/1.3.12/ CC="gcc"
make
make install
cd ../../..
}

build_apr_iconv(){
cd apr/apr-iconv
mkdir linux
cd linux
../configure --prefix=$INSTALL_PREFIX/apr/1.3.12/ --with-apr=$INSTALL_PREFIX/apr/1.3.12/ CC="gcc"
make
make install
}

download
build_apr
build_apr_iconv




