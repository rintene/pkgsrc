--- src/hotspot/os/posix/signals_posix.cpp.orig 2026-05-29 16:54:53.322214775 +0200
+++ src/hotspot/os/posix/signals_posix.cpp      2026-05-29 16:57:10.952544712 +0200
@@ -548,6 +548,8 @@
 #define JVM_HANDLE_XXX_SIGNAL JVM_handle_aix_signal
 #elif defined(LINUX)
 #define JVM_HANDLE_XXX_SIGNAL JVM_handle_linux_signal
+#elif defined(SOLARIS) || defined(__sun) || defined(__illumos__)
+#define JVM_HANDLE_XXX_SIGNAL JVM_handle_solaris_signal
 #else
 #error who are you?
 #endif
