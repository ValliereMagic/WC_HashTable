#Init the package
AC_INIT([WC_HashTable], [1.0.0])

#Minimum version of autoconf
AC_PREREQ([2.68])

#File must exist for the build to continue
AC_CONFIG_SRCDIR([src/WC_HashTable.c])

#Location for header file for make config
AC_CONFIG_HEADERS([src/config.h])

#Auxiliary build files
AC_CONFIG_AUX_DIR([build-aux])

#Configure M4 DIR
AC_CONFIG_MACRO_DIRS([m4])

#Init automake
AM_INIT_AUTOMAKE([1.11 -Wall -Werror foreign subdir-objects])

#Allow use of non-POSIX archivers
AM_PROG_AR

#Init Libtool
LT_INIT([win32-dll])

#Configure creates makefile
AC_CONFIG_FILES([Makefile])

#Find and probe C compiler
AC_PROG_CC

#add various checks here

# Checks for header files.
AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC

AC_OUTPUT
