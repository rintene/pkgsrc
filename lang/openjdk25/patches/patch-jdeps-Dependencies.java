--- src/jdk.jdeps/share/classes/com/sun/tools/jdeps/Dependencies.java.orig	2026-02-04 03:45:26.000000000 +0100
+++ src/jdk.jdeps/share/classes/com/sun/tools/jdeps/Dependencies.java	2026-04-09 10:09:09.576697302 +0200
@@ -699,7 +699,8 @@
                         }
                     }
                     case Signature.ArrayTypeSig at -> scan(at.componentSignature());
-                    case Signature.BaseTypeSig _, Signature.TypeVarSig _ -> {}
+                    case Signature.BaseTypeSig unused -> {}
+		     case  Signature.TypeVarSig unused -> {}
                 }
             }
         }
