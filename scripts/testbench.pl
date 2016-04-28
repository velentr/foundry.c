#!/usr/bin/perl

use strict;

my @results;

foreach (@ARGV)
{
    print "------\nTesting $_:\n";
    push @results, system "./$_";
    print "Done testing $_.\n";
}

print "\n------\n";
my $failcount = 0;
for my $i (0 .. $#results)
{
    if ($results[$i] != 0)
    {
        print "Test $ARGV[$i] failed.\n";
        $failcount++;
    }
}

my $count = $#results + 1;
print "\n------\nFailed $failcount / $count tests.\n";
