#!/bin/bash

# Quit if an error occurs
set -e
SYSTEM="$(uname -a | grep -io manjaro)"
if $SYSTEM;
then 
echo "Not on Manjaro System. Exiting..."
exit 1;
fi


# Everything should be done in a fresh repo pulled from master and in /tmp
STARTDIR=$(pwd)
#copy google analytics key to tmp
cp ./analyticsConfig.h /tmp/analyticsConfig.h 

cd /tmp/
NAME=gigamesh$(uuidgen)
git clone --recurse-submodules https://gitlab.com/fcgl/GigaMesh.git $NAME
wait
# Make sure all submodules are up to date!
cd $NAME
git submodule foreach git pull origin master
cd ..
# end update of submodule
wait
cd $NAME

#uncomment to specifiy commit to build
#git reset 937b4ca963ade717e1 --hard

#git checkout develop

#copy key from tmp to the actual position 
cp ../analyticsConfig.h ./gui/src/analyticsConfig.h 

# Get NUM_PROCESSORS and CURRENT_VERSION
NUM_PROCESSORS=$(nproc)
if [ $# -eq 1 ]; then
	CURRENT_VERSION=$1
elif [ $# -eq 0 ]; then
	CURRENT_VERSION=$(git log -1 --format=%ci | cut -b3,4,6,7,9,10)
else
	echo "ERR: parameters must number exactly 0 or 1"
	exit 1
fi

# Change to subdirectory
cd packaging/arch

# Update Version in PKGBUILD
sed -i "s/pkgver=.*/pkgver=$CURRENT_VERSION/g" PKGBUILD

# Create symlink to source and to changelog
if [ ! -h src ]; then
        ln -s ../../ src
fi
if [ ! -h gigamesh.changelog ]; then
        ln -s ../../CHANGELOG
fi
cd ../..

# cmake everything
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -G"Unix Makefiles"
make -j
find . -type f -executable -not -name \*.sh -exec strip {} \;

cd ..

cd packaging/arch
# Build Manjaro package
which makepkg
if [ $? == 0 ]; then
	# We enforce the package build e.g. today's package has to be redone.
	makepkg -f -L
	rm -R pkg
else
	echo \'makepg\' missing - seems this is not a Manjaro system.
fi

# Step out of the subdirectory (extension in older version was .xz, now: .zst)
echo "Copying gigamesh-${CURRENT_VERSION}-*.pkg.tar.zst to $STARTDIR"
mv gigamesh-${CURRENT_VERSION}-*.pkg.tar.zst $STARTDIR
cd ..
