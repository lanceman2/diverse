#!/bin/sh

# an example of loading an and using dtk shared memory to move the head and
# other cave devices

dtk-caveDeviceSimulator &
export DGL_DSO_FILES=caveDTKInput:simulatorKeyboardControl:dosgToggleObjectsKeyboardControl:hyperGroup:dosgSimulator:dosgToggleObjects

dosg-fly sub.osg

kill %1
