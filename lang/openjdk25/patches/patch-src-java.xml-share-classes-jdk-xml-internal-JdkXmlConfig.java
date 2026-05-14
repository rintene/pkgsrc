$NetBSD$

--- src/java.xml/share/classes/jdk/xml/internal/JdkXmlConfig.java.orig
+++ src/java.xml/share/classes/jdk/xml/internal/JdkXmlConfig.java
@@ -54,8 +54,8 @@
 
     // The JDK built-in Catalog
     private static class CatalogHolder {
-        private static final Catalog JDKCATALOG = CatalogManager.catalog(
-                CatalogFeatures.defaults(), URI.create(JDKCATALOG_URL));
+        private static final Catalog JDKCATALOG = initCatalog();
+        private static Catalog initCatalog() { try { return CatalogManager.catalog(CatalogFeatures.defaults(), URI.create(JDKCATALOG_URL)); } catch (Exception e) { return null; } }
     }
 
     /**
