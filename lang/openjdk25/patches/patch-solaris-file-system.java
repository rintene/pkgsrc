--- /dev/null	2026-04-09 09:42:17.000000000 +0200
+++ ./src/java.base/solaris/classes/sun/nio/fs/SolarisFileSystem.java	2026-04-09 09:40:48.821281044 +0200
@@ -0,0 +1,54 @@
+package sun.nio.fs;
+
+import java.io.BufferedReader;
+import java.io.FileReader;
+import java.io.IOException;
+import java.nio.file.*;
+import java.util.*;
+
+class SolarisFileSystem extends UnixFileSystem {
+    SolarisFileSystem(UnixFileSystemProvider provider, String dir) {
+        super(provider, dir);
+    }
+
+    @Override
+    public WatchService newWatchService() throws IOException {
+        return new PollingWatchService();
+    }
+
+    @Override
+    public Set<String> supportedFileAttributeViews() {
+        return new HashSet<>(UnixFileSystem.standardFileAttributeViews());
+    }
+
+    @Override
+    void copyNonPosixAttributes(int ofd, int nfd) {
+    }
+
+    @Override
+    List<UnixMountEntry> getMountEntries() {
+    ArrayList<UnixMountEntry> entries = new ArrayList<>();
+    try (BufferedReader br = new BufferedReader(
+            new java.io.FileReader("/etc/mnttab",
+                java.nio.charset.StandardCharsets.UTF_8))) {
+        String line;
+        while ((line = br.readLine()) != null) {
+            if (line.isEmpty() || line.startsWith("#")) continue;
+            String[] f = line.split("\t", -1);
+            if (f.length < 4) continue;
+            UnixMountEntry e = new UnixMountEntry();
+            e.init(f[0].getBytes(), f[1].getBytes(),
+                   f[2].getBytes(), f[3].getBytes());
+            entries.add(e);
+        }
+    } catch (IOException x) {
+        // ignore
+    }
+    return entries;
+}
+
+    @Override
+    FileStore getFileStore(UnixMountEntry entry) throws IOException {
+        return new SolarisFileStore(this, entry);
+    }
+}
