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
# For now, build for Windows and Mac uses VSCode MSBuild and XCode tools
.PHONY: all clean program mods run

# default value for command-line arguments (like make BUILD_TYPE=Release)
BUILD_TYPE ?= Debug

all: program mods

program:
	$(MAKE) -C program BUILD_TYPE=$(BUILD_TYPE)

mods:
	$(MAKE) -C mods BUILD_TYPE=$(BUILD_TYPE)

clean:
	$(MAKE) -C program clean BUILD_TYPE=$(BUILD_TYPE)
	$(MAKE) -C mods clean BUILD_TYPE=$(BUILD_TYPE)

run: all
	$(MAKE) -C program run BUILD_TYPE=$(BUILD_TYPE)
