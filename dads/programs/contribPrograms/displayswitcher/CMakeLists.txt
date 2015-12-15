# CMakeLists.txt to build programs/contribPrograms/displayswitcher

INCLUDE_DIRECTORIES (${DADS_SOURCE_DIR}/lib/dSocket ${DTK_INCLUDE}
	${QT_INCLUDE_DIR})
ADD_CUSTOM_COMMAND (OUTPUT
	${DADS_BINARY_DIR}/programs/contribPrograms/displayswitcher/moc_displayswitchermainwindow.cpp
	COMMAND ${QT_MOC_EXECUTABLE} -o
	${DADS_BINARY_DIR}/programs/contribPrograms/displayswitcher/moc_displayswitchermainwindow.cpp
	${DADS_SOURCE_DIR}/programs/contribPrograms/displayswitcher/displayswitchermainwindow.h)
ADD_EXECUTABLE (displayswitcher displayswitchermainwindow.cpp
	displayswitcher.cpp
	${DADS_BINARY_DIR}/programs/contribPrograms/displayswitcher/moc_displayswitchermainwindow.cpp)
TARGET_LINK_LIBRARIES (displayswitcher dSocket ${QT_QT_LIBRARY})
INSTALL (TARGETS displayswitcher RUNTIME DESTINATION bin PERMISSIONS
	OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ
	WORLD_EXECUTE)

