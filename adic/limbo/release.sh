#!/bin/bash

#configuration
BUILDDIR=/tmp
#attention it is assumed dope used the same prefix
PREFIX=/tmp/usr
export ACLOCAL_FLAGS="-I config/m4"
export CXX=g++-3.2
export CXXFLAGS="-Wall -ansi -pedantic -Wno-long-long -Os"
PACKAGE=adic
CONFIGURE_OPTIONS="--with-dope-prefix=$PREFIX"
export ADIC_DEB_CONFIGURE_OPTIONS=$CONFIGURE_OPTIONS
#end of configuration

set -e

cd $BUILDDIR

#check that $PACKAGE directory does not already exist
if test -e $PACKAGE; then
	echo Warning $PACKAGE directory already exists - abort or I will remove it
	read
	rm -rf $PACKAGE
fi

#get source
cvs -d:pserver:anonymous:@cvs.$PACKAGE.berlios.de:/cvsroot/$PACKAGE login
cvs -z3 -d:pserver:anonymous:@cvs.$PACKAGE.berlios.de:/cvsroot/$PACKAGE co $PACKAGE

#build distribution tarball
cd $PACKAGE
./bootstrap.sh
./configure $CONFIGURE_OPTIONS
make dist
DISTFILE=`ls -1 $PACKAGE-*.tar.gz`
DIR=${DISTFILE%.tar.gz}
if [ "x$DIR" = "x" ]; then
    echo Fatal error
    exit 1
fi
cd $BUILDDIR

if test -e $DIR; then
	echo Warning $DIR directory already exists - abort or I will remove it
	read
	rm -rf $DIR
fi
tar xzvf $PACKAGE/$DISTFILE
cd $DIR
dpkg-buildpackage -rfakeroot
