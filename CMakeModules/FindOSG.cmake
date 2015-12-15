# Locate gdal
# This module defines
# OSG_LIBRARY
# OSG_FOUND, if false, do not try to link to gdal 
# OSG_INCLUDE_DIR, where to find the headers
#
# $OSG_DIR is an environment variable that would
# correspond to the ./configure --prefix=$OSG_DIR
#
# Created by Robert Osfield. 

FIND_PATH( OSG_DIR include/osg/Node
		$ENV{OSG_DIR}
		$ENV{OSGDIR}
		$ENV{OSG_ROOT}
		$ENV{OPENTHREADS_DIR}
		$ENV{OSG_DIR}
		$ENV{OSGDIR}
		${OSG_DIR}
		${OPENTHREADS_DIR}
		${PRODUCER_DIR}
		/usr/local
		/usr
		/sw # Fink
		/opt/local # DarwinPorts
		/opt/csw # Blastwave
		/opt
		[HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]
		/usr/freeware
)

IF( NOT OSG_DIR )
	FIND_PATH( OSG_DIR osg/Node
			PATHS
			~/Library/Frameworks
			/Library/Frameworks
			${OSG_DIR}
			${OPENTHREADS_DIR}
			${PRODUCER_DIR}
	)
ENDIF( NOT OSG_DIR )

IF( OSG_DIR )

	FIND_PATH( OSG_INCLUDE_DIR osg/Node
			${OSG_DIR}/include
			$ENV{OSG_DIR}/include
			$ENV{OSG_DIR}
			$ENV{OSGDIR}/include
			$ENV{OSGDIR}
			$ENV{OSG_ROOT}/include
			~/Library/Frameworks
			/Library/Frameworks
			/usr/local/include
			/usr/include
			/sw/include # Fink
			/opt/local/include # DarwinPorts
			/opt/csw/include # Blastwave
			/opt/include
			[HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]/include
			/usr/freeware/include
	)

	MACRO( FIND_OSG_LIBRARY MYLIBRARY MYLIBRARYNAME )

		FIND_LIBRARY( ${MYLIBRARY}
				NAMES ${MYLIBRARYNAME}
				PATHS
				${OSG_DIR}/lib
				${OSG_DIR}/lib64
				$ENV{OSG_DIR}/lib
				$ENV{OSG_DIR}/lib64
				$ENV{OSG_DIR}
				$ENV{OSGDIR}/lib
				$ENV{OSGDIR}/lib64
				$ENV{OSGDIR}
				$ENV{OSG_ROOT}/lib
				$ENV{OSG_ROOT}/lib64
				~/Library/Frameworks
				/Library/Frameworks
				/usr/local/lib
				/usr/local/lib64
				/usr/lib
				/usr/lib64
				/sw/lib
				/opt/local/lib
				/opt/csw/lib
				/opt/lib
				[HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]/lib
				/usr/freeware/lib64
		)

	ENDMACRO( FIND_OSG_LIBRARY LIBRARY LIBRARYNAME )

	FIND_OSG_LIBRARY( OSG_LIBRARY osg )
	FIND_OSG_LIBRARY( OSGUTIL_LIBRARY osgUtil )
	FIND_OSG_LIBRARY( OSGDB_LIBRARY osgDB )
	FIND_OSG_LIBRARY( OSGTEXT_LIBRARY osgText )
	FIND_OSG_LIBRARY( OSGTERRAIN_LIBRARY osgTerrain )
	FIND_OSG_LIBRARY( OSGFX_LIBRARY osgFX )
	FIND_OSG_LIBRARY( OSGVIEWER_LIBRARY osgViewer )

	FIND_OSG_LIBRARY( OSG_LIBRARY_DEBUG osgd )
	FIND_OSG_LIBRARY( OSGUTIL_LIBRARY_DEBUG osgUtild )
	FIND_OSG_LIBRARY( OSGDB_LIBRARY_DEBUG osgDBd )
	FIND_OSG_LIBRARY( OSGTEXT_LIBRARY_DEBUG osgTextd )
	FIND_OSG_LIBRARY( OSGTERRAIN_LIBRARY_DEBUG osgTerraind )
	FIND_OSG_LIBRARY( OSGFX_LIBRARY_DEBUG osgFXd )
	FIND_OSG_LIBRARY( OSGVIEWER_LIBRARY_DEBUG osgViewerd )

	IF( OSG_LIBRARY AND NOT OSG_LIBRARY_DEBUG )
		SET( OSG_LIBRARY_DEBUG ${OSG_LIBRARY} CACHE FILEPATH "Debug version of osg Library (use regular version if not available)" FORCE )
	ENDIF( OSG_LIBRARY AND NOT OSG_LIBRARY_DEBUG )
	IF( OSGUTIL_LIBRARY AND NOT OSGUTIL_LIBRARY_DEBUG )
		SET( OSGUTIL_LIBRARY_DEBUG ${OSGUTIL_LIBRARY} CACHE FILEPATH "Debug version of osgUtil Library (use regular version if not available)" FORCE )
	ENDIF( OSGUTIL_LIBRARY AND NOT OSGUTIL_LIBRARY_DEBUG )
	IF( OSGDB_LIBRARY AND NOT OSGDB_LIBRARY_DEBUG )
		SET( OSGDB_LIBRARY_DEBUG ${OSGDB_LIBRARY} CACHE FILEPATH "Debug version of osgDB Library (use regular version if not available)" FORCE )
	ENDIF( OSGDB_LIBRARY AND NOT OSGDB_LIBRARY_DEBUG )
	IF( OSGTEXT_LIBRARY AND NOT OSGTEXT_LIBRARY_DEBUG )
		SET( OSGTEXT_LIBRARY_DEBUG ${OSGTEXT_LIBRARY} CACHE FILEPATH "Debug version of osgText Library (use regular version if not available)" FORCE )
	ENDIF( OSGTEXT_LIBRARY AND NOT OSGTEXT_LIBRARY_DEBUG )
	IF( OSGTERRAIN_LIBRARY AND NOT OSGTERRAIN_LIBRARY_DEBUG )
		SET( OSGTERRAIN_LIBRARY_DEBUG ${OSGTERRAIN_LIBRARY} CACHE FILEPATH "Debug version of osgTerrain Library (use regular version if not available)" FORCE )
	ENDIF( OSGTERRAIN_LIBRARY AND NOT OSGTERRAIN_LIBRARY_DEBUG )
	IF( OSGFX_LIBRARY AND NOT OSGFX_LIBRARY_DEBUG )
		SET( OSGFX_LIBRARY_DEBUG ${OSGFX_LIBRARY} CACHE FILEPATH "Debug version of osgFX Library (use regular version if not available)" FORCE )
	ENDIF( OSGFX_LIBRARY AND NOT OSGFX_LIBRARY_DEBUG )
	IF( OSGVIEWER_LIBRARY AND NOT OSGVIEWER_LIBRARY_DEBUG )
		SET( OSGVIEWER_LIBRARY_DEBUG ${OSGVIEWER_LIBRARY} CACHE FILEPATH "Debug version of osgViewer Library (use regular version if not available)" FORCE )
	ENDIF( OSGVIEWER_LIBRARY AND NOT OSGVIEWER_LIBRARY_DEBUG )
ENDIF( OSG_DIR )

SET( OSG_FOUND "NO" )
IF( OSG_LIBRARY AND OSG_INCLUDE_DIR )
	SET( OSG_FOUND "YES" )
ENDIF( OSG_LIBRARY AND OSG_INCLUDE_DIR )

