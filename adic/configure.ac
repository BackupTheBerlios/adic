dnl process this file with autoconf to produce a configure script
AC_PREREQ(2.53)
AC_INIT
AC_CONFIG_SRCDIR([src/mesh.h])


dnl
dnl Source packaging numbers
PACKAGE_MAJOR_VERSION=0
PACKAGE_MINOR_VERSION=0
PACKAGE_MICRO_VERSION=9
PACKAGE_VERSION=$PACKAGE_MAJOR_VERSION.$PACKAGE_MINOR_VERSION.$PACKAGE_MICRO_VERSION
AC_DEFINE_UNQUOTED(PACKAGE_MAJOR_VERSION, $PACKAGE_MAJOR_VERSION, [major version])
AC_DEFINE_UNQUOTED(PACKAGE_MINOR_VERSION, $PACKAGE_MINOR_VERSION, [minor version])
AC_DEFINE_UNQUOTED(PACKAGE_MICRO_VERSION, $PACKAGE_MICRO_VERSION, [micro version])

dnl for automake
VERSION=$PACKAGE_VERSION
PACKAGE=adic

AC_CONFIG_AUX_DIR(config)
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE($PACKAGE, $VERSION)
AM_CONFIG_HEADER(config/config.h)
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CXXCPP

dnl build options
AC_ARG_VAR(ADIC_PORT, the default port to use)

dnl checks for libraries
AM_PATH_DOPE(0.0.6, , [AC_MSG_ERROR([dope++ required])])
dnl MDL_HAVE_OPENGL

AM_PATH_SDL(1.2.0, , [AC_MSG_ERROR([SDL required])])
AC_CACHE_SAVE

dnl variable substitutions
if test "x$ADIC_PORT" = "x"; then
	ADIC_PORT="40700"
	echo ADIC_PORT is not set. I use default $ADIC_PORT
fi
AC_DEFINE_UNQUOTED(ADIC_PORT, $ADIC_PORT, [default port to use])

dnl output files
AC_CONFIG_FILES([Makefile doc/Makefile doc/Doxyfile config/Makefile config/m4/Makefile src/Makefile data/Makefile])
AC_OUTPUT


