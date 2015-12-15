#!/bin/sh

# an example of loading an and using dtk shared memory to move the head and
# other cave devices, plus uses wandMouse to move X cursor to where wand
# intesects a screen viewport

dtk-writeShm dgl/wandMouse 1 1 "%c" 2

# if you can do this next two command remotely you'll be happier
dtk-caveDeviceSimulator &

export DGL_DSO_FILES=caveDTKInput:simulatorKeyboardControl:dosgToggleObjectsKeyboardControl:hyperGroup:dosgSimulator:dosgToggleObjects:dosgWandMouse

dosg-fly NULL

kill %1
