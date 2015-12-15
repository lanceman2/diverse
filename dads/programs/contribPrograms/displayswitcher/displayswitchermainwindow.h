#ifndef _DISPLAYSWITCHERMAINWINDOW_H_
#define _DISPLAYSWITCHERMAINWINDOW_H_

#include "dSocket.h"
#include <qmainwindow.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <vector>
#include <map>

using namespace std;

class displaySwitcherMainWindow : public QMainWindow
{
	Q_OBJECT
public:
	displaySwitcherMainWindow( QWidget* parent = NULL );
	~displaySwitcherMainWindow();
	int tieInput( int input, int output );
	int tieMultiInput();
	int recallPreset( int input );
	int lockPanel( bool lock );
	int viewOutputTie( int output );
	int viewPresetConfig( int preset = 0 );
	int infoRequest();
	int initialize();
	void setOutputTies( int index, int value );

public slots:
	void setInstantUpdates( bool updates );
	void setCAVEInputs( int index );
	void setDADSInputs( int index );
	void setOutputs( int index );
	void presetCAVE() { recallPreset( 1 ); };
	void presetDADS() { recallPreset( 2 ); };
	void applyTies();
	void showAdvanced( bool shown );

private:
	int m_videoinputs;
	int m_videooutputs;
	int m_audioinputs;
	int m_audiooutputs;
	int m_selectedinput;
	bool m_lockedpanel;
	bool m_instantupdates;
	QPushButton* m_cavebutton;
	QPushButton* m_dadsbutton;
	QPushButton* m_loadbutton;
	QPushButton* m_advancedbutton;
	QPushButton* m_accept;
	QPushButton* m_reject;
	QPushButton* m_apply;
	QCheckBox* m_instant;
	QWidget* m_advancedwidget;
	vector<QPushButton*> m_inputs;
	vector<QPushButton*> m_outputs;
	map<int,int> m_ties;
	dSocket* m_clientsocket;
};

#endif

