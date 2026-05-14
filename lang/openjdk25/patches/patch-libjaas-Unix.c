--- src/jdk.security.auth/unix/native/libjaas/Unix.c.orig	2026-05-05 17:37:23.850307734 +0200
+++ src/jdk.security.auth/unix/native/libjaas/Unix.c	2026-05-05 17:38:07.093774566 +0200
@@ -111,8 +111,13 @@
     }
 
     memset(pwd_buf, 0, sizeof(pwd_buf));
+#ifdef __sun
+    pwd = getpwuid_r(getuid(), &resbuf, pwd_buf, sizeof(pwd_buf));
+    if (pwd != NULL) {
+#else
     if (getpwuid_r(getuid(), &resbuf, pwd_buf, sizeof(pwd_buf), &pwd) == 0 &&
             pwd != NULL) {
+#endif
         (*env)->SetLongField(env, obj, userID, pwd->pw_uid);
         (*env)->SetLongField(env, obj, groupID, pwd->pw_gid);
         jstr = (*env)->NewStringUTF(env, pwd->pw_name);
