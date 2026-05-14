--- make/autoconf/libraries.m4.orig	2026-04-04 06:52:09.555105597 +0200
+++ make/autoconf/libraries.m4	2026-04-04 06:53:17.165292031 +0200
@@ -74,6 +74,10 @@
     NEEDS_LIB_FREETYPE=true
   fi
 
+  if test "x$OPENJDK_TARGET_OS" = xsolaris; then
+    BASIC_JVM_LIBS="$BASIC_JVM_LIBS -lsocket -lsched -ldoor -lnsl -lkstat"
+  fi
+
   # Check if alsa is needed
   if test "x$OPENJDK_TARGET_OS" = xlinux; then
     NEEDS_LIB_ALSA=true

--- make/autoconf/libraries.m4.orig     2026-04-06 14:35:43.682650962 +0200
+++ make/autoconf/libraries.m4  2026-04-06 14:36:12.178020633 +0200
@@ -145,6 +145,11 @@
     BASIC_JVM_LIBS="$BASIC_JVM_LIBS -lrt"
   fi

+  # Solaris/illumos socket, scheduling, kstat libs
+  if test "x$OPENJDK_TARGET_OS" = xsolaris; then
+    BASIC_JVM_LIBS="$BASIC_JVM_LIBS -lsocket -lsched -ldoor -lnsl -lkstat"
+  fi
+
   # perfstat lib
   if test "x$OPENJDK_TARGET_OS" = xaix; then
     BASIC_JVM_LIBS="$BASIC_JVM_LIBS -lperfstat"

