# CMakeLists.txt to build DSOs

ADD_SUBDIRECTORY( dgl )

IF( DGL_BUILD_WITH_OPENSCENEGRAPH )
	ADD_SUBDIRECTORY( dosg )
ENDIF( DGL_BUILD_WITH_OPENSCENEGRAPH )

