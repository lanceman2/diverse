# CMakeLists.txt to build programs/contribPrograms/dtk-master/dtk-masterControl

INCLUDE_DIRECTORIES	(${DADS_SOURCE_DIR}/programs/contribPrograms/dtk-master/common
	${DADS_SOURCE_DIR}/lib/dSocket)
ADD_EXECUTABLE (dtk-masterControl dtk-masterControl.cpp)
TARGET_LINK_LIBRARIES (dtk-masterControl dSocket)
INSTALL (TARGETS dtk-masterControl RUNTIME DESTINATION bin PERMISSIONS
	OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ
	WORLD_EXECUTE)

