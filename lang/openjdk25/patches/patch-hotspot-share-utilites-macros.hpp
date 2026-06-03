--- src/hotspot/share/utilities/macros.hpp
+++ src/hotspot/share/utilities/macros.hpp
@@ -394,6 +394,9 @@
 #ifdef LINUX
 #define LINUX_ONLY(code) code
 #define NOT_LINUX(code)
+#elif defined(__sun)
+#define LINUX_ONLY(code)
+#define NOT_LINUX(code) code
 #else
 #define LINUX_ONLY(code)
 #define NOT_LINUX(code) code
