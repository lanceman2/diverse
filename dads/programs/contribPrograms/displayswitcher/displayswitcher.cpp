#include "displayswitchermainwindow.h"
#include <qapplication.h>

int main( int argc, char** argv )
{
	QApplication qapp( argc, argv );
	displaySwitcherMainWindow window;
	QFont font = window.font();
	font.setPointSize( 9 );
	window.setFont( font );
	qapp.setMainWidget( &window );
	if( window.initialize() )
		return -1;
	return qapp.exec();
}

