AC_INIT([Chat],
        [3.11.1],
        [https://bugzilla.gnome.org/enter_bug.cgi?product=gnome-chat],
        [gnome-chat],
        [https://wiki.gnome.org/Apps/Chat])
AC_PREREQ(2.63)

AC_CONFIG_AUX_DIR([config])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src])

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AM_INIT_AUTOMAKE([dist-xz no-dist-gzip tar-ustar])
AM_MAINTAINER_MODE([enable])

AC_PROG_CC
AC_PROG_CC_C_O
AC_HEADER_STDC

GLIB_GSETTINGS

GLIB_MIN_VERSION=2.35.1
GTK_MIN_VERSION=3.9.11

GNOME_COMMON_INIT
GNOME_COMPILE_WARNINGS([maximum])
GNOME_DEBUG_CHECK
GNOME_MAINTAINER_MODE_DEFINES

IT_PROG_INTLTOOL([0.35.0])

GETTEXT_PACKAGE=AC_PACKAGE_TARNAME
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"],
                   [GETTEXT package name])
AM_GLIB_GNU_GETTEXT

LT_INIT

PKG_CHECK_MODULES(GLIB, [glib-2.0 >= $GLIB_MIN_VERSION])
PKG_CHECK_MODULES(GIO, [gio-2.0])
PKG_CHECK_MODULES(GTK, [gtk+-3.0 >= $GTK_MIN_VERSION])

GLIB_COMPILE_RESOURCES=`$PKG_CONFIG --variable glib_compile_resources gio-2.0`
AC_SUBST(GLIB_COMPILE_RESOURCES)

AC_CONFIG_FILES([
Makefile
data/Makefile
data/gnome-chat.desktop.in
src/Makefile
po/Makefile.in
])
AC_OUTPUT
