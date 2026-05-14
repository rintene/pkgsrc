#!/bin/sh
set -e
BUILDDIR="$1"
BOOTDIR="$2"
WRKSRC="$3"
MAKE_PROGRAM="$4"
FILESDIR="$5"

# Создаём java.xml.jar из bootstrap jmods
mkdir -p ${WRKSRC}/../jmod_extracted
cd ${WRKSRC}/../jmod_extracted
unzip -qo ${BOOTDIR}/jmods/java.xml.jmod 'classes/*' 2>/dev/null || true
cd ${WRKSRC}/../jmod_extracted/classes
zip -qr /tmp/java.xml.jar . 2>/dev/null || true

mkdir -p ${WRKSRC}/../jmod_localedata
cd ${WRKSRC}/../jmod_localedata
unzip -qo ${BOOTDIR}/jmods/jdk.localedata.jmod 'classes/sun/text/resources/*' 2>/dev/null || true
cd ${WRKSRC}/../jmod_localedata/classes
zip -qr /tmp/java.xml.jar sun/text/resources/ 2>/dev/null || true

cd ${WRKSRC}

mkdir -p ${WRKSRC}/src/hotspot/os/solaris
cp ${FILESDIR}/c1_globals_solaris.hpp ${WRKSRC}/src/hotspot/os/solaris

find ${WRKSRC}/src/hotspot/os/posix -name "*.hpp" |
	while read f; do base=$(basename $f | sed 's/posix/solaris/g'); 
 		if [ ! -f ${WRKSRC}/src/hotspot/os/solaris/$base ]; 
 			then echo "MISSING: $base"; 
 			sed 's/POSIX/SOLARIS/g; s/posix/solaris/g' $f > ${WRKSRC}/src/hotspot/os/solaris/$base;
 		fi;
done

SRC=${WRKSRC}/src/hotspot/os_cpu/linux_x86
DEST=${WRKSRC}/src/hotspot/os_cpu/solaris_x86
mkdir -p $DEST
mkdir -p $DEST/gc
mkdir -p $DEST/gc/z

cd ${WRKSRC}

grep -Rl "\<_ *->" ${WRKSRC}/src/hotspot | xargs gsed -i 's/_ *->/unused ->/g'
grep -Rl "\<_ *->" ${WRKSRC}/src/jdk.compiler | xargs gsed -i 's/_ *->/unused ->/g'
grep -Rl "\<_ *->" ${WRKSRC}/src/jdk.javadoc | xargs gsed -i 's/_ *->/unused ->/g'

cd ${WRKSRC}

grep -q '__sun' ${WRKSRC}/src/hotspot/share/utilities/macros.hpp || patch -p0 -N << 'EOF'
--- src/hotspot/share/utilities/macros.hpp
+++ src/hotspot/share/utilities/macros.hpp
@@ -394,6 +394,9 @@
 #ifdef LINUX
 #define LINUX_ONLY(code) code
 #define NOT_LINUX(code)
+#elif defined(__sun)
+#define LINUX_ONLY(code)
+#define NOT_LINUX(code) code
 #else
 #define LINUX_ONLY(code)
 #define NOT_LINUX(code) code
EOF
grep -q '__sun' ${WRKSRC}/src/hotspot/share/runtime/os.cpp || patch -p0 -N << 'EOF'
--- src/hotspot/share/runtime/os.cpp
+++ src/hotspot/share/runtime/os.cpp
@@ -183,7 +183,7 @@
   // No offset when dealing with UTC
   time_t UTC_to_local = 0;
   if (!utc) {
-#if (defined(_ALLBSD_SOURCE) || defined(_BSD_SOURCE)) && !defined(AIX)
+#if (defined(_ALLBSD_SOURCE) || defined(_BSD_SOURCE)) && !defined(AIX) && !defined(__sun)
     UTC_to_local = -(time_struct.tm_gmtoff);
 #elif defined(_WINDOWS)
     long zone;
EOF
grep -q 'get_fpu_control_word' ${WRKSRC}/src/hotspot/os/solaris/os_solaris.hpp || patch -p0 -N << 'EOF'
--- src/hotspot/os/solaris/os_solaris.hpp
+++ src/hotspot/os/solaris/os_solaris.hpp
@@ -168,6 +168,9 @@
   static sigset_t* unblocked_signals();
   static sigset_t* vm_signals();

+  static int get_fpu_control_word();
+  static void set_fpu_control_word(int fpu_control);
+
   // %%% Following should be promoted to os.hpp:
   // Trace number of created threads
   static          jint  _os_thread_limit;
EOF

grep -q '__illumos__' ${WRKSRC}/src/hotspot/os/posix/signals_posix.cpp || patch -p0 -N << 'EOF'
--- src/hotspot/os/posix/signals_posix.cpp
+++ src/hotspot/os/posix/signals_posix.cpp
@@ -548,7 +548,7 @@
 #define JVM_HANDLE_XXX_SIGNAL JVM_handle_aix_signal
 #elif defined(LINUX)
 #define JVM_HANDLE_XXX_SIGNAL JVM_handle_linux_signal
-#elif defined(SOLARIS)
+#elif defined(SOLARIS) || defined(__sun) || defined(__illumos__)
 #define JVM_HANDLE_XXX_SIGNAL JVM_handle_solaris_signal
 #else
 #error who are you?
EOF

grep -q 'Usun' ${WRKSRC}/make/modules/java.base/gensrc/GensrcMisc.gmk || patch -p0 -N -l << 'EOF'
--- make/modules/java.base/gensrc/GensrcMisc.gmk
+++ make/modules/java.base/gensrc/GensrcMisc.gmk
@@ -110,7 +110,7 @@
        $(call MakeDir, $(@D))
        $(call ExecuteWithLog, $(SUPPORT_OUTPUTDIR)/gensrc/java.base/_$(@F), \
            ( $(AWK) '/@@END_COPYRIGHT@@/{exit}1' $< && \
-             $(CPP) $(CPP_FLAGS) $(SYSROOT_CFLAGS) $(CFLAGS_JDKLIB) $(CPP_FILEPREFIX) $< \
+             $(CPP) -Usun $(CPP_FLAGS) $(SYSROOT_CFLAGS) $(CFLAGS_JDKLIB) $(CPP_FILEPREFIX) $< \
                  2> >($(GREP) -v '^$(<F)$$' >&2) \
                  | $(AWK) '/@@START_HERE@@/,0' \
                  |  $(SED) -e 's/@@START_HERE@@/\/\/ AUTOMATICALLY GENERATED FILE - DO NOT EDIT/' \
EOF

export JAVA_TOOL_OPTIONS="-XX:+UseSerialGC -Xmx256m"
LD_LIBRARY_PATH=${BOOTDIR}/lib ${MAKE_PROGRAM}  build/solaris-x86_64-zero-release/buildjdk/jdk/lib/libjli.so 2>/dev/null || true
unset LD_LIBRARY_PATH_64
LD_LIBRARY_PATH=${BOOTDIR}/lib ${MAKE_PROGRAM} create-buildjdk

unset LD_LIBRARY_PATH_64

export LD_LIBRARY_PATH=${BUILDDIR}/buildjdk/jdk/lib:${BUILDDIR}/buildjdk/jdk/lib/zero:${BUILDDIR}/jdk/lib:${BUILDDIR}/jdk/lib/zero
${MAKE_PROGRAM} -j1 DISABLE_SJAVAC=true ENABLE_GENERATE_CLASSLIST=false JAVAC_CMD=${FILESDIR}/javac_bootjdk.sh product-images
