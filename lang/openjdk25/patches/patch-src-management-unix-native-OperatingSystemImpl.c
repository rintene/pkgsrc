--- src/jdk.management/unix/native/libmanagement_ext/OperatingSystemImpl.c.orig	2026-04-30 19:05:03.095813519 +0200
+++ src/jdk.management/unix/native/libmanagement_ext/OperatingSystemImpl.c	2026-04-30 19:07:17.838694603 +0200
@@ -63,7 +63,7 @@
 
 static jlong page_size = 0;
 
-#if defined(_ALLBSD_SOURCE) || defined(_AIX)
+#if defined(_ALLBSD_SOURCE) || defined(_AIX) || defined(__sun)
 #define MB      (1024UL * 1024UL)
 #else
 
