# CMakeLists.txt to build programs/xwand

INCLUDE_DIRECTORIES( ${DTK_SOURCE_DIR}/include ${X11_Xlib_INCLUDE_PATH}
		${X11_Xtst_INCLUDE_PATH} )
LINK_DIRECTORIES( ${DTK_X11_LINK_DIRS} ${DTK_Xtst_LINK_DIRS} )
ADD_EXECUTABLE( dtk-xwand dtk-xwand.cpp )
TARGET_LINK_LIBRARIES( dtk-xwand dtk ${DTK_XTEST_LIBS} ${DTK_X11_LIBS} )
INSTALL( TARGETS dtk-xwand RUNTIME DESTINATION bin PERMISSIONS OWNER_READ
		OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ
		WORLD_EXECUTE )

