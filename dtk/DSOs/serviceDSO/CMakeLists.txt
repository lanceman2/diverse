# CMakeLists.txt to build serviceDSOs

ADD_SUBDIRECTORY( hello )
ADD_SUBDIRECTORY( flockofbirds )
ADD_SUBDIRECTORY( is900cf )

IF( NOT WIN32 )
	ADD_SUBDIRECTORY( flockCalibration )
	ADD_SUBDIRECTORY( wanda )
	ADD_SUBDIRECTORY( IWB )
	ADD_SUBDIRECTORY( pcwand )
	ADD_SUBDIRECTORY( udpEcho )
	ADD_SUBDIRECTORY( is900 )
	ADD_SUBDIRECTORY( is900-2handed )
	ADD_SUBDIRECTORY( is900-viswall )
	ADD_SUBDIRECTORY( is900-wall )
	ADD_SUBDIRECTORY( is900-wandonly )
	ADD_SUBDIRECTORY( is900-wall-wandonly )
ENDIF( NOT WIN32 )

IF( DTK_ARCH_UPPER MATCHES "LINUX" )
	ADD_SUBDIRECTORY( axisPad )
	ADD_SUBDIRECTORY( proPedals )
	ADD_SUBDIRECTORY( sideWinder2Joystick )
	ADD_SUBDIRECTORY( sideWinderJoystick )
	ADD_SUBDIRECTORY( visPad )
	ADD_SUBDIRECTORY( wingManJoystick )
	ADD_SUBDIRECTORY( yokeLE )
	ADD_SUBDIRECTORY( drivingForceGT )
ENDIF( DTK_ARCH_UPPER MATCHES "LINUX" )

