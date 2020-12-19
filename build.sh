#!/bin/bash
## Linbo GUI global build srcipt
## Copyright (C) 2020  Dorian Zedler <dorian@itsblue.de>
##
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU Affero General Public License as published
## by the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Affero General Public License for more details.
##
## You should have received a copy of the GNU Affero General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.
##
## This script will build the linbo GUI for 32 and 64 bit.

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $DIR

cd build

Architecures=(64 32)

for ARCH in "${Architecures[@]}"
do
    echo "Building linbo_gui7 for $ARCH"

	# build for 64-Bit
	mkdir buildGUI$ARCH

	# create the buildfile
	cp build_Qt_and_Gui_generic buildGUI$ARCH/build.sh
	cd buildGUI$ARCH

	sed -i "s/## $ARCH: //g" build.sh

	# remove all other architectures to prevent errors
	for NOT_ARCH in "${Architecures[@]}"
	do
		sed -i "/## $NOT_ARCH: /d" build.sh
	done

	./build.sh "$1" "$2"

	if [[ $? -ne 0 ]]; then
	   echo "There was an error when building linbo_gui for $ARCH!"
	   exit 1
	fi
	
	# remove old linbofs folder
	rm -rf ../linbofs$ARCH
	
	# copy linbofs folder in place
	cp -r linbofs ../linbofs$ARCH

	cd ..

	# copy icons into linbofs folder
	cp -r ./icons  ./linbofs$ARCH

    sleep 10

done

mv linbofs32 linbofs

echo "--------------------------------------"
echo "- linbo_gui7 was built successfully! -"
echo "-   Now building debian packages     -"
echo "--------------------------------------"

cd ..
./debian/mkdeb.sh