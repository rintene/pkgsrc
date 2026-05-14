--- ./src/java.base/unix/classes/sun/nio/fs/UnixMountEntry.java.orig	2026-04-09 09:37:46.436032483 +0200
+++ ./src/java.base/unix/classes/sun/nio/fs/UnixMountEntry.java	2026-04-09 09:38:15.348844548 +0200
@@ -46,6 +46,13 @@
         return Util.toString(name);
     }
 
+void init(byte[] name, byte[] dir, byte[] fstype, byte[] opts) {
+    this.name = name;
+    this.dir = dir;
+    this.fstype = fstype;
+    this.opts = opts;
+}
+
     String fstype() {
         if (fstypeAsString == null)
             fstypeAsString = Util.toString(fstype);
