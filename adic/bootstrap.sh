#!/bin/sh
# Run this if you have to create the configure script
# (f.e. you checked out from cvs)

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME="game"

(test -f $srcdir/configure.ac) || {
    echo -n "**Error**: Directory "\`$srcdir\'" does not look like the"
    echo " top-level directory"
    exit 1
}

(test -f $srcdir/configure) && {
    echo -n "**Error**: Directory "\`$srcdir\'" already contains"
    echo " the configure script"
    exit 1
}

. $srcdir/config/autogen.sh
