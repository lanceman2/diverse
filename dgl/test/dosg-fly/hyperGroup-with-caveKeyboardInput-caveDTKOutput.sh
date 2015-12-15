#!/bin/sh

# an example of loading an object under each node in the dosg scenegraph,
# and using local records to control the objects

export DGL_DSO_FILES=caveDTKOutput:caveKeyboardInput:simulatorKeyboardControl:dosgToggleObjectsKeyboardControl:hyperGroup:dosgSimulator:dosgToggleObjects

dosg-fly sub.osg
