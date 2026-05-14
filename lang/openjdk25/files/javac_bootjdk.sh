#!/bin/sh
export LD_LIBRARY_PATH_64=/opt/local/java/openjdk21/lib
exec /opt/local/java/openjdk21/bin/javac "$@"
