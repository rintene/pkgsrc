--- src/hotspot/share/runtime/os.hpp
+++ src/hotspot/share/runtime/os.hpp
@@ -1032,6 +1032,8 @@
   class Aix;
 #elif defined(BSD)
   class Bsd;
+#elif defined(__sun)
+  class Solaris;
 #elif defined(LINUX)
   class Linux;
 #elif defined(_WINDOWS)
