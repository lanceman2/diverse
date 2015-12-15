# CMakeLists.txt to build programs/dgl

IF( DGL_BUILD_WITH_FLTK )
	ADD_SUBDIRECTORY( dgl-navControl )
ENDIF( DGL_BUILD_WITH_FLTK )

IF( NOT WIN32 )
	ADD_SUBDIRECTORY( dgl-config )

	IF( CURSES_FOUND )
		ADD_SUBDIRECTORY( dgl-readState )
	ENDIF( CURSES_FOUND )

	ADD_SUBDIRECTORY( dgl-waitExec )
ENDIF( NOT WIN32 )

