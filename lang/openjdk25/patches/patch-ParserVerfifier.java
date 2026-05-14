--- src/java.base/share/classes/jdk/internal/classfile/impl/verifier/ParserVerifier.java.orig	2026-04-08 14:07:33.551843927 +0200
+++ src/java.base/share/classes/jdk/internal/classfile/impl/verifier/ParserVerifier.java	2026-04-08 14:12:20.622219591 +0200
@@ -434,7 +434,8 @@
                 }
                 yield l;
             }
-            case AnnotationValue.OfConstant _, AnnotationValue.OfClass _ -> 2;
+            case AnnotationValue.OfConstant _ -> 2;
+            case AnnotationValue.OfClass unused -> 2;
             case AnnotationValue.OfEnum _ -> 4;
         };
     }
