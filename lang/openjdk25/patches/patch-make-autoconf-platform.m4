$NetBSD$

--- make/autoconf/platform.m4.orig
+++ make/autoconf/platform.m4
@@ -210,6 +210,14 @@
       VAR_OS=bsd
       VAR_OS_TYPE=unix
       ;;
+    *sunos*)
+	VAR_OS=bsd
+	VAR_OS_TYPE=unix
+	;;
+	*solaris*)
+	VAR_OS=solaris
+	VAR_OS_TYPE=unix
+;;
     *cygwin*)
       VAR_OS=windows
       VAR_OS_ENV=windows.cygwin
