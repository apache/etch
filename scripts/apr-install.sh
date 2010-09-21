#!/bin/bash

# set install prefix
export set INSTALL_PREFIX=/home/fitzner/Projekte/2010_apache/etch/external

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

build_apr-iconv(){
cd apr/apr-iconv
mkdir linux
cd linux
../configure --prefix=$INSTALL_PREFIX/apr/1.3.12/ --with-apr=$INSTALL_PREFIX/apr/1.3.12/ CC="gcc"
make
make install
}

download
build_apr
build_apr-iconv




