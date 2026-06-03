--- /data/pkgsrc/devel/openjdk25/work/jdk25u-jdk-25.0.3-ga/src/jdk.javadoc/share/classes/jdk/javadoc/internal/doclets/formats/html/ModuleWriter.java	2026-04-17 21:08:13.000000000 +0200
+++ /root/jdk25u/src/jdk.javadoc/share/classes/jdk/javadoc/internal/doclets/formats/html/ModuleWriter.java	2026-05-21 11:55:44.638043533 +0200
@@ -622,14 +622,14 @@
                 } else {
                     String aepText = resources.getText("doclet.Indirect_Exports_Summary");
                     var aepTable = getTable2(Text.of(aepText), indirectPackagesHeader);
-                    addIndirectPackages(aepTable, indirectPackages, _ -> true);
+                    addIndirectPackages(aepTable, indirectPackages, unused -> true);
                     section.add(aepTable);
                 }
             }
             if (display(indirectOpenPackages)) {
                 String aopText = resources.getText("doclet.Indirect_Opens_Summary");
                 var aopTable = getTable2(Text.of(aopText), indirectPackagesHeader);
-                addIndirectPackages(aopTable, indirectOpenPackages, _ -> true);
+                addIndirectPackages(aopTable, indirectOpenPackages, unused -> true);
                 section.add(aopTable);
             }
             summariesList.add(HtmlTree.LI(section));
