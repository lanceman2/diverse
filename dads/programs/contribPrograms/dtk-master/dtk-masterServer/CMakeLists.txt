# CMakeLists.txt to build programs/contribPrograms/dtk-master/dtk-masterServer

INCLUDE_DIRECTORIES	(${DADS_SOURCE_DIR}/programs/contribPrograms/dtk-master/common
	${DADS_SOURCE_DIR}/lib/dSocket)
ADD_EXECUTABLE (dtk-masterServer dtk-masterServer.cpp)
TARGET_LINK_LIBRARIES (dtk-masterServer dSocket)
INSTALL (TARGETS dtk-masterServer RUNTIME DESTINATION bin PERMISSIONS
	OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ
	WORLD_EXECUTE)

