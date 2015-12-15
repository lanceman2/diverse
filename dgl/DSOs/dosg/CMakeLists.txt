# CMakeLists.txt to build DSOs/dosg

INCLUDE_DIRECTORIES( ${DGL_SOURCE_DIR}/include ${DTK_INCLUDE}
		${GLX_HEADER_PATH} ${PRODUCER_INCLUDE_DIR}
		${OPENTHREADS_INCLUDE_DIR} )
ADD_LIBRARY( dosgSimulator MODULE dosgSimulator.cpp )
TARGET_LINK_LIBRARIES( dosgSimulator dosg )
ADD_LIBRARY( dosgToggleObjectsKeyboardControl MODULE
		dosgToggleObjectsKeyboardControl.cpp )
TARGET_LINK_LIBRARIES( dosgToggleObjectsKeyboardControl dosg )
ADD_LIBRARY( dosgToggleObjects MODULE dosgToggleObjects.cpp )
TARGET_LINK_LIBRARIES( dosgToggleObjects dosg )
ADD_LIBRARY( dosgHeadLight MODULE dosgHeadLight.cpp )
TARGET_LINK_LIBRARIES( dosgHeadLight dosg )
ADD_LIBRARY( dosgWandLight MODULE dosgWandLight.cpp )
TARGET_LINK_LIBRARIES( dosgWandLight dosg )
ADD_LIBRARY( dosgCenterPivotNavMod MODULE dosgCenterPivotNavMod.cpp )
TARGET_LINK_LIBRARIES( dosgCenterPivotNavMod dosg )
ADD_LIBRARY( dosgWorldPivotNavMod MODULE dosgWorldPivotNavMod.cpp )
TARGET_LINK_LIBRARIES( dosgWorldPivotNavMod dosg )
ADD_LIBRARY( dosgTestCoordXforms MODULE dosgTestCoordXforms.cpp )
TARGET_LINK_LIBRARIES( dosgTestCoordXforms dosg )
ADD_LIBRARY( dosgWandMouse MODULE ../dgl/wandMouse.cpp
		../dgl/fakeMouseButton.cpp )
SET_TARGET_PROPERTIES( dosgWandMouse PROPERTIES COMPILE_FLAGS -DCURSOR )
IF( UNIX )
	SET( X11_LIBS "-L${X11_LIB_PATH} -lXtst -lXext -lX11" )
ENDIF( UNIX )
TARGET_LINK_LIBRARIES( dosgWandMouse dosg ${X11_LIBS} )
ADD_LIBRARY( dosgScreenFrame MODULE dosgScreenFrame.cpp )
TARGET_LINK_LIBRARIES( dosgScreenFrame dosg )
ADD_LIBRARY( dosgSimulatorGroup MODULE dosgSimulatorGroup.cpp )
TARGET_LINK_LIBRARIES( dosgSimulatorGroup dosg )
ADD_LIBRARY( dosgShmTimeWarp MODULE dosgShmTimeWarp.cpp )
TARGET_LINK_LIBRARIES( dosgShmTimeWarp dosg )
ADD_LIBRARY( dosgExitAfterNFrames MODULE dosgExitAfterNFrames.cpp )
TARGET_LINK_LIBRARIES( dosgExitAfterNFrames dosg )
ADD_LIBRARY( dosgCullSceneForNFrames MODULE dosgCullSceneForNFrames.cpp )
TARGET_LINK_LIBRARIES( dosgCullSceneForNFrames dosg )
ADD_LIBRARY( dosgThreadingTest MODULE dosgThreadingTest.cpp )
TARGET_LINK_LIBRARIES( dosgThreadingTest dosg )
ADD_LIBRARY( dosgSetClearColor MODULE dosgSetClearColor.cpp )
TARGET_LINK_LIBRARIES( dosgSetClearColor dosg )
ADD_LIBRARY( dosgWriteState MODULE dosgWriteState.cpp )
TARGET_LINK_LIBRARIES( dosgWriteState dosg )
ADD_LIBRARY( dosgDesktopWandHPR MODULE dosgDesktopWandHPR.cpp )
TARGET_LINK_LIBRARIES( dosgDesktopWandHPR dosg )

SET_TARGET_PROPERTIES( dosgSimulator dosgToggleObjectsKeyboardControl
		dosgToggleObjects dosgHeadLight dosgWandLight dosgCenterPivotNavMod
		dosgWorldPivotNavMod dosgTestCoordXforms dosgWandMouse dosgScreenFrame
		dosgSimulatorGroup dosgShmTimeWarp dosgExitAfterNFrames
		dosgCullSceneForNFrames dosgThreadingTest dosgSetClearColor
		dosgWriteState dosgDesktopWandHPR PROPERTIES LINK_FLAGS
		"${DGL_LIB_FLAGS} ${DGL_OSG_LDFLAGS}" PREFIX "" )

INSTALL( TARGETS dosgSimulator dosgToggleObjectsKeyboardControl
		dosgToggleObjects dosgHeadLight dosgWandLight dosgCenterPivotNavMod
		dosgWorldPivotNavMod dosgTestCoordXforms dosgWandMouse dosgScreenFrame
		dosgSimulatorGroup dosgShmTimeWarp dosgExitAfterNFrames
		dosgCullSceneForNFrames dosgThreadingTest dosgSetClearColor
		dosgWriteState dosgDesktopWandHPR LIBRARY DESTINATION etc/dgl/DSO
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
		GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )

