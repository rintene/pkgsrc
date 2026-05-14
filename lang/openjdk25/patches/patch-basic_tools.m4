--- make/autoconf/basic_tools.m4.orig	2026-04-11 09:08:54.666733843 +0200
+++ make/autoconf/basic_tools.m4	2026-04-11 09:10:03.983601172 +0200
@@ -378,8 +378,9 @@
 
   # Check if it's a GNU date compatible version
   AC_MSG_CHECKING([if date is a GNU compatible version])
+  check_date_utc=`$DATE --utc --date="@0" +"%Y" 2>/dev/null`  
   check_date=`$DATE --version 2>&1 | $GREP "GNU\|BusyBox\|uutils"`
-  if test "x$check_date" != x; then
+  if  test "x$check_date" != x || test "x$check_date_utc" = x1970; then
     AC_MSG_RESULT([yes])
     IS_GNU_DATE=yes
   else
