# CMakeLists.txt to build programs/dosg/dosg-transform

INCLUDE_DIRECTORIES( ${DGL_SOURCE_DIR}/include ${DTK_INCLUDE}
		${GLX_HEADER_PATH} ${OSG_INCLUDE_PATH} ${PRODUCER_INCLUDE_DIR}
		${OPENTHREADS_INCLUDE_DIR} )
ADD_EXECUTABLE( dosg-transform dosg-transform.cpp )
TARGET_LINK_LIBRARIES( dosg-transform dgl dosg )
SET_TARGET_PROPERTIES( dosg-transform PROPERTIES LINK_FLAGS
		"${DGL_OSG_LDFLAGS} ${DGL_LIB_FLAGS}" )
INSTALL( TARGETS dosg-transform RUNTIME DESTINATION bin PERMISSIONS OWNER_READ
		OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ
		WORLD_EXECUTE )

