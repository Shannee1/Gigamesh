#!/bin/sh
#sed -i "/unreleasd/c\Version $CURRENT_VERSION" packaging/gigamesh.changelog
# Exit immediately if a command exits with a non-zero status.
set -e
STARTDIR=$(pwd)
# Main variables
P_NAME="gigamesh"
cd ..
P_VERSION=$(git log -1 --format=%ci | cut -b3,4,6,7,9,10)
cd packaging

if test "$DEBSIGN_KEYID"; then
	# only build a source package, and sign it
	DPKG_BUILPACKAGE_OPTS="-sa -S -k$DEBSIGN_KEYID"
	BUILDER_ROLE='Uploaders'
	test "$DEBEMAIL"
	test "$DEBFULLNAME"
	DEBCHANGE_OPTIONS="$@"
else
	# build an unsigned binary package
	DPKG_BUILPACKAGE_OPTS="-b -us -uc"
	BUILDER_ROLE='Changed-By'
	export DEBFULLNAME="$(/usr/bin/getent passwd "$USER" | \
				/usr/bin/cut --delimiter=: --fields=5 | /usr/bin/cut --delimiter=, --fields=1)"
	#export DEBEMAIL="${EMAIL:-$USER@$(/bin/hostname --long)}"
	export DEBEMAIL="info@gigamesh.eu"
	DEBCHANGE_OPTIONS="--fromdirname"
fi
export DEBMAIL="$DEBEMAIL"

export DEBCHANGE_VENDOR=${DEBCHANGE_VENDOR:-$(/usr/bin/dpkg-vendor --query vendor | /usr/bin/tr 'A-Z' 'a-z')}
test "$DEBCHANGE_VENDOR"
DISTRIBUTION=${DISTRIBUTION:-UNRELEASED}
test "$DISTRIBUTION"
cd "$(dirname "$0")/.."

# Clone fresh version from master branch including submodules

cd /tmp/
NAME=gigamesh$(uuidgen)
git clone ssh://git@fcgitlab.iwr.uni-heidelberg.de:2222/gigamesh/GigaMesh.git $NAME
wait
cd $NAME

DEBIAN_FILES="$PWD/packaging/debian"
DEBIAN_FILES_VENDOR="$PWD/packaging/$DEBCHANGE_VENDOR"
DIST_DIR="$PWD/dist"
#echo $DIST_DIR

#
# Unpack and create .orig and source dir.
#

SDIST_FILE="$DIST_DIR/$P_NAME-$P_VERSION.tar.gz"
ORIG_FILE="$DIST_DIR/${P_NAME}_${P_VERSION}.orig.tar.gz"

BUILD_DIR="$DIST_DIR/$P_NAME-$P_VERSION"
if test -d "$BUILD_DIR"; then
	echo "*** $BUILD_DIR already exists, is it a leftover from previous builds? Aborting."
	exit 1
fi

export TMPDIR="$(/bin/mktemp --directory --tmpdir debbuild-$P_NAME-$P_VERSION-$USER-XXXXXX)"

base=$(basename $PWD)
mkdir -p dist
# Remove all ply files, not needed
#echo "Removing PLY files"

echo "Removing all PLY files!"
find . -name "*.ply" -exec rm -rf {} \;
# Make libpsalm
cd external/libpsalmBoostless
mkdir build
cd build
cmake ..
wait
cmake --build . --config Release
cp libpsalm.a ..
cd ..
rm -rf build
cd ..
# END make libpsalm

# Make ALGLib
bash make_alglib.sh
wait
cd .. 
#END make ALGLib

# make gigamesh CLI (command line interface)
# gigamesh-featurevectors, gigamesh-clean, gigamesh-tolegacy and gigamesh-info
mkdir meshBuild
cd meshBuild
cmake ..
cmake --build . --config Release
#export CC=gcc-8 CXX=g++-8 LINK=g++-8 if neccessary
#make -j$(nproc) CC=gcc-8 CXX=g++-8 LINK=g++-8 package-files
wait
cd ..
# end make gigamesh CLI (command line interface)

#
#echo "TAR1"
/bin/tar -czf /tmp/tmparchive.orig.tar.gz $PWD &
#/bin/tar -czf /tmp/tmparchive.tar.gz $PWD &
wait
cp /tmp/tmparchive.orig.tar.gz $SDIST_FILE
wait
mv /tmp/tmparchive.orig.tar.gz $ORIG_FILE
#echo "TAR2"
#echo "DIST_DIR"
#echo $DIST_DIR
/bin/tar --extract --gunzip --file "$SDIST_FILE" --directory "$DIST_DIR"
#Rename extracted file to right format
#echo "BASE"
#echo $base
mv $DIST_DIR/tmp/$NAME/ $DIST_DIR/$P_NAME-$P_VERSION
test -d "$BUILD_DIR"
#echo "DEBUG"
#ls -la
#echo $PWD
cd dist/gigamesh-$P_VERSION

# If the orig file already exists for this version, check that no source
# changes occured.
if test -r "$ORIG_FILE"; then
	ORIG_SOURCES="$TMPDIR/$P_NAME-$P_VERSION"
	DIFF_OUTPUT="$TMPDIR/orig-diff-$P_VERSION"
	/bin/tar --extract --gunzip --file "$ORIG_FILE" --directory "$TMPDIR"
	echo "test before" 
    /usr/bin/diff --recursive --minimal --unified \
		"$ORIG_SOURCES" "$BUILD_DIR" >"$DIFF_OUTPUT" || true
	# either way, the sdist archive is no longer useful
	echo "test"
    /bin/rm --force "$SDIST_FILE"
	if test -s "$DIFF_OUTPUT"; then
		/bin/rm --force --recursive "$BUILD_DIR"
		echo '*** Current sbuild differs from existing .orig archive. Aborting.'
		cat "$DIFF_OUTPUT"
		exit 1
	fi
	unset ORIG_SOURCES DIFF_OUTPUT
else
	/bin/mv "$SDIST_FILE" "$ORIG_FILE"
fi

unset SDIST_FILE ORIG_FILE

#
# preparing to build the package
#

cd "$BUILD_DIR"

/bin/cp --archive --target-directory=. "$DEBIAN_FILES"

/bin/sed --in-place --file=- debian/control <<-CONTROL
	/^Maintainer:/ a\
	$BUILDER_ROLE: $DEBFULLNAME <$DEBEMAIL>
	CONTROL

/usr/bin/debchange \
	--vendor "$DEBCHANGE_VENDOR" \
	--distribution "$DISTRIBUTION" \
	--force-save-on-release \
	--auto-nmu \
	$DEBCHANGE_OPTIONS

if test "$DEBCHANGE_VENDOR" = debian; then
	# if this is the main (Debian) build, update the source changelog
	/bin/cp --archive --no-target-directory debian/changelog "$DEBIAN_FILES"/changelog
elif test -d "$DEBIAN_FILES_VENDOR"; then
	# else copy any additional files
	/bin/cp --archive --target-directory=debian/ "$DEBIAN_FILES_VENDOR"/* || true
fi

# install vendor-specific substvars files, if any
/usr/bin/find debian/ -type f -name "substvars.*.$DEBCHANGE_VENDOR" |\
while read subst_source; do
	subst_target="${subst_source%.$DEBCHANGE_VENDOR}"
	/bin/mv --force "$subst_source" "$subst_target"
done
# remove the templates, they are not relevant to the debian source package
/bin/rm --force debian/substvars.*.*

# apply custom substvars and clean-up debian/
cat debian/substvars.* | /bin/grep '^[-A-Za-z]*=' | /usr/bin/tr '=' ' ' |\
while read variable value; do
	/bin/sed --in-place --expression="s/\${solaar:$variable}/$value/" debian/control
done
/bin/rm --force debian/substvars.*

/usr/bin/debuild \
	--lintian --tgz-check \
	--preserve-envvar=DISPLAY \
	$DPKG_BUILPACKAGE_OPTS \
	--lintian-opts --profile "$DEBCHANGE_VENDOR"

/bin/rm --force --recursive "$BUILD_DIR"
cd ..
echo Succesfull created package, copying package file to $STARTDIR
cp gigamesh_*.deb $STARTDIR
pwd
# Remove folder in tmp
rm -Rf /tmp/debbuild-gigamesh*
#rm -Rf /tmp/$NAME