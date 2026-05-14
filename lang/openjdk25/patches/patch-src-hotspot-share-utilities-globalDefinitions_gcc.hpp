--- src/hotspot/share/utilities/globalDefinitions_gcc.hpp
+++ src/hotspot/share/utilities/globalDefinitions_gcc.hpp
@@ -74,7 +74,7 @@
 // checking for nanness
 #if defined(__APPLE__)
 inline int g_isnan(double f) { return isnan(f); }
-#elif defined(LINUX) || defined(_ALLBSD_SOURCE) || defined(_AIX)
+#elif defined(LINUX) || defined(_ALLBSD_SOURCE) || defined(_AIX) || defined(__sun)
 inline int g_isnan(float  f) { return isnan(f); }
 inline int g_isnan(double f) { return isnan(f); }
 #else
