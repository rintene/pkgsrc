#!/bin/sh
set -e
BUILDDIR="$1"
BOOTDIR="$2"
WRKSRC="$3"
MAKE_PROGRAM="$4"
FILESDIR="$5"

echo "POST_PATCH"
cd ${WRKSRC}
rm -rf ${WRKSRC}/tmp/g
echo "Current dir: $(pwd)"
mkdir -p ${WRKSRC}/src/hotspot/os_cpu/solaris_zero/
for p in ${FILESDIR}/../patches/solaris-openjdk/patches-25/*.patch; do
	if grep -q "^--- /tmp/g/" "$p"; then
                patch -p0 -N < "$p" || echo "  Warning: $p failed"
        else
                patch -p1 -N < "$p" || echo "  Warning: $p failed"
        fi
done

ARGS_DIR=${WRKSRC}/make/modules

cat > ${ARGS_DIR}/jdk.compiler/createsymbols_args.txt << 'EOF'
--add-modules
jdk.compiler,jdk.jdeps
--add-exports
jdk.compiler/com.sun.tools.javac.api=ALL-UNNAMED
--add-exports
jdk.compiler/com.sun.tools.javac.code=ALL-UNNAMED
--add-exports
jdk.compiler/com.sun.tools.javac.util=ALL-UNNAMED
--add-exports
jdk.compiler/com.sun.tools.javac.jvm=ALL-UNNAMED
EOF

cat > ${ARGS_DIR}/jdk.javadoc/createsymbols_javadoc_args.txt << 'EOF'
--add-modules
jdk.compiler,jdk.jdeps
--add-exports
jdk.compiler/com.sun.tools.javac.api=ALL-UNNAMED
--add-exports
jdk.compiler/com.sun.tools.javac.code=ALL-UNNAMED
--add-exports
jdk.compiler/com.sun.tools.javac.util=ALL-UNNAMED
--add-exports
jdk.compiler/com.sun.tools.javac.jvm=ALL-UNNAMED
EOF

patch -p0 < /tmp/last-patch.c || echo "UNIX NATIVE DISPATCHER FAILED"

 patch -p0 -N -l << 'EOF'
--- src/java.base/unix/native/libnio/fs/UnixNativeDispatcher.c.orig  2026-04-18 14:39:20.307230908 +0200
+++ src/java.base/unix/native/libnio/fs/UnixNativeDispatcher.c  2026-04-18 14:38:53.370390265 +0200
@@ -494,14 +494,26 @@
     jlong pathAddress, jlong modeAddress)
 {
     FILE* fp = NULL;
+    int saved_errno;
     const char* path = (const char*)jlong_to_ptr(pathAddress);
     const char* mode = (const char*)jlong_to_ptr(modeAddress);
-
     do {
         fp = fopen(path, mode);
-    } while (fp == NULL && errno == EINTR);
-
+        saved_errno = errno;
+    } while (fp == NULL && saved_errno == EINTR);
     if (fp == NULL) {
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
         throwUnixException(env, errno);
     }

@@ -519,7 +531,7 @@
      * is completely written even if fclose() failed.
      */
     if (fclose(fp) == EOF && errno != EINTR) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
 }

@@ -1427,7 +1439,7 @@

     err = pathconf(path, (int)name);
     if (err == -1) {
-        throwUnixException(env, errno);
+        throwUnixException(env, GET_ERRNO());
     }
     return (jlong)err;
 }

EOF

