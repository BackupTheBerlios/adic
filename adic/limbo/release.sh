#!/bin/bash -x

#configuration
BUILDDIR=/tmp/adic
#attention it is assumed dope uses the same prefix
PREFIX=/tmp/usr
export ACLOCAL_FLAGS="-I config/m4"
export CXX=g++-3.2
export CXXFLAGS="-Wall -ansi -pedantic -Wno-long-long -Os -DNDEBUG"
CONFIGURE_OPTIONS="--with-dope-prefix=$PREFIX"
#todo use debian libsdl-* again (i do not use it because of troubles with libpng and debian testing)
#if I do this the sdl stuff must be build after the debian packages again
export ADIC_DEB_CONFIGURE_OPTIONS="$CONFIGURE_OPTIONS --with-sdl-prefix=$PREFIX"
DOPE_BUILD=$BUILDDIR/dope-release.sh
ADIC_DEBREP=/mnt/schlumpf-pub/debian/adic/packages
SETUPCROSS=~/develop/cross/config
ADIC_CROSS_CONFIGURE_OPTIONS="--with-dope-prefix=$PREFIX/cross --disable-shared"
HTTPGET=curl
BUILDDEB="dpkg-buildpackage -rfakeroot -us -uc"
export UPLOADDIR=$BUILDDIR/upload
#end of configuration

set -e

if test -e $BUILDDIR; then
    echo Warning $BUILDDIR directory already exists
    read
else
    mkdir -p $BUILDDIR
fi
cd $BUILDDIR

#build dope first
PACKAGE=dope

#get dope release.sh script
if test -e $DOPE_BUILD; then
	echo Warning $DOPE_BUILD script already exists - i assume this is okay
	read
else
    cvs -d:pserver:anonymous:@cvs.$PACKAGE.berlios.de:/cvsroot/$PACKAGE login
    cvs -z3 -d:pserver:anonymous:@cvs.$PACKAGE.berlios.de:/cvsroot/$PACKAGE co -p $PACKAGE/limbo/release.sh >$DOPE_BUILD
    chmod u+x $DOPE_BUILD
fi
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
    echo Warning `pwd`/$PACKAGE directory already exists - assuming we already have a distribution tarball in there
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
    echo Warning `pwd`/$DIR directory already exists - assuming we already unpacked distribution
    read
else
    tar xzvf $PACKAGE/$DISTFILE
fi

#build in seperate directory
mkdir -p build


# get SDL stuff

cd $BUILDDIR
if test ! -e SDL-1.2.5; then
    $HTTPGET http://www.libsdl.org/release/SDL-1.2.5.tar.gz|tar xzf -
fi
if test ! -e SDL_image-1.2.3; then
    $HTTPGET http://www.libsdl.org/projects/SDL_image/release/SDL_image-1.2.3.tar.gz|tar xzf -
fi
if test ! -e SDL_mixer-1.2.5; then
    $HTTPGET http://www.libsdl.org/projects/SDL_mixer/release/SDL_mixer-1.2.5.tar.gz|tar xzf -
fi

#build SDL stuff
if test -e $PREFIX/bin/sdl-config; then
    echo sdl-config found - assuming it is okay
    read
else
    cd $BUILDDIR/SDL-1.2.5
    ./configure --disable-shared --enable-video-opengl --enable-dlopen --disable-video-ggi --disable-video-svga --disable-video-dummy --disable-diskaudio --disable-nas --disable-video-fbcon --disable-video-directfb --disable-video-x11-xv --disable-dga --disable-video-x11-dga --enable-esd-shared --enable-arts-shared --disable-cdrom --disable-debug --prefix=$PREFIX
    make install
    cd $BUILDDIR/SDL_image-1.2.3
    ./configure --disable-shared --with-sdl-prefix=$PREFIX --prefix=$PREFIX --disable-bmp --disable-gif --disable-jpg --disable-lbm --disable-pcx --disable-pnm --disable-tga --disable-tif --disable-xcf --disable-xpm
    make install
    cd $BUILDDIR/SDL_mixer-1.2.5
    ./configure --disable-shared --with-sdl-prefix=$PREFIX --prefix=$PREFIX --disable-music-cmd --disable-music-midi --disable-music-timidity-midi --disable-music-native-midi --disable-music-native-midi-gpl --disable-music-ogg --disable-music-mp3
    make install
fi
#modify inter-library dependencies
cd $PREFIX/lib
mv libSDL_image.la libSDL_image.la.old
#QPREFIX=`echo $PREFIX|sed 's/\//\\\\\//g'`
sed "s/^dependency_libs=.*\$/dependency_libs=\' \/usr\/lib\/libpng.a\'/" libSDL_image.la.old > libSDL_image.la
mv libSDL_mixer.la libSDL_mixer.la.old
sed "s/^dependency_libs=.*\$/dependency_libs=\'\'/" libSDL_mixer.la.old > libSDL_mixer.la


#build debian packages

mkdir -p $BUILDDIR/build/debian
cd $BUILDDIR/build/debian
if test -e $DIR; then
    echo Warning `pwd`/$DIR directory already exists - assuming we already build the debian package
    read
else
    tar xzvf $BUILDDIR/$PACKAGE/$DISTFILE
    cd $DIR
    $BUILDDEB
    mkdir -p $UPLOADDIR
    mv $BUILDDIR/build/debian/adic_${DIR#adic-*}* $UPLOADDIR
    mv $BUILDDIR/build/debian/adic-data_${DIR#adic-*}* $UPLOADDIR
fi


#build semi-static binary package for linux/x11
#we cannot statically link the client

cd $BUILDDIR/build
mkdir -p semi-static
cd semi-static
if test -e $DIR; then
    echo Warning `pwd`/$DIR directory already exists - assuming we already did compile the semi-static binaries
    read
else
    mkdir $DIR
    cd $DIR
    DISTNAME=adic-bin-linux-i386-semistatic-${DIR#adic-*}
    STPREFIX=$BUILDDIR/build/semi-static/$DISTNAME
    $BUILDDIR/$DIR/configure $CONFIGURE_OPTIONS --with-sdl-prefix=$PREFIX --enable-mostlystatic --enable-fastcompile --prefix=$STPREFIX --bindir=$STPREFIX --datadir=$STPREFIX/data
    make install
    # rearange files
    cd $STPREFIX
    mv data/doc/adic/* .
    mv data/adic d2
    rm -rf data
    mv d2 data
    strip adicclient adicserver adicbot
    #create configuration
    ./adicserver --dump --dataPath=data 2>&1 |xmllint --format - >sconfig.xml
    cat >startserver <<-EOF
	#!/bin/bash
	cd \${0%/*}
	exec ./adicserver --file=sconfig.xml
	EOF
    chmod +x startserver
    ./adicclient --dump --dataPath=data --width=800 --height=600 --quality=3 2>&1 |xmllint --format - >cconfig.xml
    cat >startclient <<-EOF
	#!/bin/bash
	cd \${0%/*}
	exec ./adicclient --file=cconfig.xml
	EOF
    chmod +x startclient
    cd ..
    tar cvf - $DISTNAME|gzip --best > $UPLOADDIR/$DISTNAME.tar.gz
    tar cvf - $DISTNAME|bzip2 --best > $UPLOADDIR/$DISTNAME.tar.bz2
fi

#todo build static binary package for linux/fbdev
#./configure  --disable-alsa --disable-esd --disable-arts --disable-video-x11 --disable-dga --disable-video-dga --disable-video-ggi --disable-video-svga --enable-video-opengl --prefix=/tmp/usr --disable-cdrom --disable-video-dummy --disable-video-x11-xv --disable-diskaudio --disable-nas --enable-dlopen
# todo build static (or "semi-static") binary packages for linux/X11
# this builds a "semi-static" adicclient:
#    semi-static because i had trouble with statically linked X libraries
#g++-3.2 -Wall -ansi -pedantic -Wno-long-long -Os -o adicclient gamecommon.o netstream.o adicclient.o -nodefaultlibs  -L/tmp/usr/lib -L/usr/local/lib /tmp/usr/lib/libdope.a /usr/local/lib/libsigc.a /usr/lib/libxml2.a -L/usr/lib -L/usr/X11R6/lib /usr/lib/libSDL_image.a /usr/lib/libjpeg.a /usr/lib/libpng.a /usr/local/lib/libSDL_mixer.a /usr/lib/libSDL.a -lpthread -ldl /usr/lib/libesd.a /usr/lib/libaudiofile.a -lm -lX11 -lXext /usr/local/lib/gcc-lib/i586-pc-linux-gnu/3.2/libstdc++.a -lz -lgcc_s -lc


#cross compile for win32
if test -e $SETUPCROSS; then
    source $SETUPCROSS
    cd $BUILDDIR/build
    mkdir -p arch-cross
    cd arch-cross
    if test -e $DIR; then
	echo Warning `pwd`/$DIR directory already exists - assuming we already did cross-compile
	read
    else	
	mkdir $DIR
	cd $DIR
	DISTNAME=adic-bin-win32-${DIR#adic-*}
	CROSSPREFIX=$PREFIX/cross/$DISTNAME
	$BUILDDIR/$DIR/configure $CROSS_CONFIGURE_OPTIONS $ADIC_CROSS_CONFIGURE_OPTIONS --enable-fastcompile --prefix=$CROSSPREFIX --bindir=$CROSSPREFIX --datadir=$CROSSPREFIX/data
	make install
	# rearange files
	cd $CROSSPREFIX
	mv data/doc/adic/* .
	mv data/adic d2
	rm -rf data
	mv d2 data
	$STRIP *.exe
	#create configuration
        $BUILDDIR/build/debian/$DIR/src/adicserver --dump --dataPath=data 2>&1 |xmllint --format -|unix2dos >sconfig.xml
	unix2dos >startserver.bat <<-EOF
	adicserver --file=sconfig.xml
	EOF
        $BUILDDIR/build/debian/$DIR/src/adicclient --dump --dataPath=data --width=1024 --height=768 --quality=3 2>&1 |xmllint --format -|unix2dos >cconfig.xml
	unix2dos >startclient.bat <<-EOF
	adicclient --file=cconfig.xml
	EOF
	
	cd ..
	mkdir -p $UPLOADDIR
	zip -r $UPLOADDIR/$DISTNAME.zip $DISTNAME
    fi
else
    echo cross compiler config script not found
fi


#todo upload packages
exit 0
#copy packages to repository
#todo
cp -i $UPLOADDIR/adic_* $ADIC_DEBREP
cp -i $UPLOADDIR/adic-data_* $ADIC_DEBREP
cd $ADIC_DEBREP/..
./gen-packages.sh
./update.sh
