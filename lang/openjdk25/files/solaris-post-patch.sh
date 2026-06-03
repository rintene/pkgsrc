#!/bin/sh
set -e
BUILDDIR="$1"
BOOTDIR="$2"
WRKSRC="$3"
MAKE_PROGRAM="$4"
FILESDIR="$5"

cd ${WRKSRC}
mkdir -p ${WRKSRC}/src/hotspot/os_cpu/solaris_zero/
for p in ${FILESDIR}/../patches/solaris-openjdk/patches-25/*.patch; do
	if grep -q "^--- /tmp/g/" "$p"; then
                patch -p0 -N < "$p" || echo "  Warning: $p failed"
        else
                patch -p1 -N < "$p" || echo "  Warning: $p failed"
        fi
done

ARGS_DIR=${WRKSRC}/make/modules

cat > ${ARGS_DIR}/jdk.compiler/createsymbols_args.txt << 'EOF'
--add-modules
jdk.compiler,jdk.jdeps
--add-exports
jdk.compiler/com.sun.tools.javac.api=ALL-UNNAMED
--add-exports
jdk.compiler/com.sun.tools.javac.code=ALL-UNNAMED
--add-exports
jdk.compiler/com.sun.tools.javac.util=ALL-UNNAMED
--add-exports
jdk.compiler/com.sun.tools.javac.jvm=ALL-UNNAMED
EOF

cat > ${ARGS_DIR}/jdk.javadoc/createsymbols_javadoc_args.txt << 'EOF'
--add-modules
jdk.compiler,jdk.jdeps
--add-exports
jdk.compiler/com.sun.tools.javac.api=ALL-UNNAMED
--add-exports
jdk.compiler/com.sun.tools.javac.code=ALL-UNNAMED
--add-exports
jdk.compiler/com.sun.tools.javac.util=ALL-UNNAMED
--add-exports
jdk.compiler/com.sun.tools.javac.jvm=ALL-UNNAMED
EOF
