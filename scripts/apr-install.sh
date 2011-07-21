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
mkdir apr
cd apr
# apr
svn co http://svn.apache.org/repos/asf/apr/apr/tags/1.4.2/

cd ..

mkdir apr-iconv
cd apr-iconv
# apr-iconv
svn co http://svn.apache.org/repos/asf/apr/apr-iconv/tags/1.2.1/

cd ..
cd ..
}

#
# build and patch apr sources
#
build_apr(){
# build apr
cd apr/apr/1.4.2
wget -O broadcast_patch.patch https://issues.apache.org/bugzilla/attachment.cgi?id=26781
patch -p0 -i broadcast_patch.patch
./buildconf
mkdir linux
cd linux
../configure --prefix=$INSTALL_PREFIX/apr/1.4.2-with-broadcast-patch CC="gcc"
make
make install
cd ../../../..
}

build_apr_iconv(){
cd apr/apr-iconv/1.2.1
./buildconf --with-apr=../../apr/1.4.2/
mkdir linux
cd linux
../configure --prefix=$INSTALL_PREFIX/apr/1.4.2-with-broadcast-patch --with-apr=$INSTALL_PREFIX/apr/1.4.2-with-broadcast-patch CC="gcc"
make
make install
}

download
build_apr
build_apr_iconv




