--- src/java.desktop/share/native/libjsound/SoundDefs.h.orig	2026-05-05 13:45:16.709335985 +0200
+++ src/java.desktop/share/native/libjsound/SoundDefs.h	2026-05-05 13:45:37.301278690 +0200
@@ -123,5 +123,8 @@
 #define INLINE          inline
 #endif
 
+#if X_PLATFORM == X_SOLARIS
+#define INLINE          inline
+#endif
 
 #endif  // __SOUNDDEFS_INCLUDED__
