#! /bin/sh

unzip flexomat.zip
convert -resize 512x64! CINTA.GIF cinta.png
osgconv FLEXOMET.3DS foo.osg #ignore errors
sed -e 's/CINTA.GIF/cinta.png/' < foo.osg > flexomat.osg
dosg-transform --center --bound flexomat.osg flexomat-normalized.osg
dosg-transform --translate 0 2 0 flexomat-normalized.osg diverse.osg tape.ive
mv tape.ive ..

