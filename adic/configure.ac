dnl process this file with autoconf to produce a configure script
AC_PREREQ(2.53)
AC_INIT
AC_CONFIG_SRCDIR([src/mesh.h])


dnl
dnl Source packaging numbers
PACKAGE_MAJOR_VERSION=0
PACKAGE_MINOR_VERSION=9
PACKAGE_MICRO_VERSION=0
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
dnl AC_PROG_CC
dnl AC_PROG_CPP
AC_PROG_CXX
dnl AC_PROG_CXXCPP

dnl build options
AC_ARG_VAR(ADIC_PORT, the default port to use)

AC_ARG_ENABLE([staticcpp],
	AC_HELP_STRING([--enable-staticcpp],
		[enable static cpp standard library @<:@default=no@:>@]),
		[],
		[enable_staticcpp=no])

dnl checks for libraries
AM_PATH_DOPE(0.0.8, , [AC_MSG_ERROR([dope++ required])])
dnl MDL_HAVE_OPENGL

AM_PATH_SDL(1.2.0, , [AC_MSG_ERROR([SDL required])])
AC_CACHE_SAVE

dnl variable substitutions
if test "x$ADIC_PORT" = "x"; then
	ADIC_PORT="40700"
	echo ADIC_PORT is not set. I use default $ADIC_PORT
fi
AC_DEFINE_UNQUOTED(ADIC_PORT, $ADIC_PORT, [default port to use])


if test "x$enable_staticcpp" = "xno"; then
     SPECIAL_LDADD=""
else
     dnl hack to get static linkage with libtool and g++-3.2 of stdc++ library
     dnl it seems we have to use the dynamic gcc_s - otherwise exception handling does
     dnl not work - but are different gcc_s libraries compatible ?
     dnl especially is the gcc_s from gcc 3.0.x compatible with the one in gcc 3.2 ?
     dnl it seems to work
     SPECIAL_LDADD="-nodefaultlibs `$CXX --print-file-name=libstdc++.a` -lgcc_s -lc"
fi
AC_SUBST(SPECIAL_LDADD)

dnl output files
AC_CONFIG_FILES([Makefile doc/Makefile doc/Doxyfile config/Makefile config/m4/Makefile src/Makefile debian/Makefile data/Makefile])
AC_OUTPUT


