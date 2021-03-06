# CMakeLists.txt to build DIVERSE

PROJECT( DIVERSE )

IF( COMMAND CMAKE_POLICY )
	cmake_policy( SET CMP0000 OLD )
	cmake_policy( SET CMP0002 OLD )
	cmake_policy( SET CMP0003 OLD )
	cmake_policy( SET CMP0005 OLD )
	cmake_policy( SET CMP0006 OLD )
ENDIF( COMMAND CMAKE_POLICY )

SET(CMAKE_MODULE_PATH "${DIVERSE_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}" )

# This is set to true for debugging purposes - be sure to comment them out
# before relase
SET( CMAKE_VERBOSE_MAKEFILE ON )
IF( CMAKE_VERBOSE_MAKEFILE )
MESSAGE("\n*****************************************************************\nBefore release be sure to turn off CMAKE_VERBOSE_MAKEFILE\n*****************************************************************" )
ENDIF( CMAKE_VERBOSE_MAKEFILE )

SET( DIVERSE_MAJOR_VERSION 3 )
SET( DIVERSE_MINOR_VERSION 0 )
SET( DIVERSE_BUILD_VERSION 1 )
SET( DIVERSE_VERSION
"${DIVERSE_MAJOR_VERSION}.${DIVERSE_MINOR_VERSION}.${DIVERSE_BUILD_VERSION}" )

SET( DIVERSE_HOMEPAGE "http://diverse.sourceforge.net/" )
SET( DIVERSE_RELEASE_DATE "Fri Dec 12 18:00:00 EDT 2008" )
SET( DIVERSE_ARCH "${CMAKE_SYSTEM_NAME}" )
SET( DIVERSE_PROCESSOR "${CMAKE_SYSTEM_PROCESSOR}" )
IF( WIN32 )
	EXECUTE_PROCESS( COMMAND date /t OUTPUT_VARIABLE DIVERSE_COMPILE_DATE
			OUTPUT_STRIP_TRAILING_WHITESPACE )
	IF( NOT DIVERSE_COMPILE_DATE )
		SET( DIVERSE_COMPILE_DATE ${DIVERSE_RELEASE_DATE} )
	ENDIF( NOT DIVERSE_COMPILE_DATE )
	SET( DIVERSE_WHO_COMPILED $ENV{USERNAME} )
	SET( DIVERSE_COMPILE_HOST $ENV{USERDOMAIN} )

ELSE( WIN32 )
	EXECUTE_PROCESS( COMMAND date OUTPUT_VARIABLE DIVERSE_COMPILE_DATE
			OUTPUT_STRIP_TRAILING_WHITESPACE )
	EXECUTE_PROCESS( COMMAND whoami OUTPUT_VARIABLE DIVERSE_WHO_COMPILED
			OUTPUT_STRIP_TRAILING_WHITESPACE )
	EXECUTE_PROCESS( COMMAND hostname OUTPUT_VARIABLE DIVERSE_COMPILE_HOST
			OUTPUT_STRIP_TRAILING_WHITESPACE )
ENDIF( WIN32 )
SET( DIVERSE_WHO_COMPILED  )
SET( DIVERSE_COMPILE_HOST "`hostname`" )

SET( BUILD_SHARED_LIBS ON )
SET( CMAKE_SKIP_RPATH OFF )
SET( CMAKE_BUILD_WITH_INSTALL_RPATH ON )
SET( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" )
SET( CMAKE_MODULE_PATH ${DIVERSE_SOURCE_DIR}/CMakeModules )

# Package distribution
SET( CPACK_PACKAGE_DESCRIPTION_SUMMARY "DIVERSE ${DIVERSE_VERSION}" )
SET( CPACK_PACKAGE_NAME "diverse" )
SET( CPACK_PACKAGE_VENDOR "The DIVERSE Team" )
SET( CPACK_PACKAGE_VERSION_MAJOR "${DIVERSE_MAJOR_VERSION}" )
SET( CPACK_PACKAGE_VERSION_MINOR "${DIVERSE_MINOR_VERSION}" )
SET( CPACK_PACKAGE_VERSION_PATCH "${DIVERSE_BUILD_VERSION}" )
SET( CPACK_RESOURCE_FILE_LICENSE "${DIVERSE_SOURCE_DIR}/dtk/COPYING.GPL.txt" )
SET( CPACK_IGNORE_FILES "/CVS/;/\\\\.svn/;\\\\.swp$;\\\\.#;/#" )
SET( CPACK_SOURCE_IGNORE_FILES "/CVS/;/\\\\.svn/;\\\\.swp$;\\\\.#;/#" )
IF( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
	SET( CPACK_PACKAGE_INSTALL_DIRECTORY "/usr/local" )
	SET( CPACK_SOURCE_GENERATOR "TGZ;TBZ2;ZIP" )
	SET( CPACK_GENERATOR "PackageMaker" )
ELSEIF( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
	SET( CPACK_PACKAGE_INSTALL_DIRECTORY "DIVERSE" )
	SET( CPACK_NSIS_DISPLAY_NAME diverse-${DIVERSE_VERSION} )
	SET( CPACK_NSIS_MODIFY_PATH OFF )
	SET( CPACK_NSIS_URL_INFO_ABOUT "http://diverse.sourceforge.net/diverse/" )
	SET( CPACK_NSIS_INSTALLED_ICON_NAME "images\\\\nanopus.bmp" )
	SET( CPACK_SOURCE_GENERATOR "ZIP" )
	SET( CPACK_GENERATOR "NSIS" )
ELSEIF( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
	SET( CPACK_PACKAGE_INSTALL_DIRECTORY "/usr/local" )
	SET( CPACK_SOURCE_GENERATOR "TGZ;TBZ2;ZIP" )
	SET( CPACK_GENERATOR "TGZ;TBZ2" )
ELSE( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
	SET( CPACK_PACKAGE_INSTALL_DIRECTORY "/usr/local" )
	SET( CPACK_SOURCE_GENERATOR "TGZ;TBZ2;ZIP" )
	SET( CPACK_GENERATOR ${CPACK_SOURCE_GENERATOR} )
ENDIF( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
INCLUDE( CPack )
IF( UNIX AND NOT APPLE )
	INCLUDE(UseRPMTools )
	IF(RPMTools_FOUND )
		CONFIGURE_FILE( "${DIVERSE_SOURCE_DIR}/RPM/SPECS/diverse.spec.in"
			"${DIVERSE_BINARY_DIR}/RPM/SPECS/diverse.spec"
			IMMEDIATE @ONLY )
		RPMTools_ADD_RPM_TARGETS(diverse
			"${DIVERSE_BINARY_DIR}/RPM/SPECS/diverse.spec" )
	ENDIF(RPMTools_FOUND )
ENDIF( UNIX AND NOT APPLE )

OPTION( DIVERSE_BUILD_WITH_DGL 
"Build the DIVERSE interface to OpenGL( DGL)." ON )

IF( WIN32 )
	OPTION( DIVERSE_BUILD_WITH_DADS
		"Build the DIVERSE Adaptable Display System( DADS)." OFF )
ELSE( WIN32 )
	OPTION( DIVERSE_BUILD_WITH_DADS
		"Build the DIVERSE Adaptable Display System( DADS)." ON )
ENDIF( WIN32 )

SET( DIVERSE_ALL_IN_ONE_BUILD ON )

ADD_SUBDIRECTORY( dtk )

IF( DIVERSE_BUILD_WITH_DGL )
	ADD_SUBDIRECTORY( dgl )
ENDIF( DIVERSE_BUILD_WITH_DGL )

IF( DIVERSE_BUILD_WITH_DADS )
	ADD_SUBDIRECTORY( dads )
ENDIF( DIVERSE_BUILD_WITH_DADS )

CONFIGURE_FILE(
	"${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	IMMEDIATE @ONLY )

IF( WIN32 )
	SET( SYSTEM32_DIR $ENV{SystemRoot}/system32/ )
	STRING( REPLACE "\\" "/" SYSTEM32_DIR ${SYSTEM32_DIR} )
	INSTALL( PROGRAMS ${SYSTEM32_DIR}msvcp71.dll ${SYSTEM32_DIR}msvcr71.dll DESTINATION bin
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
		GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )
ENDIF( WIN32 )

ADD_CUSTOM_TARGET(uninstall
	"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" )

