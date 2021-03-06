dnl process this file with autoconf to produce a configure script
AC_PREREQ(2.53)
AC_INIT
AC_CONFIG_SRCDIR([src/mesh.h])


dnl
dnl Source packaging numbers
. ${srcdir}/version

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

dnl this option is mainly for building packages for debian/woody
dnl because woody does not contain gcc 3.2
AC_ARG_ENABLE([staticcpp],
	AC_HELP_STRING(
		[--enable-staticcpp],
		[enable static cpp standard library @<:@default=no@:>@]
	),
     	[case "${enableval}" in
	       yes) staticcpp=true ;;
	       no)  staticcpp=false ;;
	       *) AC_MSG_ERROR(bad value ${enableval} for --enable-staticcpp) ;;
	     esac],
	staticcpp=false;
)

dnl this option is to build portable linux binaries
AC_ARG_ENABLE([mostlystatic],
	AC_HELP_STRING(
		[--enable-mostlystatic],
		[static linkage as far as possible @<:@default=no@:>@]
	),
     	[case "${enableval}" in
	       yes) mostlystatic=true ;;
	       no)  mostlystatic=false ;;
	       *) AC_MSG_ERROR(bad value ${enableval} for --enable-mostlystatic) ;;
	     esac],
	mostlystatic=false;
)

dnl this option is to build static binaries
AC_ARG_ENABLE([allstatic],
	AC_HELP_STRING(
		[--enable-allstatic],
		[static linkage @<:@default=no@:>@]
	),
     	[case "${enableval}" in
	       yes) allstatic=true ;;
	       no)  allstatic=false ;;
	       *) AC_MSG_ERROR(bad value ${enableval} for --enable-allstatic) ;;
	     esac],
	allstatic=false;
)

AC_ARG_ENABLE([fastcompile],
	AC_HELP_STRING(
		[--enable-fastcompile],
		[use fast compilation hack @<:@default=no@:>@]
	),
     	[case "${enableval}" in
	       yes) fastcompile=true ;;
	       no)  fastcompile=false ;;
	       *) AC_MSG_ERROR(bad value ${enableval} for --enable-fastcompile) ;;
	     esac]
	,
	[fastcompile=false])
AM_CONDITIONAL(FASTCOMPILE, test x$fastcompile = xtrue)

dnl checks for libraries
AM_PATH_DOPE(0.1.1, , [AC_MSG_ERROR([dope++ required])])
AM_PATH_SDL(1.2.0, , [AC_MSG_ERROR([SDL required])])
AC_CACHE_SAVE

dnl variable substitutions
if test "x$ADIC_PORT" = "x"; then
	ADIC_PORT="40700"
	echo ADIC_PORT is not set. I use default $ADIC_PORT
fi
AC_DEFINE_UNQUOTED(ADIC_PORT, $ADIC_PORT, [default port to use])

if $staticcpp||$mostlystatic; then
	dnl hack to get static linkage with libtool and g++-3.2 of stdc++ library
	dnl it seems we have to use the dynamic gcc_s - otherwise exception handling does
	dnl not work - but are different gcc_s libraries compatible ?
	dnl especially is the gcc_s from gcc 3.0.x compatible with the one in gcc 3.2 ?
	dnl it seems to work
	dnl update:
	dnl exceptions work with libgcc_eh.a - now all c++ stuff is linked statically
	SPECIAL_LDADD="-nodefaultlibs `$CXX --print-file-name=libstdc++.a` -lgcc_eh -lc"
fi

if $staticcpp; then
	echo "linking stdc++ library statically"
	SERVER_LDADD="$SPECIAL_LDADD"
	GUICLIENT_LDADD="$SPECIAL_LDADD"
	BOTCLIENT_LDADD="$SPECIAL_LDADD"
fi

if $mostlystatic; then
	echo "linking as much as possible statically"
	SERVER_LDADD="-all-static"
	dnl -static with libtool means that all libtool libraries are linked statically
	GUICLIENT_LDADD="-static $SPECIAL_LDADD"
	BOTCLIENT_LDADD="-all-static"
fi

if $allstatic; then
	dnl this does not work with XFree & Nvidia Opengl
	dnl it seems you cannot dlopen nvidia's opengl from a static binary
	dnl did not look into the details yet
	echo "linking statically"
	SERVER_LDADD="-all-static"
	GUICLIENT_LDADD="-all-static"
	BOTCLIENT_LDADD="-all-static"
fi

dnl echo SERVER_LDADD=$SERVER_LDADD
dnl echo GUICLIENT_LDADD=$GUICLIENT_LDADD
dnl echo BOTCLIENT_LDADD=$BOTCLIENT_LDADD

AC_SUBST(SERVER_LDADD)
AC_SUBST(GUICLIENT_LDADD)
AC_SUBST(BOTCLIENT_LDADD)

dnl output files
AC_CONFIG_FILES([Makefile doc/Makefile doc/Doxyfile config/Makefile config/m4/Makefile src/Makefile debian/Makefile data/Makefile])
AC_OUTPUT


