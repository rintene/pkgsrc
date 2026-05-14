#!/bin/sh
unset LD_LIBRARY_PATH_64
export LD_LIBRARY_PATH_64=@BUILDDIR@/buildjdk/jdk/lib
export LD_LIBRARY_PATH=@BUILDDIR@/buildjdk/jdk/lib:@BOOTDIR@/lib
NEWARGS="-Djdk.xml.jdkcatalog.resolve=continue"
for arg in "$@"; do
    case "$arg" in
        -Xshare:*|-XX:SharedArchiveFile=*) ;;
        -Xmx2048M) NEWARGS="$NEWARGS -Xmx512M" ;;
        -cp) NEWARGS="$NEWARGS -cp" ;;
        *)
            if [ "$prev" = "-cp" ]; then
                NEWARGS="$NEWARGS /tmp/java.xml.jar:$arg"
            else
                NEWARGS="$NEWARGS $arg"
            fi
            ;;
    esac
    prev="$arg"
done
exec @BUILDDIR@/buildjdk/jdk/bin/java $NEWARGS
