#!/bin/sh -x
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
# a tool for automatic rebuild and re-run on source-code modification under linux
# dependencies: inotifywait, find
SolutionDir="`dirname $0`/../"

cd "${SolutionDir}"
while true; do
	if make all; then killall llss; make run-app-trace & fi
	find -name lib -prune -o \( -regex '.*\.[ch]' -print0 \) | xargs -r0 inotifywait -e move
done
