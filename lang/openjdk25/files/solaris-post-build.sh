#!/bin/sh
set -e
BUILDDIR="$1"
BOOTDIR="$2"

# Копируем библиотеки в /opt/local/lib для rpath
for lib in ${BUILDDIR}/buildjdk/jdk/lib/lib*.so; do
    name=$(basename $lib)
    [ ! -f /opt/local/lib/$name ] && cp $lib /opt/local/lib/$name || true
done
cp ${BUILDDIR}/buildjdk/jdk/lib/server/libjvm.so /opt/local/lib/libjvm.so || true

# Симлинки в bootstrap/lib
for lib in libjava.so libjimage.so libnio.so libzip.so libnet.so \
           libverify.so libjsig.so libfallbackLinker.so libsyslookup.so; do
    [ ! -f ${BOOTDIR}/lib/$lib ] && \
        ln -s ${BUILDDIR}/buildjdk/jdk/lib/$lib ${BOOTDIR}/lib/$lib || true
done
mkdir -p ${BOOTDIR}/lib/server
[ ! -f ${BOOTDIR}/lib/server/libjvm.so ] && \
    ln -s ${BUILDDIR}/buildjdk/jdk/lib/server/libjvm.so \
          ${BOOTDIR}/lib/server/libjvm.so || true

# Создаём java.base.jmod
${BOOTDIR}/bin/jmod create \
    --class-path ${BUILDDIR}/jdk/modules/java.base \
    --libs ${BUILDDIR}/support/modules_libs/java.base \
    --cmds ${BUILDDIR}/support/modules_cmds/java.base \
    --config ${BUILDDIR}/support/modules_conf/java.base \
    --module-version 25.0.3 \
    --target-platform solaris-amd64 \
    ${BUILDDIR}/images/jmods/java.base.jmod

# Создаём остальные недостающие jmod
for mod in jdk.compiler jdk.javadoc jdk.jlink jdk.jdeps jdk.jshell \
           jdk.jartool jdk.jsobject jdk.naming.rmi jdk.net jdk.nio.mapmode \
           jdk.sctp jdk.security.jgss jdk.unsupported jdk.unsupported.desktop \
           jdk.xml.dom jdk.zipfs jdk.security.auth; do
    cmds_dir=${BUILDDIR}/support/modules_cmds/$mod
    libs_dir=${BUILDDIR}/support/modules_libs/$mod
    cmd="${BOOTDIR}/bin/jmod create --class-path ${BUILDDIR}/jdk/modules/$mod --module-version 25.0.3"
    [ -d $cmds_dir ] && cmd="$cmd --cmds $cmds_dir"
    [ -d $libs_dir ] && cmd="$cmd --libs $libs_dir"
    eval "$cmd ${BUILDDIR}/images/jmods/$mod.jmod"
done

# Финальный jlink
MODULES=$(ls ${BUILDDIR}/images/jmods | sed 's/\.jmod$//' | tr '\n' ',' | sed 's/,$//')
${BOOTDIR}/bin/jlink \
    --module-path ${BUILDDIR}/images/jmods \
    --add-modules $MODULES \
    --output ${BUILDDIR}/images/jdk
