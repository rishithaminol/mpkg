#!/bin/bash

MPKG="../mpkg"
PROG_NAME="$0"

# if previous tests available, remove them.
if [ -d "root" ] || [ -d "root2" ]; then
	rm -rf root*
fi

mkdir "root" "root2"

"$MPKG" -i "files/ncurses-5.9-1.0.1.amd64.mav" -r root || {
	echo "$PROG_NAME: $MPKG makes some errors. return value = $?"
	exit 1
}
/bin/tar -xf files/data.tar.gz -C root2

# Check hardlink creation
hlink_creation="OK"
while read x; do
	md51=$(md5sum "$x" | awk '{print $1}')
	md52=$(md5sum "$(echo $x | sed 's/^root/root2/')" | awk '{print $1}')

	if [ "$md51" != "$md52" ]; then
		echo "$x $MPKG Hardlink creation was failed"
		hlink_creation="FAIL"
	fi
done <<< "$(./find_hardlinks.sh root)"
echo "Hardlink creation: $hlink_creation"

# Check symlink creation
symlink_creation="OK"
while read x; do
	real_link1=$(readlink $x)
	real_link2=$(readlink $(echo $x | sed 's/^root/root2/'))

	if [ "$real_link1" != "$real_link2" ]; then
		echo "$x $MPKG Symlink creation was failed"
		symlink_creation="FAIL"
	fi
done <<< "$(./find_symlinks.sh root)"
echo "Symlink creation: $symlink_creation"
