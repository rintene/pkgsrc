--- src/hotspot/share/runtime/os.cpp     2026-04-17 21:08:13.000000000 +0200
+++ src/hotspot/share/runtime/os.cpp       2026-05-21 11:55:09.466491855 +0200
@@ -91,6 +91,7 @@

 # include <signal.h>
 # include <errno.h>
+# include <alloca.h>

 OSThread*         os::_starting_thread    = nullptr;
 volatile unsigned int os::_rand_seed      = 1234567;
@@ -182,7 +183,7 @@
   // No offset when dealing with UTC
   time_t UTC_to_local = 0;
   if (!utc) {
-#if (defined(_ALLBSD_SOURCE) || defined(_GNU_SOURCE)) && !defined(AIX)
+#if (defined(_ALLBSD_SOURCE) || defined(_BSD_SOURCE)) && !defined(AIX) && !defined(__sun)
     UTC_to_local = -(time_struct.tm_gmtoff);
 #elif defined(_WINDOWS)
     long zone;
@@ -193,7 +194,7 @@
 #endif

     // tm_gmtoff already includes adjustment for daylight saving
-#if !defined(_ALLBSD_SOURCE) && !defined(_GNU_SOURCE)
+#if !defined(_ALLBSD_SOURCE) && !defined(_BSD_SOURCE)
     // If daylight savings time is in effect,
     // we are 1 hour East of our time zone
     if (time_struct.tm_isdst > 0) {
