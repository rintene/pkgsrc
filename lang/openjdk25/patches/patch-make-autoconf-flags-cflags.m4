--- make/autoconf/flags-cflags.m4.orig	2026-04-30 10:06:07.846792825 +0200
+++ make/autoconf/flags-cflags.m4	2026-04-30 10:12:42.909211781 +0200
@@ -40,8 +40,16 @@
     # --disable-new-dtags forces use of RPATH instead of RUNPATH for rpaths.
     # This protects internal library dependencies within the JDK from being
     # overridden using LD_LIBRARY_PATH. See JDK-8326891 for more information.
-    SET_EXECUTABLE_ORIGIN='-Wl,-rpath,\$$ORIGIN[$]1 -Wl,--disable-new-dtags'
-    SET_SHARED_LIBRARY_ORIGIN="-Wl,-z,origin $SET_EXECUTABLE_ORIGIN"
+    if test "x$OPENJDK_TARGET_OS" = xlinux; then
+     SET_EXECUTABLE_ORIGIN='-Wl,-rpath,\$$ORIGIN[$]1 -Wl,--disable-new-dtags'
+    else
+     SET_EXECUTABLE_ORIGIN='-Wl,-R\$$ORIGIN[$]1'
+    fi
+    if test "x$OPENJDK_TARGET_OS" = xlinux; then
+     SET_SHARED_LIBRARY_ORIGIN="-Wl,-z,origin $SET_EXECUTABLE_ORIGIN"
+    else
+     SET_SHARED_LIBRARY_ORIGIN="$SET_EXECUTABLE_ORIGIN"
+    fi
     SET_SHARED_LIBRARY_NAME='-Wl,-soname=[$]1'
 
   elif test "x$TOOLCHAIN_TYPE" = xclang; then
@@ -62,9 +70,10 @@
     else
       # Default works for linux, might work on other platforms as well.
       SHARED_LIBRARY_FLAGS='-shared'
-      SET_EXECUTABLE_ORIGIN='-Wl,-rpath,\$$ORIGIN[$]1'
       if test "x$OPENJDK_TARGET_OS" = xlinux; then
-        SET_EXECUTABLE_ORIGIN="$SET_EXECUTABLE_ORIGIN -Wl,--disable-new-dtags"
+       SET_EXECUTABLE_ORIGIN="-Wl,-rpath,\$$ORIGIN[$]1,--disable-new-dtags"
+      else
+       SET_EXECUTABLE_ORIGIN='-Wl,-R\$$ORIGIN[$]1'
       fi
       SET_SHARED_LIBRARY_NAME='-Wl,-soname=[$]1'
 
