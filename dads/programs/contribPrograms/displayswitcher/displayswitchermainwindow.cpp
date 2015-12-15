#include <stdlib.h>
#include "displayswitchermainwindow.h"
#include <qlayout.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qhbuttongroup.h>

displaySwitcherMainWindow::displaySwitcherMainWindow( QWidget* parent )
:QMainWindow( parent )
,m_instantupdates( false )
{
	QWidget* mainwidget = new QWidget( this );
	QFont font = mainwidget->font();
	font.setPointSize( 9 );
	mainwidget->setFont( font );
	setCentralWidget( mainwidget );
	QVBoxLayout* mainlayout = new QVBoxLayout( mainwidget, 5, 5, "mainlayout" );
	QHBoxLayout* hlayout = new QHBoxLayout( mainlayout );
	m_cavebutton = new QPushButton( "CAVE", mainwidget );
	connect( m_cavebutton, SIGNAL( clicked() ), this, SLOT( presetCAVE() ) );
	hlayout->addWidget( m_cavebutton );
	m_dadsbutton = new QPushButton( "DADS", mainwidget );
	connect( m_dadsbutton, SIGNAL( clicked() ), this, SLOT( presetDADS() ) );
	hlayout->addWidget( m_dadsbutton );
//	m_loadbutton = new QPushButton( "Load", mainwidget );
//	hlayout->addWidget( m_loadbutton );
	m_advancedbutton = new QPushButton( "Advanced", mainwidget );
	m_advancedbutton->setToggleButton( true );
	hlayout->addWidget( m_advancedbutton );
	m_advancedwidget = new QWidget( mainwidget );
	mainlayout->addWidget( m_advancedwidget );
	m_advancedwidget->hide();
	connect( m_advancedbutton, SIGNAL( toggled( bool ) ),
			m_advancedwidget, SLOT( setShown( bool ) ) );
	connect( m_advancedbutton, SIGNAL( toggled( bool ) ),
			this, SLOT( showAdvanced( bool ) ) );
	mainlayout = new QVBoxLayout( m_advancedwidget, 5, 5, "advancedlayout" );
	hlayout = new QHBoxLayout( mainlayout );
	QCheckBox* checkbox = new QCheckBox( "Instant Updates", m_advancedwidget );
	connect( checkbox, SIGNAL( toggled( bool ) ),
			this, SLOT( setInstantUpdates( bool ) ) );
	hlayout->addWidget( checkbox );
//	QPushButton* button = new QPushButton( "Save", m_advancedwidget );
//	hlayout->addWidget( button );
	hlayout = new QHBoxLayout( mainlayout );
	QHButtonGroup* buttongroup = new QHButtonGroup( "CAVE", m_advancedwidget );
	connect( buttongroup, SIGNAL( clicked( int ) ),
			this, SLOT( setCAVEInputs( int ) ) );
	hlayout->addWidget( buttongroup );
	QPushButton* button = new QPushButton( "1 - Front", buttongroup );
	button->setToggleButton( true );
	buttongroup->insert( button );
	m_inputs.push_back( button );
	button = new QPushButton( "2 - Bottom", buttongroup );
	button->setToggleButton( true );
	buttongroup->insert( button );
	m_inputs.push_back( button );
	button = new QPushButton( "3 - Left", buttongroup );
	button->setToggleButton( true );
	buttongroup->insert( button );
	m_inputs.push_back( button );
	button = new QPushButton( "4 - Right", buttongroup );
	button->setToggleButton( true );
	buttongroup->insert( button );
	m_inputs.push_back( button );
	buttongroup = new QHButtonGroup( "DADS", m_advancedwidget );
	connect( buttongroup, SIGNAL( clicked( int ) ),
			this, SLOT( setDADSInputs( int ) ) );
	hlayout->addWidget( buttongroup );
	button = new QPushButton( "5 - Front", buttongroup );
	button->setToggleButton( true );
	buttongroup->insert( button );
	m_inputs.push_back( button );
	button = new QPushButton( "6 - Bottom", buttongroup );
	button->setToggleButton( true );
	buttongroup->insert( button );
	m_inputs.push_back( button );
	button = new QPushButton( "7 - Left", buttongroup );
	button->setToggleButton( true );
	buttongroup->insert( button );
	m_inputs.push_back( button );
	button = new QPushButton( "8 - Right", buttongroup );
	button->setToggleButton( true );
	buttongroup->insert( button );
	m_inputs.push_back( button );
	hlayout = new QHBoxLayout( mainlayout );
	buttongroup = new QHButtonGroup( "Display", m_advancedwidget );
	connect( buttongroup, SIGNAL( clicked( int ) ),
			this, SLOT( setOutputs( int ) ) );
	hlayout->addWidget( buttongroup );
	button = new QPushButton( "1 - Front ()", buttongroup );
	buttongroup->insert( button );
	m_outputs.push_back( button );
	button = new QPushButton( "2 - Bottom ()", buttongroup );
	buttongroup->insert( button );
	m_outputs.push_back( button );
	button = new QPushButton( "3 - Left ()", buttongroup );
	buttongroup->insert( button );
	m_outputs.push_back( button );
	button = new QPushButton( "4 - Right ()", buttongroup );
	buttongroup->insert( button );
	m_outputs.push_back( button );
	hlayout = new QHBoxLayout( mainlayout );
	button = new QPushButton( "Apply", m_advancedwidget );
	hlayout->addWidget( button );
	connect( button, SIGNAL( clicked() ), this, SLOT( applyTies() ) );
	show();
}

int displaySwitcherMainWindow::initialize()
{
	m_clientsocket = new dSocket( 65530, "hammer.sv.vt.edu" );
	if( !m_clientsocket )
	{
		QMessageBox::critical( this, "Network Error", "Unable to create socket\n", "&Exit" );
		close();
	}
	if( !m_clientsocket->connectSocket() )
	{
		QMessageBox::critical( this, "Network Error", "Unable to connect to "
				"hammer.sv.vt.edu\n", "&Exit" );
		close();
	}
	return 0;
}

displaySwitcherMainWindow::~displaySwitcherMainWindow()
{
	delete m_clientsocket;
}

void displaySwitcherMainWindow::setInstantUpdates( bool updates )
{
	m_instantupdates = updates;
}

void displaySwitcherMainWindow::setCAVEInputs( int index )
{
	if( m_inputs[index]->isOn() )
	{
		m_selectedinput = index;
		for( int i=0;i<(int)m_inputs.size();i++ )
		{
			if( i != index )
			{
				if( m_inputs[i]->isOn() )
					m_inputs[i]->setOn( false );
			}
		}
	}
	else
		m_selectedinput = -1;
}

void displaySwitcherMainWindow::setDADSInputs( int index )
{
	setCAVEInputs( index + 4 );
}

void displaySwitcherMainWindow::setOutputs( int index )
{
	if( m_selectedinput > -1 )
	{
		m_ties[index] = m_selectedinput;
		m_inputs[m_selectedinput]->setOn( false );
		setOutputTies( index, m_selectedinput );
		if( m_instantupdates )
			tieInput( m_selectedinput, index );
	}
}

void displaySwitcherMainWindow::setOutputTies( int index, int value )
{
	m_ties[index] = value;
	char temp[8];
	sprintf( temp, "%d", value + 1 );
	string buttontext = m_outputs[index]->text().ascii();
	unsigned int startpos = buttontext.find_first_of( "(", 0 ) + 1;
	unsigned int endpos = buttontext.find_first_of( ")", startpos );
	buttontext.replace( startpos, endpos - startpos, temp );
	m_outputs[index]->setText( buttontext.c_str() );
}

void displaySwitcherMainWindow::applyTies()
{
	tieMultiInput();
}

void displaySwitcherMainWindow::showAdvanced( bool shown )
{
	if( shown )
		viewPresetConfig();
}

int displaySwitcherMainWindow::tieInput( int input, int output )
{
	char temp[256];
	sprintf( temp, "TIE %d %d", input + 1, output + 1 );
	if( !m_clientsocket->writeSocket( temp, strlen( temp ) ) )
	{
		printf( "ERROR: Failed to write to socket\n" );
		return -1;
	}
	if( !m_clientsocket->readSocket() )
	{
		printf( "ERROR: Failed to read from socket\n" );
		return -1;
	}
	string response = (char*)m_clientsocket->getBuffer();
	return 0;
}

int displaySwitcherMainWindow::tieMultiInput()
{
	char temp[256] = "MULTI ";
	map<int,int>::iterator itr;
	for( itr=m_ties.begin();itr!=m_ties.end();itr++ )
		sprintf( temp, "%s %d %d", temp, itr->second + 1, itr->first + 1 );
	printf( "%s\n", temp );
	if( !m_clientsocket->writeSocket( temp, strlen( temp ) ) )
	{
		printf( "ERROR: Failed to write to socket\n" );
		return -1;
	}
	if( !m_clientsocket->readSocket() )
	{
		printf( "ERROR: Failed to read from socket\n" );
		return -1;
	}
	string response = (char*)m_clientsocket->getBuffer();
	printf( "response: %s\n", response.c_str() );
	return 0;
}

int displaySwitcherMainWindow::recallPreset( int input )
{
	char temp[256] = "RECALL";
	sprintf( temp, "%s %d", temp, input );
	if( !m_clientsocket->writeSocket( temp, strlen( temp ) ) )
	{
		printf( "ERROR: Failed to write to socket\n" );
		return -1;
	}
	if( !m_clientsocket->readSocket() )
	{
		printf( "ERROR: Failed to read from socket\n" );
		return -1;
	}
	string response = (char*)m_clientsocket->getBuffer();
	viewPresetConfig();
	return 0;
}

int displaySwitcherMainWindow::lockPanel( bool lock )
{
	char temp[256] = "LOCK";
	if( !m_clientsocket->writeSocket( temp, strlen( temp ) ) )
	{
		printf( "ERROR: Failed to write to socket\n" );
		return -1;
	}
	if( !m_clientsocket->readSocket() )
	{
		printf( "ERROR: Failed to read from socket\n" );
		return -1;
	}
	string response = (char*)m_clientsocket->getBuffer();
	return 0;
}

int displaySwitcherMainWindow::viewOutputTie( int output )
{
	char temp[256] = "UNLOCK";
	if( !m_clientsocket->writeSocket( temp, strlen( temp ) ) )
	{
		printf( "ERROR: Failed to write to socket\n" );
		return -1;
	}
	if( !m_clientsocket->readSocket() )
	{
		printf( "ERROR: Failed to read from socket\n" );
		return -1;
	}
	string response = (char*)m_clientsocket->getBuffer();
	return 0;
}

int displaySwitcherMainWindow::viewPresetConfig( int preset )
{
	printf( "1\n" );
	char temp[256] = "VIEWPRESET ";
	sprintf( temp, "%s %d", temp, preset );
	if( !m_clientsocket->writeSocket( temp, strlen( temp ) ) )
	{
		printf( "ERROR: Failed to write to socket\n" );
		return -1;
	}
	if( !m_clientsocket->readSocket() )
	{
		printf( "ERROR: Failed to read from socket\n" );
		return -1;
	}
	printf( "2\n" );
	if( m_clientsocket->getSize() )
	{
		string response = (char*)m_clientsocket->getBuffer();
		printf( "response: %s\n", response.c_str() );
		bool done = false;
		int output = 0;
		unsigned int startpos = response.find_first_of( "0123456789", 0 );
		unsigned int endpos = response.find_first_not_of( "0123456789", startpos );
		string valstr = response.substr( startpos, endpos - startpos );
		printf( "3\n" );
		while( !done )
		{
			printf( "output: %d\n", output );
			if( valstr == "Vid" )
			{
				done = true;
				continue;
			}
			int input = atoi( valstr.c_str() ) - 1;
			setOutputTies( output, input );
			output++;
			startpos = response.find_first_not_of( " ", endpos );
			endpos = response.find_first_of( " ", startpos );
			valstr = response.substr( startpos, endpos - startpos );
		}
	}
	return 0;
}

int displaySwitcherMainWindow::infoRequest()
{
	m_clientsocket->writeSocket( "INFO", 4 );
	m_clientsocket->readSocket();
	string msg = (char*)m_clientsocket->getBuffer();
	if( msg.size() )
	{
		string value;
		unsigned int startpos = msg.find_first_of( "V", 0 ) + 1;
		unsigned int endpos = msg.find_first_of( "X", startpos );
		value = msg.substr( startpos, endpos - startpos );
		m_videoinputs = atoi( value.c_str() );
		startpos = endpos++;
		endpos = msg.find_first_of( " ", startpos );
		value = msg.substr( startpos, endpos - startpos );
		m_videooutputs = atoi( value.c_str() );
		startpos = msg.find_first_of( "A", endpos ) + 1;
		endpos = msg.find_first_of( "X", startpos );
		value = msg.substr( startpos, endpos - startpos );
		m_audioinputs = atoi( value.c_str() );
		startpos = endpos++;
		endpos = msg.find_first_of( " ", startpos );
		value = msg.substr( startpos, endpos - startpos );
		m_audiooutputs = atoi( value.c_str() );
		startpos = msg.find_first_of( "01", endpos );
		m_lockedpanel = atoi( msg.substr( startpos, 1 ).c_str() );
	}
	return 0;
}

