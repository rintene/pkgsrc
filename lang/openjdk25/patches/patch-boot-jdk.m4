--- make/autoconf/boot-jdk.m4.orig	2026-02-04 03:45:26.000000000 +0100
+++ make/autoconf/boot-jdk.m4	2026-05-14 15:05:39.873407767 +0200
@@ -298,6 +298,7 @@
       fi
       AC_MSG_RESULT(\[$]BOOT_JDK/bin/$2)
       UTIL_FIXUP_EXECUTABLE($1)
+      $1="env LD_LIBRARY_PATH=$BOOT_JDK/lib [$]$1"
       AC_SUBST($1)
     ])
 ])
@@ -470,6 +471,8 @@
 
   # Starting amount of heap memory.
   UTIL_ADD_JVM_ARG_IF_OK([-Xms64M],boot_jdk_jvmargs_big,[$JAVA])
+  UTIL_ADD_JVM_ARG_IF_OK([-XX:TieredStopAtLevel=1],boot_jdk_jvmargs,[$JAVA])
+  UTIL_ADD_JVM_ARG_IF_OK([-XX:HeapBaseMinAddress=512m],boot_jdk_jvmargs,[$JAVA])
   BOOTCYCLE_JVM_ARGS_BIG=-Xms64M
 
   # Maximum amount of heap memory.
@@ -631,9 +634,14 @@
 
   # Since these tools do not yet exist, we cannot use UTIL_FIXUP_EXECUTABLE to
   # detect the need of fixpath
-  JMOD="$BUILD_JDK/bin/jmod"
+  if test "x$OPENJDK_BUILD_OS" = "xsolaris"; then
+    JMOD="LD_LIBRARY_PATH=$BUILD_JDK/lib:$BUILD_JDK/lib/server:$BUILD_JDK/lib/zero $BUILD_JDK/bin/jmod"
+    JLINK="LD_LIBRARY_PATH=$BUILD_JDK/lib:$BUILD_JDK/lib/server:$BUILD_JDK/lib/zero $BUILD_JDK/bin/jlink"
+  else
+    JMOD="$BUILD_JDK/bin/jmod"
+    JLINK="$BUILD_JDK/bin/jlink"
+  fi
   UTIL_ADD_FIXPATH(JMOD)
-  JLINK="$BUILD_JDK/bin/jlink"
   UTIL_ADD_FIXPATH(JLINK)
   AC_SUBST(JMOD)
   AC_SUBST(JLINK)
