#!/bin/bash


#configuration
BUILDDIR=/tmp
#attention it is assumed dope uses the same prefix
PREFIX=/tmp/usr
export ACLOCAL_FLAGS="-I config/m4"
export CXX=g++-3.2
export CXXFLAGS="-Wall -ansi -pedantic -Wno-long-long -Os -DNDEBUG"
CONFIGURE_OPTIONS="--with-dope-prefix=$PREFIX"
export ADIC_DEB_CONFIGURE_OPTIONS=$CONFIGURE_OPTIONS
DOPE_BUILD=$BUILDDIR/dope-release.sh
ADIC_DEBREP=/mnt/schlumpf-pub/debian/adic/packages
SETUPCROSS=~/develop/cross/config
ADIC_CROSS_CONFIGURE_OPTIONS="--with-dope-prefix=$PREFIX/cross --disable-shared"
#end of configuration

set -e

#build dope first
PACKAGE=dope
cd $BUILDDIR

#get dope release.sh script
if test -e $DOPE_BUILD; then
	echo Warning $DOPE_BUILD script already exists - abort or I will remove it
	read
	rm $DOPE_BUILD
fi
cvs -d:pserver:anonymous:@cvs.$PACKAGE.berlios.de:/cvsroot/$PACKAGE login
cvs -z3 -d:pserver:anonymous:@cvs.$PACKAGE.berlios.de:/cvsroot/$PACKAGE co -p $PACKAGE/limbo/release.sh >$DOPE_BUILD
chmod u+x $DOPE_BUILD
#backup ACLOCAL_FLAGS
T="$ACLOCAL_FLAGS"
export ACLOCAL_FLAGS=""
$DOPE_BUILD
export ACLOCAL_FLAGS="$T"


#now build adic
PACKAGE=adic

cd $BUILDDIR

#check that $PACKAGE directory does not already exist
if test -e $PACKAGE; then
    echo Warning $PACKAGE directory already exists - assuming we already have a distribution tarball in there
    read
else
    #get source
    cvs -d:pserver:anonymous:@cvs.$PACKAGE.berlios.de:/cvsroot/$PACKAGE login
    cvs -z3 -d:pserver:anonymous:@cvs.$PACKAGE.berlios.de:/cvsroot/$PACKAGE co $PACKAGE

    #build distribution tarball
    cd $PACKAGE
    ./bootstrap.sh
    ./configure $CONFIGURE_OPTIONS
    make dist
fi

cd $BUILDDIR/$PACKAGE
DISTFILE=`ls -1 $PACKAGE-*.tar.gz`
DIR=${DISTFILE%.tar.gz}
if [ "x$DIR" = "x" ]; then
    echo Fatal error
    exit 1
fi

cd $BUILDDIR
if test -e $DIR; then
    echo Warning $DIR directory already exists - assuming we already unpacked distribution
    read
else
    tar xzvf $PACKAGE/$DISTFILE
fi

#build in seperate directory
mkdir -p build
cd build

if false; then

#build debian packages
mkdir -p debian
if test -e $DIR; then
    echo Warning $DIR directory already exists - abort or i will remove it
    read
    rm -rf $DIR
fi
cd $DIR
tar xzvf $PACKAGE/$DISTFILE
cd $DIR
dpkg-buildpackage -rfakeroot

#check debian packages
#lintian $BUILDDIR/*.deb

#copy packages to repository
cp -i $BUILDDIR/adic_* $ADIC_DEBREP
cp -i $BUILDDIR/adic-data_* $ADIC_DEBREP
cd $ADIC_DEBREP/..
./gen-packages.sh
./update.sh

fi

# todo build static (or "semi-static") binary packages for linux/X11
# this builds a "semi-static" adicclient:
#    semi-static because i had trouble with statically linked X libraries
#g++-3.2 -Wall -ansi -pedantic -Wno-long-long -Os -o adicclient gamecommon.o netstream.o adicclient.o -nodefaultlibs  -L/tmp/usr/lib -L/usr/local/lib /tmp/usr/lib/libdope.a /usr/local/lib/libsigc.a /usr/lib/libxml2.a -L/usr/lib -L/usr/X11R6/lib /usr/lib/libSDL_image.a /usr/lib/libjpeg.a /usr/lib/libpng.a /usr/local/lib/libSDL_mixer.a /usr/lib/libSDL.a -lpthread -ldl /usr/lib/libesd.a /usr/lib/libaudiofile.a -lm -lX11 -lXext /usr/local/lib/gcc-lib/i586-pc-linux-gnu/3.2/libstdc++.a -lz -lgcc_s -lc

#cross compile
if test -e $SETUPCROSS; then
    source $SETUPCROSS
    cd $BUILDDIR/build
    mkdir -p arch-cross
    cd arch-cross
    if test -e $DIR; then
	echo Warning $DIR directory already exists - abort or I will remove it
	read
	rm -rf $DIR
    fi
    mkdir $DIR
    cd $DIR
    $BUILDDIR/$DIR/configure $CROSS_CONFIGURE_OPTIONS $ADIC_CROSS_CONFIGURE_OPTIONS --prefix=$PREFIX/cross --enable-fast-compile
    make install
else
    echo cross compiler config script not found
fi

#todo build static binary packages for linux/fbdev