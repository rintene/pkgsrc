--- src/hotspot/os/posix/os_posix.cpp
+++ src/hotspot/os/posix/os_posix.cpp
@@ -167,7 +167,7 @@
 
 bool os::committed_in_range(address start, size_t size, address& committed_start, size_t& committed_size) {
 
-#ifdef _AIX
+#ifndef LINUX
   committed_start = start;
   committed_size = size;
   return true;
@@ -618,8 +618,10 @@
 
   print_rlimit(st, ", THREADS", RLIMIT_THREADS);
 #else
+#ifndef __sun
   print_rlimit(st, ", NPROC", RLIMIT_NPROC);
 #endif
+#endif
 
   print_rlimit(st, ", NOFILE", RLIMIT_NOFILE);
   print_rlimit(st, ", AS", RLIMIT_AS, true);
