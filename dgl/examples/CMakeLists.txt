# CMakeLists.txt to build examples

ADD_SUBDIRECTORY( CCallbackHelix )

IF( DGL_BUILD_WITH_COIN )
	ADD_SUBDIRECTORY( coinload )
ENDIF( DGL_BUILD_WITH_COIN )

IF( DGL_BUILD_WITH_OPENSCENEGRAPH )
	ADD_SUBDIRECTORY( dosgfly )
ENDIF( DGL_BUILD_WITH_OPENSCENEGRAPH )

IF( DGL_BUILD_WITH_VTK )
	ADD_SUBDIRECTORY( quad )
ENDIF( DGL_BUILD_WITH_VTK )

