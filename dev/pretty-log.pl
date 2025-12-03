#!/usr/bin/env perl
use strict;
use warnings;
my $tick0_wallclock;
while (<>) {
	$tick0_wallclock=$1 if /tick0_wallclock==(\d+)$/;
       	if ($tick0_wallclock and /^(.....) (\d+) (.*)$/) {
		my $hires_ts = $tick0_wallclock+$2;
		my $ts = $hires_ts/1e9;
		my $ms = $hires_ts/1e6%1e3;
		my $us = $hires_ts/1e3%1e3;
		my ($sec, $min, $hr, $day, $mon, $year) = localtime($ts);
		printf("%s %02d:%02d:%02d.%03d_%03d %s\n", $1, $hr, $min, $sec, $ms, $us, $3);
	} else {
		print;
	}
}
