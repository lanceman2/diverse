# CMakeLists.txt to build programs/contribPrograms/dtk-swapServer

INCLUDE_DIRECTORIES (${DTK_INCLUDE})
ADD_EXECUTABLE (dtk-swapServer lswap.cpp trackerfix.cpp)
TARGET_LINK_LIBRARIES (dtk-swapServer ${DTK_LIB})

ADD_EXECUTABLE (dtk-swapServer-stop tracker-stop.cpp)
TARGET_LINK_LIBRARIES (dtk-swapServer-stop ${DTK_LIB})

INSTALL (TARGETS dtk-swapServer dtk-swapServer-stop RUNTIME DESTINATION bin
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE
	WORLD_READ WORLD_EXECUTE)

