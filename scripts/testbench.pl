#!/usr/bin/perl

use strict;

my $total = 0;
my $passed = 0;

foreach (@ARGV)
{
    $total++;

    chomp (my $result = `./$_ 2>&1`);
    if ($result or $?)
    {
        print "\n--------\n$_ failed: $result\n--------\n";
    }
    else
    {
        print ".";
        $passed++;
    }
}

print "\n--------\nPassed $passed / $total.\n";

