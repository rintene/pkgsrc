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

export JAVA_TOOL_OPTIONS="-XX:+UseSerialGC -Xmx256m"
LD_LIBRARY_PATH=${BOOTDIR}/lib ${MAKE_PROGRAM}  build/solaris-x86_64-zero-release/buildjdk/jdk/lib/libjli.so 2>/dev/null || true
unset LD_LIBRARY_PATH_64
LD_LIBRARY_PATH=${BOOTDIR}/lib ${MAKE_PROGRAM} create-buildjdk

unset LD_LIBRARY_PATH_64

export LD_LIBRARY_PATH=${BUILDDIR}/buildjdk/jdk/lib:${BUILDDIR}/buildjdk/jdk/lib/zero:${BUILDDIR}/jdk/lib:${BUILDDIR}/jdk/lib/zero
${MAKE_PROGRAM} -j1 DISABLE_SJAVAC=true ENABLE_GENERATE_CLASSLIST=false JAVAC_CMD=${FILESDIR}/javac_bootjdk.sh product-images
