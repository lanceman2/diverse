# CMakeLists.txt to build augmentDSO/dtkFLTK

INCLUDE_DIRECTORIES( ${DTK_SOURCE_DIR}/include ${DTK_FLTK_INC_DIRS} )
LINK_DIRECTORIES( ${DTK_FLTK_LINK_DIRS} )
ADD_LIBRARY( fltkSimpleDisplay MODULE fltkSimpleDisplay.cpp )
TARGET_LINK_LIBRARIES( fltkSimpleDisplay dtkFLTK )
SET_TARGET_PROPERTIES( fltkSimpleDisplay PROPERTIES PREFIX ""
		LINK_FLAGS "${DTK_FLTK_LINK_FLAGS}" )
INSTALL( TARGETS fltkSimpleDisplay LIBRARY DESTINATION etc/dtk/augmentDSO
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE
		WORLD_READ WORLD_EXECUTE )

