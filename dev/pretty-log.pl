#!/usr/bin/env perl
#
# This file is part of LLSS.
#
# LLSS is free software: you can redistribute it and/or modify it under the terms of the
# Affero GNU General Public License as published by the Free Software Foundation,
# either version 3 of the License, or (at your option) any later version.
#
# LLSS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with LLSS.
# If not, see <https://www.gnu.org/licenses/>. See LICENSE file at root of this git repo.
#
# Copyright 2025 ludolpif <ludolpif@gmail.com>
#
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
