# - Locate ALUT headers and library
# This module defines
#	ALUT_LIBRARY
#	ALUT_FOUND, if false, do not try to link to ALUT
#	ALUT_INCLUDE_DIR, where to find the headers
#	 
# $ALUTDIR is an environment variable that would
# correspond to the ./configure --prefix=$ALUTDIR
# used in building ALUT.
#	 
# Created by Patrick Shinpaugh of the DIVERSE Team. This was converted from the FindOpenAL.cmake module.
# On OSX, this will prefer the Framework version (if found) over others.
# People will have to manually change the cache values of 
# ALUT_LIBRARY to override this selection.
# Tiger will include OpenAL as part of the System.
# But for now, we have to look around.
# Other (Unix) systems should be able to utilize the non-framework paths.
#
# Modified by Patrick Shinpaugh of the DIVERSE Team

FIND_PATH( ALUT_DIR include/AL/alut.h
		$ENV{ALUT_DIR}
		$ENV{ALUTDIR}
		/usr/local
		/usr
		/sw # Fink
		/opt/local # DarwinPorts
		/opt/csw # Blastwave
		/opt
)

FIND_PATH( ALUT_DIR include/OpenAL/alut.h
		$ENV{ALUT_DIR}
		$ENV{ALUTDIR}
		/usr/local
		/usr
		/sw # Fink
		/opt/local # DarwinPorts
		/opt/csw # Blastwave
		/opt
)

FIND_PATH( ALUT_DIR include/alut.h
		$ENV{ALUT_DIR}
		$ENV{ALUTDIR}
		/usr/local
		/usr
		/sw # Fink
		/opt/local # DarwinPorts
		/opt/csw # Blastwave
		/opt
)

FIND_PATH( ALUT_DIR alut.h
		~/Library/Frameworks/OpenAL.framework/Headers
		/Library/Frameworks/OpenAL.framework/Headers
		/System/Library/Frameworks/OpenAL.framework/Headers # Tiger
)

IF( ALUT_DIR )
	FIND_PATH( ALUT_INCLUDE_DIR alut.h
			NO_DEFAULT_PATH
			PATHS
			${ALUT_DIR}
			${ALUT_DIR}/include
			PATH_SUFFIXES
			AL
			OpenAL
	)
	
	# I'm not sure if I should do a special casing for Apple. It is 
	# unlikely that other Unix systems will find the framework path.
	# But if they do ([Next|Open|GNU]Step?), 
	# do they want the -framework option also?
	IF( ${ALUT_INCLUDE_DIR} MATCHES ".framework" )
		STRING( REGEX REPLACE "(.*)/.*\\.framework/.*" "\\1" ALUT_FRAMEWORK_PATH_TMP ${ALUT_INCLUDE_DIR} )
		IF( "${ALUT_FRAMEWORK_PATH_TMP}" STREQUAL "/Library/Frameworks"
				OR "${ALUT_FRAMEWORK_PATH_TMP}" STREQUAL "/System/Library/Frameworks" )
			# String is in default search path, don't need to use -F
			SET( ALUT_LIBRARY "-framework OpenAL" CACHE STRING "OpenAL framework for OSX" )
		ELSE( "${ALUT_FRAMEWORK_PATH_TMP}" STREQUAL "/Library/Frameworks"
				OR "${ALUT_FRAMEWORK_PATH_TMP}" STREQUAL "/System/Library/Frameworks" )
			# String is not /Library/Frameworks, need to use -F
			SET( ALUT_LIBRARY "-F${ALUT_FRAMEWORK_PATH_TMP} -framework OpenAL" CACHE STRING "OpenAL framework for OSX" )
		ENDIF( "${ALUT_FRAMEWORK_PATH_TMP}" STREQUAL "/Library/Frameworks"
			OR "${ALUT_FRAMEWORK_PATH_TMP}" STREQUAL "/System/Library/Frameworks" )
		# Clear the temp variable so nobody can see it
		SET( ALUT_FRAMEWORK_PATH_TMP "" CACHE INTERNAL "" )
	
	ELSE( ${ALUT_INCLUDE_DIR} MATCHES ".framework" )
		FIND_LIBRARY(ALUT_LIBRARY 
				NAMES alut
				NO_DEFAULT_PATH
				PATHS
				${ALUT_DIR}
				$ENV{ALUT_DIR}
				$ENV{ALUTDIR}
				PATH_SUFFIXES
				lib
				libs
				lib32
				lib64
	)
	ENDIF( ${ALUT_INCLUDE_DIR} MATCHES ".framework" )
ENDIF( ALUT_DIR )

SET( ALUT_FOUND "NO" )
IF( ALUT_LIBRARY AND ALUT_INCLUDE_DIR )
	SET( ALUT_FOUND "YES" )
ENDIF( ALUT_LIBRARY AND ALUT_INCLUDE_DIR )

