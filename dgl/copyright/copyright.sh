#!/bin/sh

# usage $0 DISTDIR DGL_VERSION DGL_HOMEPAGE DGL_RELEASE_DATE
DISTDIR=$1
DGL_VERSION=$2
DGL_HOMEPAGE=$3
DGL_RELEASE_DATE=$4

# copyright directory
dirname=`dirname $0`

# current year
CURRENT_YEAR=`date +%Y`

# create new copyright with current specs
cd $dirname > /dev/null
sed -e "s|DGL_VERSION|$DGL_VERSION|" \
    -e "s|DGL_HOMEPAGE|$DGL_HOMEPAGE|" \
    -e "s|DGL_RELEASE_DATE|$DGL_RELEASE_DATE|" \
    -e "s|CURRENT_YEAR|$CURRENT_YEAR|" \
    < copyrightTemplate-txt > copyright-txt


# create C/cpp/h copyright file
delim='////////////////////////////////////////////////////////////////////////'
sed -e 1i\ "$delim" -e \$a\ "$delim" -e 's|^|//|' < copyright-txt > copyright-cpp

cd - > /dev/null


# add to cpp/h files
for f in `find $DISTDIR \( -name '*.cpp' -o -name '*.h' \) -print`; do
  mv $f $dirname/tempFile
  cat $dirname/copyright-cpp $dirname/tempFile > $f
done

# update data in splash screens
#for f in `find . -name '*.iv' -print`; do
#    mv $f $dirname/tempFile
#    sed -e "s|DGL_VERSION|$DGL_VERSION|" \
#	-e "s|DGL_HOMEPAGE|$DGL_HOMEPAGE|" \
#	-e "s|DGL_RELEASE_DATE|$DGL_RELEASE_DATE|" \
#	-e "s|CURRENT_YEAR|$CURRENT_YEAR|" \
#	< $dirname/tempFile > $f
#done




