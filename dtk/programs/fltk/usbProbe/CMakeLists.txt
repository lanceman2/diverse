# CMakeLists.txt to build programs/fltk/usbProbe

INCLUDE_DIRECTORIES( ${DTK_SOURCE_DIR}/include ${DTK_FLTK_INC_DIRS} )
LINK_DIRECTORIES( ${DTK_FLTK_LINK_DIRS} )
ADD_EXECUTABLE( dtk-usbProbe dtk-usbProbe.cpp mainWindow.cpp widget.cpp )
TARGET_LINK_LIBRARIES( dtk-usbProbe dtk ${DTK_FLTK_LIBS} )
SET_TARGET_PROPERTIES( dtk-usbProbe PROPERTIES
		LINK_FLAGS "${DTK_FLTK_LINK_FLAGS}" )
INSTALL( TARGETS dtk-usbProbe RUNTIME DESTINATION bin PERMISSIONS OWNER_READ
	OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ
	WORLD_EXECUTE )

