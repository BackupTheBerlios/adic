# Configure paths for libdope
# based upon Libsigc++ script which is
# Based on Gtk-- script by Erik Andersen and Tero Pulkkinen 

dnl Test for libdope, and define DOPE_CFLAGS and DOPE_LIBS
dnl   to be used as follows:
dnl AM_PATH_DOPE(MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(AM_PATH_DOPE,
[dnl 
dnl Get the cflags and libraries from the dope-config script
dnl

dnl
dnl Prefix options
dnl
AC_ARG_WITH(dope-prefix,
[  --with-dope-prefix=PREFIX
                          Prefix where libdope is installed (optional)]
, dope_config_prefix="$withval", dope_config_prefix="")

AC_ARG_WITH(dope-exec-prefix,
[  --with-dope-exec-prefix=PREFIX 
                          Exec prefix where  libdope is installed (optional)]
, dope_config_exec_prefix="$withval", dope_config_exec_prefix="")

AC_ARG_ENABLE(dopetest, 
[  --disable-dopetest     Do not try to compile and run a test libdope 
                          program],
, enable_dopetest=yes)

dnl
dnl Prefix handling
dnl
  if test x$dope_config_exec_prefix != x ; then
     dope_config_args="$dope_config_args --exec-prefix=$dope_config_exec_prefix"
     if test x${DOPE_CONFIG+set} != xset ; then
        DOPE_CONFIG=$dope_config_exec_prefix/bin/dope-config
     fi
  fi
  if test x$dope_config_prefix != x ; then
     dope_config_args="$dope_config_args --prefix=$dope_config_prefix"
     if test x${DOPE_CONFIG+set} != xset ; then
        DOPE_CONFIG=$dope_config_prefix/bin/dope-config
     fi
  fi

dnl
dnl See if dope-config is alive
dnl
  AC_PATH_PROG(DOPE_CONFIG, dope-config, no)
  dope_version_min=$1

dnl
dnl  Version check
dnl
  AC_MSG_CHECKING(for libdope - version >= $dope_version_min)
  no_dope=""
  if test "$DOPE_CONFIG" = "no" ; then
    no_dope=yes
  else
    dope_version=`$DOPE_CONFIG --version`

    DOPE_CFLAGS=`$DOPE_CONFIG $dope_config_args --cflags`
    DOPE_LIBS=`$DOPE_CONFIG $dope_config_args --libs`

    dope_major_version=`echo $dope_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    dope_minor_version=`echo $dope_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    dope_micro_version=`echo $dope_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

    dope_major_min=`echo $dope_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    dope_minor_min=`echo $dope_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    dope_micro_min=`echo $dope_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

    dope_version_proper=`expr \
        $dope_major_version \> $dope_major_min \| \
        $dope_major_version \= $dope_major_min \& \
        $dope_minor_version \> $dope_minor_min \| \
        $dope_major_version \= $dope_major_min \& \
        $dope_minor_version \= $dope_minor_min \& \
        $dope_micro_version \>= $dope_micro_min `

    if test "$dope_version_proper" = "1" ; then
      AC_MSG_RESULT([$dope_major_version.$dope_minor_version.$dope_micro_version])
    else
      AC_MSG_RESULT(no)
      no_dope=yes
    fi

    if test "X$no_dope" = "Xyes" ; then
      enable_dopetest=no
    fi

    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS

dnl
dnl
dnl
    if test "x$enable_dopetest" = "xyes" ; then
      AC_MSG_CHECKING(if libdope sane)
      ac_save_CXXFLAGS="$CXXFLAGS"
      ac_save_LIBS="$LIBS"
      ac_exeext=""
      CXXFLAGS="$CXXFLAGS $DOPE_CFLAGS"
      LIBS="$LIBS $DOPE_LIBS"

      rm -f conf.dopetest
      AC_TRY_RUN([
#include <stdio.h>
#include <dope/dope.h>

int main(int argc,char *argv[])
{
   if (dope_major_version!=$dope_major_version ||
       dope_minor_version!=$dope_minor_version ||
       dope_micro_version!=$dope_micro_version)
     { printf("(%d.%d.%d) ",
         dope_major_version,dope_minor_version,dope_micro_version);
       return 1;
     }
   return 0;
}

],[
  AC_MSG_RESULT(yes)
],[
  AC_MSG_RESULT(no)
  no_dope=yes
]
,[echo $ac_n "cross compiling; assumed OK... $ac_c"])

       CXXFLAGS="$ac_save_CXXFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi

  dnl
  dnl
  if test "x$no_dope" = x ; then
     ifelse([$2], , :, [$2])     
  else
     DOPE_CFLAGS=""
     DOPE_LIBS=""
     ifelse([$3], , :, [$3])
  fi

  AC_LANG_RESTORE

  AC_SUBST(DOPE_CFLAGS)
  AC_SUBST(DOPE_LIBS)
])

