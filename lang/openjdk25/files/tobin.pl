#!/usr/bin/perl
use strict;
binmode(STDIN);
my @bytes;
while (read(STDIN, my $byte, 1)) {
    push @bytes, unpack('C', $byte);
}
print join(', ', @bytes) . ",\n";
