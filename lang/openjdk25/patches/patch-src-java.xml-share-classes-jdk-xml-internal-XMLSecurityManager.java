$NetBSD$

--- src/java.xml/share/classes/jdk/xml/internal/XMLSecurityManager.java.orig
+++ src/java.xml/share/classes/jdk/xml/internal/XMLSecurityManager.java
@@ -32,6 +32,7 @@
 import java.util.Objects;
 import java.util.concurrent.CopyOnWriteArrayList;
 import java.util.stream.Collectors;
+import javax.xml.catalog.Catalog;
 import javax.xml.catalog.CatalogManager;
 import javax.xml.catalog.CatalogResolver;
 import javax.xml.catalog.CatalogResolver.NotFoundAction;
@@ -327,8 +328,9 @@
      */
     public CatalogResolver getJDKCatalogResolver() {
         String resolve = getLimitValueAsString(Limit.JDKCATALOG_RESOLVE);
-        return CatalogManager.catalogResolver(
-                JdkXmlConfig.getInstance(false).getJdkCatalog(), toActionType(resolve));
+        Catalog cat = JdkXmlConfig.getInstance(false).getJdkCatalog();
+        if (cat == null) return null;
+        return CatalogManager.catalogResolver(cat, toActionType(resolve));
     }
 
     // convert the string value of the RESOLVE property to the corresponding
