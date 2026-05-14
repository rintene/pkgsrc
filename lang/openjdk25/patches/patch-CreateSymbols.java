--- make/langtools/src/classes/build/tools/symbolgenerator/CreateSymbols.java.orig	2026-05-06 11:11:59.229006635 +0200
+++ make/langtools/src/classes/build/tools/symbolgenerator/CreateSymbols.java	2026-05-06 11:15:28.218584495 +0200
@@ -2186,7 +2186,7 @@
         switch (attr) {
             case AnnotationDefaultAttribute a ->
                     ((MethodDescription) feature).annotationDefaultValue = convertElementValue(a.defaultValue());
-            case DeprecatedAttribute _ -> feature.deprecated = true;
+            case DeprecatedAttribute ignore66 -> feature.deprecated = true;
             case ExceptionsAttribute a -> ((MethodDescription) feature).thrownTypes = a.exceptions().stream().map(ClassEntry::asInternalName).collect(Collectors.toList());
             case InnerClassesAttribute a -> {
                 if (feature instanceof ModuleHeaderDescription)
@@ -2207,8 +2207,11 @@
                 var f = (FieldDescription) feature;
                 f.constantValue = convertConstantValue(a.constant(), f.descriptor);
             }
-            case SourceFileAttribute _, BootstrapMethodsAttribute _, CodeAttribute _, SyntheticAttribute _ -> {}
-            case EnclosingMethodAttribute _ -> {
+            case SourceFileAttribute ignored1 -> {}
+            case BootstrapMethodsAttribute ignored2 -> {}
+            case CodeAttribute ignored3 -> {}
+            case SyntheticAttribute ignored4 -> {}
+            case EnclosingMethodAttribute ignored5 -> {
                 return false;
             }
             case RuntimeVisibleParameterAnnotationsAttribute a -> ((MethodDescription) feature).runtimeParameterAnnotations = parameterAnnotations2Description(a.parameterAnnotations());
@@ -2236,7 +2239,7 @@
                     }
                 }).collect(Collectors.toList());
             }
-            case ModuleHashesAttribute _ -> {}
+            case ModuleHashesAttribute ignored7 -> {}
             case NestHostAttribute a -> ((ClassHeaderDescription) feature).nestHost = a.nestHost().asInternalName();
             case NestMembersAttribute a -> ((ClassHeaderDescription) feature).nestMembers = a.nestMembers().stream().map(ClassEntry::asInternalName).collect(Collectors.toList());
             case RecordAttribute a -> {
@@ -2333,7 +2336,7 @@
                     desc.targetInfo.put("typeParameterIndex", tpbt.typeParameterIndex());
                     desc.targetInfo.put("boundIndex", tpbt.boundIndex());
                 }
-                case TypeAnnotation.EmptyTarget _ -> {
+                case TypeAnnotation.EmptyTarget ignored8 -> {
                     // nothing to write
                 }
                 case TypeAnnotation.FormalParameterTarget fpt -> desc.targetInfo.put("formalParameterIndex", fpt.formalParameterIndex());
