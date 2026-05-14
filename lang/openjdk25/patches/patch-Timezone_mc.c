--- src/java.base/unix/native/libjava/TimeZone_md.c.orig	2026-04-08 17:36:46.351476331 +0200
+++ src/java.base/unix/native/libjava/TimeZone_md.c	2026-04-08 17:54:31.376278917 +0200
@@ -56,6 +56,46 @@
 
 static const char popularZones[][4] = {"UTC", "GMT"};
 
+#ifdef __sun
+static
+char* getPlatformTimeZoneID(void) {
+    // Try TZ environment variable
+    const char* tz = getenv("TZ");
+    if (tz != NULL && tz[0] != '\0') {
+        return strdup(tz);
+    }
+
+    // Solaris: read /etc/default/init
+    FILE* fp = fopen("/etc/default/init", "r");
+    if (fp != NULL) {
+        char line[256];
+        while (fgets(line, sizeof(line), fp)) {
+            if (strncmp(line, "TZ=", 3) == 0) {
+                char* value = line + 3;
+                char* nl = strchr(value, '\n');
+                if (nl) *nl = '\0';
+
+                // Trim whitespace
+                while (*value == ' ' || *value == '\t') value++;
+                char* end = value + strlen(value) - 1;
+                while (end > value && (*end == ' ' || *end == '\t')) end--;
+                *(end + 1) = '\0';
+
+                fclose(fp);
+                if (strlen(value) > 0) {
+                    return strdup(value);
+                }
+                break;
+            }
+        }
+        fclose(fp);
+    }
+
+    // Fallback
+    return strdup("GMT");
+}
+#endif
+
 #if defined(__linux__) || defined(MACOSX)
 static char *isFileIdentical(char* buf, size_t size, char *pathname);
 
