# CMakeLists.txt to build DSOs/dgl

INCLUDE_DIRECTORIES( ${DGL_SOURCE_DIR}/include ${DTK_INCLUDE}
		${GLX_HEADER_PATH} ${PRODUCER_INCLUDE_DIR} ${OPENTHREADS_INCLUDE_DIR} )
ADD_LIBRARY( desktopGroup MODULE desktopGroup.cpp )
TARGET_LINK_LIBRARIES( desktopGroup dgl )
ADD_LIBRARY( desktopCaveEmulateGroup MODULE desktopCaveEmulateGroup.cpp )
TARGET_LINK_LIBRARIES( desktopCaveEmulateGroup dgl )
ADD_LIBRARY( desktopDisplay MODULE desktopDisplay.cpp )
TARGET_LINK_LIBRARIES( desktopDisplay dgl )
ADD_LIBRARY( desktopOrtho MODULE desktopOrtho.cpp )
TARGET_LINK_LIBRARIES( desktopOrtho dgl )
ADD_LIBRARY( desktopScreen MODULE desktopScreen.cpp )
TARGET_LINK_LIBRARIES( desktopScreen dgl )
ADD_LIBRARY( desktopScreenGroup MODULE desktopScreenGroup.cpp )
TARGET_LINK_LIBRARIES( desktopScreenGroup dgl )
ADD_LIBRARY( desktopHyperSim MODULE desktopHyperSim.cpp )
TARGET_LINK_LIBRARIES( desktopHyperSim dgl )
ADD_LIBRARY( vtCaveDisplayFront MODULE vtCaveDisplayFront.cpp )
TARGET_LINK_LIBRARIES( vtCaveDisplayFront dgl )
ADD_LIBRARY( vtCaveDisplayFloor MODULE vtCaveDisplayFloor.cpp )
TARGET_LINK_LIBRARIES( vtCaveDisplayFloor dgl )
ADD_LIBRARY( vtCaveDisplayLeft MODULE vtCaveDisplayLeft.cpp )
TARGET_LINK_LIBRARIES( vtCaveDisplayLeft dgl )
ADD_LIBRARY( vtCaveDisplayRight MODULE vtCaveDisplayRight.cpp )
TARGET_LINK_LIBRARIES( vtCaveDisplayRight dgl )
ADD_LIBRARY( vtCaveClusterGroup MODULE vtCaveClusterGroup.cpp )
TARGET_LINK_LIBRARIES( vtCaveClusterGroup dgl )
ADD_LIBRARY( vtCaveMonoDisplayFront MODULE vtCaveMonoDisplayFront.cpp )
TARGET_LINK_LIBRARIES( vtCaveMonoDisplayFront dgl )
ADD_LIBRARY( vtCaveMonoDisplayFloor MODULE vtCaveMonoDisplayFloor.cpp )
TARGET_LINK_LIBRARIES( vtCaveMonoDisplayFloor dgl )
ADD_LIBRARY( vtCaveMonoDisplayLeft MODULE vtCaveMonoDisplayLeft.cpp )
TARGET_LINK_LIBRARIES( vtCaveMonoDisplayLeft dgl )
ADD_LIBRARY( vtCaveMonoDisplayRight MODULE vtCaveMonoDisplayRight.cpp )
TARGET_LINK_LIBRARIES( vtCaveMonoDisplayRight dgl )
ADD_LIBRARY( vtCaveMonoClusterGroup MODULE vtCaveMonoClusterGroup.cpp )
TARGET_LINK_LIBRARIES( vtCaveMonoClusterGroup dgl )
ADD_LIBRARY( vtCaveClusterClientGroup MODULE vtCaveClusterClientGroup.cpp )
TARGET_LINK_LIBRARIES( vtCaveClusterClientGroup dgl )
ADD_LIBRARY( vtCaveGroup MODULE vtCaveGroup.cpp )
TARGET_LINK_LIBRARIES( vtCaveGroup dgl )
ADD_LIBRARY( vtWallDisplay1280x1024 MODULE vtWallDisplay1280x1024.cpp )
TARGET_LINK_LIBRARIES( vtWallDisplay1280x1024 dgl )
ADD_LIBRARY( vtWallMonoDisplay1280x1024 MODULE vtWallMonoDisplay1280x1024.cpp )
TARGET_LINK_LIBRARIES( vtWallMonoDisplay1280x1024 dgl )
ADD_LIBRARY( vtWallDisplay1400x1050 MODULE vtWallDisplay1400x1050.cpp )
TARGET_LINK_LIBRARIES( vtWallDisplay1400x1050 dgl )
ADD_LIBRARY( vtWallMonoDisplay1400x1050 MODULE vtWallMonoDisplay1400x1050.cpp )
TARGET_LINK_LIBRARIES( vtWallMonoDisplay1400x1050 dgl )
ADD_LIBRARY( gnomonHeadsUp MODULE gnomonHeadsUp.cpp )
TARGET_LINK_LIBRARIES( gnomonHeadsUp dgl )
ADD_LIBRARY( passiveScreens MODULE passiveScreens.cpp )
TARGET_LINK_LIBRARIES( passiveScreens dgl )
ADD_LIBRARY( passiveEyeSetter MODULE passiveEyeSetter.cpp )
TARGET_LINK_LIBRARIES( passiveEyeSetter dgl )
ADD_LIBRARY( passiveStereoGroup MODULE passiveStereoGroup.cpp )
TARGET_LINK_LIBRARIES( passiveStereoGroup dgl )
ADD_LIBRARY( DGLkeyboardNavControl MODULE DGLkeyboardNavControl.cpp )
TARGET_LINK_LIBRARIES( DGLkeyboardNavControl dgl )
ADD_LIBRARY( envvarDisplay MODULE envvarDisplay.cpp )
TARGET_LINK_LIBRARIES( envvarDisplay dgl )
ADD_LIBRARY( nistRaveHyperDisplay MODULE nistRaveHyperDisplay.cpp )
TARGET_LINK_LIBRARIES( nistRaveHyperDisplay dgl )
ADD_LIBRARY( hyperDisplay MODULE hyperDisplay.cpp )
TARGET_LINK_LIBRARIES( hyperDisplay dgl )
ADD_LIBRARY( setHeadTracking MODULE setHeadTracking.cpp )
TARGET_LINK_LIBRARIES( setHeadTracking dgl )
ADD_LIBRARY( caveKeyboardInput MODULE caveKeyboardInput.cpp )
TARGET_LINK_LIBRARIES( caveKeyboardInput dgl )
ADD_LIBRARY( simulatorKeyboardControl MODULE simulatorKeyboardControl.cpp )
TARGET_LINK_LIBRARIES( simulatorKeyboardControl dgl )
ADD_LIBRARY( multiThreaded MODULE multiThreaded.cpp )
TARGET_LINK_LIBRARIES( multiThreaded dgl )
ADD_LIBRARY( singleThreaded MODULE singleThreaded.cpp )
TARGET_LINK_LIBRARIES( singleThreaded dgl )
ADD_LIBRARY( slb MODULE slb.cpp )
TARGET_LINK_LIBRARIES( slb dgl )
ADD_LIBRARY( noHT MODULE noHT.cpp )
TARGET_LINK_LIBRARIES( noHT dgl )
ADD_LIBRARY( wandMouse MODULE wandMouse.cpp fakeMouseButton.cpp )
IF( UNIX )
	SET( X11_LIBS "-L${X11_LIB_PATH} -lXtst -lXext -lX11" )
ENDIF( UNIX )
TARGET_LINK_LIBRARIES( wandMouse dgl "${X11_LIBS}" )
ADD_LIBRARY( wandMouseGestureControl MODULE wandMouseGestureControl.cpp )
TARGET_LINK_LIBRARIES( wandMouseGestureControl dgl )
ADD_LIBRARY( setClearColor MODULE setClearColor.cpp )
TARGET_LINK_LIBRARIES( setClearColor dgl )
ADD_LIBRARY( shmNavControl MODULE shmNavControl.cpp )
TARGET_LINK_LIBRARIES( shmNavControl dgl )
ADD_LIBRARY( writeState MODULE writeState.cpp )
TARGET_LINK_LIBRARIES( writeState dgl )
ADD_LIBRARY( NULLDisplay MODULE NULLDisplay.cpp )
TARGET_LINK_LIBRARIES( NULLDisplay dgl )
ADD_LIBRARY( visWallGroup MODULE visWallGroup.cpp )
TARGET_LINK_LIBRARIES( visWallGroup dgl )
ADD_LIBRARY( visWallEyeSetter MODULE visWallEyeSetter.cpp )
TARGET_LINK_LIBRARIES( visWallEyeSetter dgl )
ADD_LIBRARY( visWallScreens MODULE visWallScreens.cpp )
TARGET_LINK_LIBRARIES( visWallScreens dgl )
ADD_LIBRARY( monoDisplayMod MODULE monoDisplayMod.cpp )
TARGET_LINK_LIBRARIES( monoDisplayMod dgl )
ADD_LIBRARY( vtWallGroup MODULE vtWallGroup.cpp )
TARGET_LINK_LIBRARIES( vtWallGroup dgl )
ADD_LIBRARY( vtWallMonoGroup MODULE vtWallMonoGroup.cpp )
TARGET_LINK_LIBRARIES( vtWallMonoGroup dgl )

IF( DGL_BUILD_WITH_DGLNVSWAPBARRIER )
	INCLUDE_DIRECTORIES( ${DGL_SOURCE_DIR}/include ${GLX_HEADER_PATH}
			${X11_X11_INCLUDE_PATH} ${DTK_INCLUDE} )
	ADD_LIBRARY( dglNVSwapBarrier MODULE dglNVSwapBarrier.cpp )
	TARGET_LINK_LIBRARIES( dglNVSwapBarrier dgl GL GLU X11 )
	SET_TARGET_PROPERTIES( dglNVSwapBarrier PROPERTIES LINK_FLAGS
			-L${X11_LIB_PATH}
			PREFIX "" )
	INSTALL( TARGETS dglNVSwapBarrier LIBRARY DESTINATION etc/dgl/DSO
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
			GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )
ENDIF( DGL_BUILD_WITH_DGLNVSWAPBARRIER )

SET_TARGET_PROPERTIES( desktopGroup desktopCaveEmulateGroup desktopDisplay
		desktopOrtho desktopScreen desktopScreenGroup desktopHyperSim
		vtCaveDisplayFront vtCaveDisplayFloor vtCaveDisplayLeft
		vtCaveDisplayRight vtCaveClusterGroup vtCaveMonoDisplayFront
		vtCaveMonoDisplayFloor vtCaveMonoDisplayLeft vtCaveMonoDisplayRight
		vtCaveMonoClusterGroup vtCaveClusterClientGroup vtCaveGroup
		gnomonHeadsUp passiveScreens passiveEyeSetter passiveStereoGroup
		DGLkeyboardNavControl envvarDisplay nistRaveHyperDisplay hyperDisplay
		setHeadTracking caveKeyboardInput simulatorKeyboardControl
		multiThreaded singleThreaded slb noHT wandMouse
		wandMouseGestureControl setClearColor shmNavControl writeState
		NULLDisplay visWallGroup visWallEyeSetter visWallScreens
		monoDisplayMod vtWallDisplay1280x1024 vtWallMonoDisplay1280x1024
		vtWallDisplay1400x1050 vtWallMonoDisplay1400x1050 vtWallGroup
		vtWallMonoGroup PROPERTIES
		LINK_FLAGS ${DGL_LIB_FLAGS} PREFIX "" )
INSTALL( TARGETS desktopGroup desktopCaveEmulateGroup desktopDisplay
		desktopOrtho desktopScreen desktopScreenGroup desktopHyperSim
		vtCaveDisplayFront vtCaveDisplayFloor vtCaveDisplayLeft
		vtCaveDisplayRight vtCaveClusterGroup vtCaveMonoDisplayFront
		vtCaveMonoDisplayFloor vtCaveMonoDisplayLeft vtCaveMonoDisplayRight
		vtCaveMonoClusterGroup vtCaveClusterClientGroup vtCaveGroup
		gnomonHeadsUp passiveScreens passiveEyeSetter passiveStereoGroup
		DGLkeyboardNavControl envvarDisplay nistRaveHyperDisplay hyperDisplay
		setHeadTracking caveKeyboardInput simulatorKeyboardControl
		multiThreaded singleThreaded slb noHT wandMouse
		wandMouseGestureControl setClearColor shmNavControl writeState
		NULLDisplay visWallGroup visWallEyeSetter visWallScreens
		monoDisplayMod vtWallDisplay1280x1024 vtWallMonoDisplay1280x1024
		vtWallDisplay1400x1050 vtWallMonoDisplay1400x1050 vtWallGroup
		vtWallMonoGroup LIBRARY DESTINATION
		etc/dgl/DSO PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
		GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )

