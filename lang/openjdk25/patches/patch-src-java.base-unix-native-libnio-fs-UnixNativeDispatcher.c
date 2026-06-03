--- src/java.base/unix/native/libnio/fs/UnixNativeDispatcher.c.orig	Wed Jun  3 17:46:31 2026
+++ src/java.base/unix/native/libnio/fs/UnixNativeDispatcher.c	Wed Jun  3 17:46:42 2026
@@ -42,6 +42,26 @@
 #endif
 #include <sys/time.h>
 
+#if defined(__sun)
+#include "solaris_compat.h"
+#include <sys/syscall.h>
+/* On illumos, errno is not reliably set in JVM thread context.
+ * __systemcall returns the errno directly as its return value. */
+static int illumos_errno;
+#define RESTARTABLE_SYSCALL(call, result) do { \
+    sysret_t _sret; \
+    long _err; \
+    do { \
+        _err = __systemcall(&_sret, call); \
+        result = (_err == 0) ? (int)_sret.sys_rval1 : -1; \
+        illumos_errno = (int)_err; \
+    } while (result == -1 && illumos_errno == EINTR); \
+} while(0)
+#define GET_ERRNO() illumos_errno
+#else
+#define GET_ERRNO() errno
+#endif
+
 #if defined(__linux__) || defined(_ALLBSD_SOURCE)
 #include <sys/xattr.h>
 #endif
@@ -258,14 +278,29 @@
  * Call this to throw an internal UnixException when a system/library
  * call fails
  */
-static void throwUnixException(JNIEnv* env, int errnum) {
+/*static void throwUnixException(JNIEnv* env, int errnum) {
     jobject x = JNU_NewObjectByName(env, "sun/nio/fs/UnixException",
         "(I)V", errnum);
     if (x != NULL) {
         (*env)->Throw(env, x);
     }
+}*/
+static void throwUnixException(JNIEnv* env, int errnum) {
+    fflush(stderr);
+    jclass cls = (*env)->FindClass(env, "sun/nio/fs/UnixException");
+    if (cls != NULL) {
+        jmethodID mid = (*env)->GetMethodID(env, cls, "<init>", "(I)V");
+        if (mid != NULL) {
+            fflush(stderr);
+            jobject x = (*env)->NewObject(env, cls, mid, (jint)errnum);
+            fflush(stderr);
+            if (x != NULL) {
+                (*env)->Throw(env, x);
+            }
+        }
+        (*env)->DeleteLocalRef(env, cls);
+    }
 }
-
 /**
  * Initialization
  */
@@ -415,7 +450,7 @@
     /* EINTR not listed as a possible error */
     char* cwd = getcwd(buf, sizeof(buf));
     if (cwd == NULL) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     } else {
         jsize len = (jsize)strlen(buf);
         result = (*env)->NewByteArray(env, len);
@@ -454,7 +489,53 @@
     return (jint)res;
 }
 
+JNIEXPORT jlong JNICALL
+Java_sun_nio_fs_UnixNativeDispatcher_fopen0(JNIEnv* env, jclass this,
+    jlong pathAddress, jlong modeAddress)
+{
+    FILE* fp = NULL;
+    int saved_errno;
+    const char* path = (const char*)jlong_to_ptr(pathAddress);
+    const char* mode = (const char*)jlong_to_ptr(modeAddress);
+    do {
+        fp = fopen(path, mode);
+        saved_errno = errno;
+    } while (fp == NULL && saved_errno == EINTR);
+    if (fp == NULL) {
+        /* Workaround for illumos: fopen does not set errno reliably in JVM
+         * thread context. Use open syscall to get the real errno. */
+#ifdef __sun
+        int fd;
+        do {
+            fd = syscall(SYS_openat, AT_FDCWD, path, O_RDONLY, 0);
+            saved_errno = errno;
+        } while (fd == -1 && saved_errno == EINTR);
+        if (fd >= 0) close(fd);
+        /* fd succeeded but fopen failed - use EIO as fallback */
+        if (saved_errno == 0) saved_errno = EIO;
+#endif
+        throwUnixException(env, errno);
+    }
+
+    return ptr_to_jlong(fp);
+}
+
 JNIEXPORT void JNICALL
+Java_sun_nio_fs_UnixNativeDispatcher_fclose(JNIEnv* env, jclass this, jlong stream)
+{
+    FILE* fp = jlong_to_ptr(stream);
+
+    /* NOTE: fclose() wrapper is only used with read-only streams.
+     * If it ever is used with write streams, it might be better to add
+     * RESTARTABLE(fflush(fp)) before closing, to make sure the stream
+     * is completely written even if fclose() failed.
+     */
+    if (fclose(fp) == EOF && errno != EINTR) {
+        throwUnixException(env, GET_ERRNO());
+    }
+}
+
+JNIEXPORT void JNICALL
 Java_sun_nio_fs_UnixNativeDispatcher_rewind(JNIEnv* env, jclass this, jlong stream)
 {
     FILE* fp = jlong_to_ptr(stream);
@@ -498,17 +579,28 @@
 
     return (jint)res;
 }
-
-JNIEXPORT jint JNICALL
 Java_sun_nio_fs_UnixNativeDispatcher_open0(JNIEnv* env, jclass this,
     jlong pathAddress, jint oflags, jint mode)
 {
     jint fd;
+    int saved_errno;
     const char* path = (const char*)jlong_to_ptr(pathAddress);
-
-    RESTARTABLE(open(path, (int)oflags, (mode_t)mode), fd);
+#ifdef __sun
+ {
+        sysret_t sret;
+        long err;
+        do {
+            err = __systemcall(&sret, SYS_openat, AT_FDCWD, path, (int)oflags, (mode_t)mode);
+        } while (err == EINTR);
+        fd = (err == 0) ? (jint)sret.sys_rval1 : -1;
+        saved_errno = (int)err;
+    }
+#else
+    RESTARTABLE(openat(AT_FDCWD, path, (int)oflags, (mode_t)mode), fd);
+    saved_errno = errno;
+#endif
     if (fd == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, saved_errno);
     }
     return fd;
 }
@@ -527,7 +619,7 @@
 
     RESTARTABLE((*my_openat_func)(dfd, path, (int)oflags, (mode_t)mode), fd);
     if (fd == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
     return fd;
 }
@@ -542,9 +634,24 @@
 #else
     res = close((int)fd);
 #endif
+#ifdef __sun
+    if (res == -1) {
+        sysret_t sret;
+        int cerr = (int)__systemcall(&sret, SYS_close, (int)fd);
+        if (cerr != EINTR) {
+            throwUnixException(env, cerr);
+        }
+        return;
+    }
+#endif
+#ifdef __sun
+    /* On illumos close() rarely fails; errno unreliable, skip error check */
+    (void)res;
+#else
     if (res == -1 && errno != EINTR) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
+#endif
 }
 
 JNIEXPORT jint JNICALL
@@ -553,9 +660,22 @@
 {
     ssize_t n;
     void* bufp = jlong_to_ptr(address);
+
+#ifdef __sun
+    {
+        sysret_t sret;
+        long _err;
+        do {
+            _err = __systemcall(&sret, SYS_read, (int)fd, bufp, (size_t)nbytes);
+        } while (_err == EINTR);
+        n = (_err == 0) ? (ssize_t)sret.sys_rval1 : -1;
+        illumos_errno = (int)_err;
+    }
+#else
     RESTARTABLE(read((int)fd, bufp, (size_t)nbytes), n);
+#endif
     if (n == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
     return (jint)n;
 }
@@ -566,9 +686,21 @@
 {
     ssize_t n;
     void* bufp = jlong_to_ptr(address);
+#ifdef __sun
+    {
+        sysret_t sret;
+        long _err;
+        do {
+            _err = __systemcall(&sret, SYS_write, (int)fd, bufp, (size_t)nbytes);
+        } while (_err == EINTR);
+        n = (_err == 0) ? (ssize_t)sret.sys_rval1 : -1;
+        illumos_errno = (int)_err;
+    }
+#else
     RESTARTABLE(write((int)fd, bufp, (size_t)nbytes), n);
+#endif
     if (n == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
     return (jint)n;
 }
@@ -668,12 +800,20 @@
         }
     }
 #endif
+#ifdef __sun
+    {
+        sysret_t sret;
+        illumos_errno = (int)__systemcall(&sret, SYS_fstatat, AT_FDCWD, path, &buf, 0);
+        err = (illumos_errno == 0) ? 0 : -1;
+    }
+#else
     RESTARTABLE(stat(path, &buf), err);
+#endif
     if (err == 0) {
         copy_stat_attributes(env, &buf, attrs);
         return 0;
     } else {
-        return errno;
+        return GET_ERRNO();
     }
 }
 
@@ -695,15 +835,23 @@
         if (err == 0) {
             copy_statx_attributes(env, &statx_buf, attrs);
         } else {
-            throwUnixException(env, errno);
+            throwUnixException(env, GET_ERRNO());
         }
         // statx was available, so return now
         return;
     }
 #endif
+#ifdef __sun
+    {
+        sysret_t sret;
+        illumos_errno = (int)__systemcall(&sret, SYS_fstatat, AT_FDCWD, path, &buf, AT_SYMLINK_NOFOLLOW);
+        err = (illumos_errno == 0) ? 0 : -1;
+    }
+#else
     RESTARTABLE(lstat(path, &buf), err);
+#endif
     if (err == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     } else {
         copy_stat_attributes(env, &buf, attrs);
     }
@@ -727,15 +875,23 @@
         if (err == 0) {
             copy_statx_attributes(env, &statx_buf, attrs);
         } else {
-            throwUnixException(env, errno);
+            throwUnixException(env, GET_ERRNO());
         }
         // statx was available, so return now
         return;
     }
 #endif
+#ifdef __sun
+    {
+        sysret_t sret;
+        illumos_errno = (int)__systemcall(&sret, SYS_fstat, (int)fd, &buf);
+        err = (illumos_errno == 0) ? 0 : -1;
+    }
+#else
     RESTARTABLE(fstat((int)fd, &buf), err);
+#endif
     if (err == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     } else {
         copy_stat_attributes(env, &buf, attrs);
     }
@@ -762,7 +918,7 @@
         if (err == 0) {
             copy_statx_attributes(env, &statx_buf, attrs);
         } else {
-            throwUnixException(env, errno);
+            throwUnixException(env, GET_ERRNO());
         }
         // statx was available, so return now
         return;
@@ -773,9 +929,17 @@
         JNU_ThrowInternalError(env, "should not reach here");
         return;
     }
+#ifdef __sun
+    {
+        sysret_t sret;
+        illumos_errno = (int)__systemcall(&sret, SYS_fstatat, (int)dfd, path, &buf, (int)flag);
+        err = (illumos_errno == 0) ? 0 : -1;
+    }
+#else
     RESTARTABLE((*my_fstatat_func)((int)dfd, path, &buf, (int)flag), err);
+#endif
     if (err == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     } else {
         copy_stat_attributes(env, &buf, attrs);
     }
@@ -790,7 +954,7 @@
 
     RESTARTABLE(chmod(path, (mode_t)mode), err);
     if (err == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
 }
 
@@ -802,7 +966,7 @@
 
     RESTARTABLE(fchmod((int)filedes, (mode_t)mode), err);
     if (err == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
 }
 
@@ -815,7 +979,7 @@
 
     RESTARTABLE(fchmodat((int)fd, path, (mode_t)mode, (int)flag), err);
     if (err == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
 }
 
@@ -828,7 +992,7 @@
 
     RESTARTABLE(chown(path, (uid_t)uid, (gid_t)gid), err);
     if (err == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
 }
 
@@ -840,7 +1004,7 @@
 
     RESTARTABLE(lchown(path, (uid_t)uid, (gid_t)gid), err);
     if (err == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
 }
 
@@ -851,7 +1015,7 @@
 
     RESTARTABLE(fchown(filedes, (uid_t)uid, (gid_t)gid), err);
     if (err == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
 }
 
@@ -871,7 +1035,7 @@
 
     RESTARTABLE(utimes(path, &times[0]), err);
     if (err == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
 }
 
@@ -890,7 +1054,7 @@
 
     RESTARTABLE(futimens(filedes, &times[0]), err);
     if (err == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
 }
 
@@ -910,7 +1074,7 @@
     RESTARTABLE(utimensat(fd, path, &times[0], flags), err);
 
     if (err == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
 }
 
@@ -924,7 +1088,7 @@
     /* EINTR not listed as a possible error */
     dir = opendir(path);
     if (dir == NULL) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
     return ptr_to_jlong(dir);
 }
@@ -941,7 +1105,7 @@
     /* EINTR not listed as a possible error */
     dir = (*my_fdopendir_func)((int)dfd);
     if (dir == NULL) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
     return ptr_to_jlong(dir);
 }
@@ -951,7 +1115,7 @@
     DIR* dirp = jlong_to_ptr(dir);
 
     if (closedir(dirp) == -1 && errno != EINTR) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
 }
 
@@ -964,7 +1128,7 @@
     ptr = readdir(dirp);
     if (ptr == NULL) {
         if (errno != 0) {
-            throwUnixException(env, errno);
+            throwUnixException(env, GET_ERRNO());
         }
         return NULL;
     } else {
@@ -983,10 +1147,30 @@
 {
     const char* path = (const char*)jlong_to_ptr(pathAddress);
 
+#ifdef __sun
+    {
+        sysret_t sret;
+        illumos_errno = (int)__systemcall(&sret, SYS_mkdirat, AT_FDCWD, path, (mode_t)mode);
+        if (illumos_errno != 0) {
+            throwUnixException(env, illumos_errno);
+        }
+    }
+#else
+#ifdef __sun
+    {
+        sysret_t sret;
+        illumos_errno = (int)__systemcall(&sret, SYS_mkdirat, AT_FDCWD, path, (mode_t)mode);
+        if (illumos_errno != 0) {
+            throwUnixException(env, illumos_errno);
+        }
+    }
+#else
     /* EINTR not listed as a possible error */
     if (mkdir(path, (mode_t)mode) == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
+#endif
+#endif
 }
 
 JNIEXPORT void JNICALL
@@ -994,11 +1178,20 @@
     jlong pathAddress)
 {
     const char* path = (const char*)jlong_to_ptr(pathAddress);
-
+#ifdef __sun
+    {
+        sysret_t sret;
+        illumos_errno = (int)__systemcall(&sret, SYS_rmdir, path);
+        if (illumos_errno != 0) {
+            throwUnixException(env, illumos_errno);
+        }
+    }
+#else
     /* EINTR not listed as a possible error */
     if (rmdir(path) == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
+#endif
 }
 
 JNIEXPORT void JNICALL
@@ -1009,9 +1202,17 @@
     const char* existing = (const char*)jlong_to_ptr(existingAddress);
     const char* newname = (const char*)jlong_to_ptr(newAddress);
 
+#ifdef __sun
+    {
+        sysret_t sret;
+        illumos_errno = (int)__systemcall(&sret, SYS_linkat, AT_FDCWD, existing, AT_FDCWD, newname, 0);
+        err = (illumos_errno == 0) ? 0 : -1;
+    }
+#else
     RESTARTABLE(link(existing, newname), err);
+#endif
     if (err == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
 }
 
@@ -1021,11 +1222,20 @@
     jlong pathAddress)
 {
     const char* path = (const char*)jlong_to_ptr(pathAddress);
-
+#ifdef __sun
+    {
+        sysret_t sret;
+        illumos_errno = (int)__systemcall(&sret, SYS_unlinkat, AT_FDCWD, path, 0);
+        if (illumos_errno != 0) {
+            throwUnixException(env, illumos_errno);
+        }
+    }
+#else
     /* EINTR not listed as a possible error */
     if (unlink(path) == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
+#endif
 }
 
 JNIEXPORT void JNICALL
@@ -1038,11 +1248,20 @@
         JNU_ThrowInternalError(env, "should not reach here");
         return;
     }
-
+#ifdef __sun
+    {
+        sysret_t sret;
+        illumos_errno = (int)__systemcall(&sret, SYS_unlinkat, (int)dfd, path, (int)flags);
+        if (illumos_errno != 0) {
+            throwUnixException(env, illumos_errno);
+        }
+    }
+#else
     /* EINTR not listed as a possible error */
     if ((*my_unlinkat_func)((int)dfd, path, (int)flags) == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
+#endif
 }
 
 JNIEXPORT void JNICALL
@@ -1051,11 +1270,20 @@
 {
     const char* from = (const char*)jlong_to_ptr(fromAddress);
     const char* to = (const char*)jlong_to_ptr(toAddress);
-
+#ifdef __sun
+    {
+        sysret_t sret;
+        illumos_errno = (int)__systemcall(&sret, SYS_renameat, AT_FDCWD, from, AT_FDCWD, to);
+        if (illumos_errno != 0) {
+            throwUnixException(env, illumos_errno);
+        }
+    }
+#else
     /* EINTR not listed as a possible error */
     if (rename(from, to) == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
+#endif
 }
 
 JNIEXPORT void JNICALL
@@ -1072,7 +1300,7 @@
 
     /* EINTR not listed as a possible error */
     if ((*my_renameat_func)((int)fromfd, from, (int)tofd, to) == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
 }
 
@@ -1085,7 +1313,7 @@
 
     /* EINTR not listed as a possible error */
     if (symlink(target, link) == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
 }
 
@@ -1100,7 +1328,7 @@
     /* EINTR not listed as a possible error */
     int n = readlink(path, target, sizeof(target));
     if (n == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     } else {
         jsize len;
         if (n == sizeof(target)) {
@@ -1129,7 +1357,7 @@
 
     /* EINTR not listed as a possible error */
     if (realpath(path, resolved) == NULL) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     } else {
         jsize len = (jsize)strlen(resolved);
         result = (*env)->NewByteArray(env, len);
@@ -1147,9 +1375,16 @@
     int err;
     const char* path = (const char*)jlong_to_ptr(pathAddress);
 
+#ifdef __sun
+    {
+        sysret_t sret;
+        illumos_errno = (int)__systemcall(&sret, SYS_faccessat, AT_FDCWD, path, (int)amode, 0);
+        return illumos_errno;
+    }
+#else
     RESTARTABLE(access(path, (int)amode), err);
-
     return (err == -1) ? errno : 0;
+#endif
 }
 
 JNIEXPORT void JNICALL
@@ -1170,7 +1405,7 @@
     RESTARTABLE(statvfs(path, &buf), err);
 #endif
     if (err == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     } else {
 #ifdef _AIX
         /* AIX returns ULONG_MAX in buf.f_blocks for the /proc file system. */
@@ -1195,6 +1430,33 @@
     }
 }
 
+JNIEXPORT jlong JNICALL
+Java_sun_nio_fs_UnixNativeDispatcher_pathconf0(JNIEnv* env, jclass this,
+    jlong pathAddress, jint name)
+{
+    long err;
+    const char* path = (const char*)jlong_to_ptr(pathAddress);
+
+    err = pathconf(path, (int)name);
+    if (err == -1) {
+        throwUnixException(env, GET_ERRNO());
+    }
+    return (jlong)err;
+}
+
+JNIEXPORT jlong JNICALL
+Java_sun_nio_fs_UnixNativeDispatcher_fpathconf(JNIEnv* env, jclass this,
+    jint fd, jint name)
+{
+    long err;
+
+    err = fpathconf((int)fd, (int)name);
+    if (err == -1) {
+        throwUnixException(env, errno);
+    }
+    return (jlong)err;
+}
+
 JNIEXPORT void JNICALL
 Java_sun_nio_fs_UnixNativeDispatcher_mknod0(JNIEnv* env, jclass this,
     jlong pathAddress, jint mode, jlong dev)
@@ -1204,7 +1466,7 @@
 
     RESTARTABLE(mknod(path, (mode_t)mode, (dev_t)dev), err);
     if (err == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
 }
 
@@ -1234,7 +1496,7 @@
             /* not found or error */
             if (errno == 0)
                 errno = ENOENT;
-            throwUnixException(env, errno);
+            throwUnixException(env, GET_ERRNO());
         } else {
             jsize len = strlen(p->pw_name);
             result = (*env)->NewByteArray(env, len);
@@ -1285,7 +1547,7 @@
             } else {
                 if (errno == 0)
                     errno = ENOENT;
-                throwUnixException(env, errno);
+                throwUnixException(env, GET_ERRNO());
             }
         } else {
             jsize len = strlen(g->gr_name);
@@ -1331,7 +1593,7 @@
             if (errno != 0 && errno != ENOENT && errno != ESRCH &&
                 errno != EBADF && errno != EPERM)
             {
-                throwUnixException(env, errno);
+                throwUnixException(env, GET_ERRNO());
             }
         } else {
             uid = p->pw_uid;
@@ -1381,7 +1643,7 @@
                     buflen += ENT_BUF_SIZE;
                     retry = 1;
                 } else {
-                    throwUnixException(env, errno);
+                    throwUnixException(env, GET_ERRNO());
                 }
             }
         } else {
@@ -1414,7 +1676,7 @@
 #endif
 
     if (res == (size_t)-1)
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     return (jint)res;
 }
 
@@ -1437,7 +1699,7 @@
 #endif
 
     if (res == -1)
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
 }
 
 JNIEXPORT void JNICALL
@@ -1458,7 +1720,7 @@
 #endif
 
     if (res == -1)
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
 }
 
 JNIEXPORT jint JNICALL
@@ -1479,6 +1741,6 @@
 #endif
 
     if (res == (size_t)-1)
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     return (jint)res;
 }
