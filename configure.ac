# $Id$ -*- Autoconf -*-

AC_PREREQ(2.65)
AC_INIT(stx-execpipe, 0.7.1)
AC_CONFIG_SRCDIR(src/stx-execpipe.cc)
AC_CONFIG_AUX_DIR(acscripts)

AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

if test "$CFLAGS" == ""; then
    CFLAGS="-g"
fi
if test "$CXXFLAGS" == ""; then
    CXXFLAGS="-g"
fi

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB

AC_LANG([C++])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE

# Checks for library functions.
AC_HEADER_STDC
AC_CHECK_FUNCS([memset])

# Arrange for 64-bit file offsets and special open() call.
AC_SYS_LARGEFILE

# Check whether to enable gcov coverage flags and macros

AC_ARG_ENABLE(gcov,
    AS_HELP_STRING([--enable-gcov],
                   [enable test coverage with gcov @<:@default=no@:>@]),
	[case "${enableval}" in
         yes) gcov=true ;;
         no)  gcov=false ;;
         *)   AC_MSG_ERROR([bad value ${enableval} for --enable-gcov]) ;;
         esac],
        [gcov=false])

AM_CONDITIONAL(GCOV, test x"$gcov" = "xtrue")

if test x"$gcov" = "xtrue"; then
    CFLAGS="$CFLAGS -fprofile-arcs -ftest-coverage"
    CXXFLAGS="$CXXFLAGS -fprofile-arcs -ftest-coverage"
fi

# Checks for library functions.

AC_CHECK_LIB(crypto, CRYPTO_lock,
  [LIBS="$LIBS -lcrypto"],
  [AC_MSG_ERROR([ Required OpenSSL Crypto Library not found. ])]
)

AC_CONFIG_FILES([Makefile
		 src/Makefile
		 testsuite/Makefile
		 examples/Makefile])
AC_OUTPUT
