# CMakeLists.txt to build programs/contribPrograms/displayswitchd

INCLUDE_DIRECTORIES (${DADS_SOURCE_DIR}/lib/dSocket)
ADD_EXECUTABLE (displayswitchd displayswitchd.cpp)
TARGET_LINK_LIBRARIES (displayswitchd dSocket)
INSTALL (TARGETS displayswitchd RUNTIME DESTINATION bin PERMISSIONS OWNER_READ
	OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ
	WORLD_EXECUTE)

