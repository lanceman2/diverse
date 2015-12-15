# - Locate OpenAL
# This module defines
#	OPENAL_LIBRARY
#	OPENAL_FOUND, if false, do not try to link to OpenAL 
#	OPENAL_INCLUDE_DIR, where to find the headers
#	 
# $OPENALDIR is an environment variable that would
# correspond to the ./configure --prefix=$OPENALDIR
# used in building OpenAL.
#	 
# Created by Eric Wing. This was influenced by the FindSDL.cmake module.
# On OSX, this will prefer the Framework version (if found) over others.
# People will have to manually change the cache values of 
# OPENAL_LIBRARY to override this selection.
# Tiger will include OpenAL as part of the System.
# But for now, we have to look around.
# Other (Unix) systems should be able to utilize the non-framework paths.
#
# Modified by Patrick Shinpaugh of the DIVERSE Team

FIND_PATH( OPENAL_DIR include/AL/al.h
		$ENV{OPENAL_DIR}
		$ENV{OPENALDIR}
		/usr/local
		/usr
		/sw # Fink
		/opt/local # DarwinPorts
		/opt/csw # Blastwave
		/opt
)

FIND_PATH( OPENAL_DIR include/OpenAL/al.h
		$ENV{OPENAL_DIR}
		$ENV{OPENALDIR}
		/usr/local
		/usr
		/sw # Fink
		/opt/local # DarwinPorts
		/opt/csw # Blastwave
		/opt
)

FIND_PATH( OPENAL_DIR include/al.h
		$ENV{OPENAL_DIR}
		$ENV{OPENALDIR}
		/usr/local
		/usr
		/sw # Fink
		/opt/local # DarwinPorts
		/opt/csw # Blastwave
		/opt
)

FIND_PATH( OPENAL_DIR al.h
		~/Library/Frameworks/OpenAL.framework/Headers
		/Library/Frameworks/OpenAL.framework/Headers
		/System/Library/Frameworks/OpenAL.framework/Headers # Tiger
)

IF( OPENAL_DIR )
	FIND_PATH( OPENAL_INCLUDE_DIR al.h
			NO_DEFAULT_PATH
			PATHS
			${OPENAL_DIR}
			${OPENAL_DIR}/include
			PATH_SUFFIXES
			AL
			OpenAL
	)

	# I'm not sure if I should do a special casing for Apple. It is 
	# unlikely that other Unix systems will find the framework path.
	# But if they do ([Next|Open|GNU]Step?), 
	# do they want the -framework option also?
	IF( ${OPENAL_INCLUDE_DIR} MATCHES ".framework" )
		STRING( REGEX REPLACE "(.*)/.*\\.framework/.*" "\\1" OPENAL_FRAMEWORK_PATH_TMP ${OPENAL_INCLUDE_DIR} )
		IF( "${OPENAL_FRAMEWORK_PATH_TMP}" STREQUAL "/Library/Frameworks" OR
				"${OPENAL_FRAMEWORK_PATH_TMP}" STREQUAL "/System/Library/Frameworks" )
			# String is in default search path, don't need to use -F
			SET( OPENAL_LIBRARY "-framework OpenAL" CACHE STRING "OpenAL framework for OSX" )
		ELSE( "${OPENAL_FRAMEWORK_PATH_TMP}" STREQUAL "/Library/Frameworks" OR
				"${OPENAL_FRAMEWORK_PATH_TMP}" STREQUAL "/System/Library/Frameworks" )
			# String is not /Library/Frameworks, need to use -F
			SET( OPENAL_LIBRARY "-F${OPENAL_FRAMEWORK_PATH_TMP} -framework OpenAL" CACHE STRING "OpenAL framework for OSX" )
		ENDIF( "${OPENAL_FRAMEWORK_PATH_TMP}" STREQUAL "/Library/Frameworks" OR
				"${OPENAL_FRAMEWORK_PATH_TMP}" STREQUAL "/System/Library/Frameworks" )
		# Clear the temp variable so nobody can see it
		SET( OPENAL_FRAMEWORK_PATH_TMP "" CACHE INTERNAL "" )
	
	ELSE( ${OPENAL_INCLUDE_DIR} MATCHES ".framework" )
		FIND_LIBRARY( OPENAL_LIBRARY 
				NAMES openal al OpenAL32
				NO_DEFAULT_PATH
				PATHS
				${OPENAL_DIR}
				$ENV{OPENAL_DIR}
				$ENV{OPENALDIR}
				PATH_SUFFIXES
				lib
				libs
				lib32
				lib64
				libs/Win32
				libs/Win64
		)
	ENDIF( ${OPENAL_INCLUDE_DIR} MATCHES ".framework" )
	
	IF( NOT OPENAL_LIBRARY OR NOT OPENAL_INCLUDE_DIR )
		# Get the system search path as a list.
		IF( UNIX )
			STRING( REGEX MATCHALL "[^:]+" DIR_SEARCH1 "$ENV{PATH}" )
		ELSE( UNIX )
			STRING( REGEX REPLACE "\\\\" "/" DIR_SEARCH1 "$ENV{PATH}")
		ENDIF( UNIX )
		STRING( REGEX REPLACE "/;" ";" DIR_SEARCH ${DIR_SEARCH1} )
	
		FIND_PROGRAM( OPENAL_CONFIG_BIN NAMES openal-config PATHS /bin /usr/bin /usr/local/bin ${DIR_SEARCH} ${OPENAL_DIR}/bin )
		IF( OPENAL_CONFIG_BIN )
			EXECUTE_PROCESS (COMMAND ${OPENAL_CONFIG_BIN} --includedir OUTPUT_VARIABLE OPENAL_INCLUDE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE )
			EXECUTE_PROCESS (COMMAND ${OPENAL_CONFIG_BIN} --libdir OUTPUT_VARIABLE OPENAL_LIB_PATH OUTPUT_STRIP_TRAILING_WHITESPACE )
			IF( OPENAL_LIB_PATH )
				FIND_LIBRARY( OPENAL_LIBRARY
						NAMES openal al OpenAL32
						PATHS ${OPENAL_LIB_PATH} )
			ENDIF( OPENAL_LIB_PATH )
		ENDIF( OPENAL_CONFIG_BIN )
	ENDIF( NOT OPENAL_LIBRARY OR NOT OPENAL_INCLUDE_DIR )
ENDIF( OPENAL_DIR )

SET( OPENAL_FOUND "NO")
IF( OPENAL_LIBRARY AND OPENAL_INCLUDE_DIR )
	SET( OPENAL_FOUND "YES" )
ENDIF( OPENAL_LIBRARY AND OPENAL_INCLUDE_DIR )

