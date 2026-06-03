--- /data/pkgsrc/devel/openjdk25/work/jdk25u-jdk-25.0.3-ga/src/jdk.compiler/share/classes/com/sun/tools/javac/comp/Check.java	2026-04-17 21:08:13.000000000 +0200
+++ /root/jdk25u/src/jdk.compiler/share/classes/com/sun/tools/javac/comp/Check.java	2026-05-21 11:55:37.047748277 +0200
@@ -1321,7 +1321,7 @@
     private void warnOnExplicitStrictfp(JCTree tree) {
         deferredLintHandler.push(tree);
         try {
-            deferredLintHandler.report(_ -> lint.logIfEnabled(tree.pos(), LintWarnings.Strictfp));
+            deferredLintHandler.report(unused -> lint.logIfEnabled(tree.pos(), LintWarnings.Strictfp));
         } finally {
             deferredLintHandler.pop();
         }
@@ -4126,7 +4126,7 @@
             int opc = ((OperatorSymbol)operator).opcode;
             if (opc == ByteCodes.idiv || opc == ByteCodes.imod
                 || opc == ByteCodes.ldiv || opc == ByteCodes.lmod) {
-                deferredLintHandler.report(_ -> lint.logIfEnabled(pos, LintWarnings.DivZero));
+                deferredLintHandler.report(unused -> lint.logIfEnabled(pos, LintWarnings.DivZero));
             }
         }
     }
@@ -4139,7 +4139,7 @@
      */
     void checkLossOfPrecision(final DiagnosticPosition pos, Type found, Type req) {
         if (found.isNumeric() && req.isNumeric() && !types.isAssignable(found, req)) {
-            deferredLintHandler.report(_ ->
+            deferredLintHandler.report(unused ->
                 lint.logIfEnabled(pos, LintWarnings.PossibleLossOfPrecision(found, req)));
         }
     }
@@ -4339,7 +4339,7 @@
                             // Warning may be suppressed by
                             // annotations; check again for being
                             // enabled in the deferred context.
-                            deferredLintHandler.report(_ ->
+                            deferredLintHandler.report(unused ->
                                 lint.logIfEnabled(pos, LintWarnings.MissingExplicitCtor(c, pkg, modle)));
                         } else {
                             return;
@@ -4674,7 +4674,7 @@
 
     void checkModuleExists(final DiagnosticPosition pos, ModuleSymbol msym) {
         if (msym.kind != MDL) {
-            deferredLintHandler.report(_ ->
+            deferredLintHandler.report(unused ->
                 lint.logIfEnabled(pos, LintWarnings.ModuleNotFound(msym)));
         }
     }
@@ -4682,14 +4682,14 @@
     void checkPackageExistsForOpens(final DiagnosticPosition pos, PackageSymbol packge) {
         if (packge.members().isEmpty() &&
             ((packge.flags() & Flags.HAS_RESOURCE) == 0)) {
-            deferredLintHandler.report(_ ->
+            deferredLintHandler.report(unused ->
                 lint.logIfEnabled(pos, LintWarnings.PackageEmptyOrNotFound(packge)));
         }
     }
 
     void checkModuleRequires(final DiagnosticPosition pos, final RequiresDirective rd) {
         if ((rd.module.flags() & Flags.AUTOMATIC_MODULE) != 0) {
-            deferredLintHandler.report(_ -> {
+            deferredLintHandler.report(unused -> {
                 if (rd.isTransitive() && lint.isEnabled(LintCategory.REQUIRES_TRANSITIVE_AUTOMATIC)) {
                     log.warning(pos, LintWarnings.RequiresTransitiveAutomatic);
                 } else {
