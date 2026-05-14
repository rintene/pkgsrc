$NetBSD$

--- make/autoconf/toolchain.m4.orig
+++ make/autoconf/toolchain.m4
@@ -42,6 +42,7 @@
 VALID_TOOLCHAINS_macosx="clang"
 VALID_TOOLCHAINS_aix="clang"
 VALID_TOOLCHAINS_windows="microsoft"
+VALID_TOOLCHAINS_solaris="gcc clang"
 
 # Toolchain descriptions
 TOOLCHAIN_DESCRIPTION_clang="clang/LLVM"
@@ -229,7 +230,7 @@
     # User override; check that it is valid
     if test "x${VALID_TOOLCHAINS/$with_toolchain_type/}" = "x${VALID_TOOLCHAINS}"; then
       AC_MSG_NOTICE([Toolchain type $with_toolchain_type is not valid on this platform.])
-      AC_MSG_NOTICE([Valid toolchains: $VALID_TOOLCHAINS.])
+      AC_MSG_NOTICE([Valid toolchains: $VALID_TOOLCHAINS. buildos: $OPENJDK_BUILD_OS])
       AC_MSG_ERROR([Cannot continue.])
     fi
     TOOLCHAIN_TYPE=$with_toolchain_type
@@ -522,6 +522,12 @@
     # If using gold it will look like:
     #   GNU gold (GNU Binutils 2.30) 1.15
     LINKER_VERSION_STRING=`$LINKER -Wl,--version 2> /dev/null | $HEAD -n 1`
+    # illumos/Solaris ld does not support --version, try -V instead
+    if test "x$LINKER_VERSION_STRING" = x; then
+      LINKER_VERSION_STRING=`$LINKER -Wl,-V 2>&1 | $HEAD -n 1`
+      LINKER_VERSION_NUMBER=`$ECHO $LINKER_VERSION_STRING | \
+          $SED -e 's/.*Solaris Link Editors: \([0-9][0-9]*\.[0-9][0-9]*\).*/\1/'`
+    fi
     # Extract version number
     if [ [[ "$LINKER_VERSION_STRING" == *gold* ]] ]; then
       [ LINKER_VERSION_NUMBER=`$ECHO $LINKER_VERSION_STRING | \

