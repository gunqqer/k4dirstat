/*
 *   File name:	kdirstatapp.cpp
 *   Summary:	The KDirStat application - menu bar, tool bar, ...
 *   License:	GPL - See file COPYING for details.
 *
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *		Parts auto-generated by KDevelop
 *
 *   Updated:	2002-05-10
 *
 *   $Id: kdirstatapp.cpp,v 1.14 2002/05/12 15:53:51 hundhammer Exp $
 *
 */


#include <kapp.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <klocale.h>
#include <kconfig.h>
#include <kstdaction.h>
#include <qclipboard.h>
#include <qpopupmenu.h>
#include <qsplitter.h>

#include "kdirstatapp.h"
#include "kcleanupcollection.h"
#include "kdirtree.h"
#include "kpacman.h"
#include "kactivitytracker.h"


#define	USER_CLEANUPS	10	// Number of user cleanup actions

#define ID_STATUS_MSG	1
#define ID_PACMAN	42
#define PACMAN_WIDTH	350
#define PACMAN_INTERVAL	75	// millisec

#define INITIAL_FEEDBACK_REMINDER	2000L
#define FEEDBACK_REMINDER_INTERVAL	1000L


using namespace KDirStat;


KDirStatApp::KDirStatApp( QWidget* , const char* name )
    : KMainWindow( 0, name )
{
    // Simple inits

    _activityTracker	= 0;	// Might or might not be needed
    _settingsDialog 	= 0;	// Delayed creation - when needed
    _feedbackDialog 	= 0;	// Delayed creation - when needed
    _treeMapView	= 0;	// Delayed creation - when needed


    // Set up internal (mainWin -> mainWin) connections

    connect( this,	SIGNAL( readConfig       ( void ) ),
	     this,	SLOT  ( readMainWinConfig( void ) ) );

    connect( this,	SIGNAL( saveConfig       ( void ) ),
	     this,	SLOT  ( saveMainWinConfig( void ) ) );


    // Create main window

    _splitter = new QSplitter( QSplitter::Vertical, this );
    setCentralWidget( _splitter );

    _treeView = new KDirTreeView( _splitter );

    connect( _treeView, SIGNAL( progressInfo( const QString & ) ),
	     this,      SLOT  ( statusMsg   ( const QString & ) ) );

    connect( _treeView, SIGNAL( selectionChanged( KFileInfo * ) ),
	     this,      SLOT  ( selectionChanged( KFileInfo * ) ) );

    connect( _treeView, SIGNAL( contextMenu( KDirTreeViewItem *, const QPoint & ) ),
	     this,      SLOT  ( contextMenu( KDirTreeViewItem *, const QPoint & ) ) );

    connect( this,	SIGNAL( readConfig() ),
	     _treeView,	SLOT  ( readConfig() ) );

    connect( this,	SIGNAL( saveConfig() ),
	     _treeView,	SLOT  ( saveConfig() ) );

    connect( _treeView, SIGNAL( finished()		),
	     this, 	SLOT  ( createTreeMapView() )	);

    connect( _treeView, SIGNAL( startingReading()	),
	     this, 	SLOT  ( deleteTreeMapView() )	);

    
    // Call inits to invoke all other construction parts

    initStatusBar();
    initActions();
    initCleanups();
    createGUI();

    _treeViewContextMenu = (QPopupMenu *) factory()->container( "treeViewContextMenu", this );
    initPacMan();

    readMainWinConfig();
    initActivityTracker();


    // Disable certain actions at startup

    _editCopy->setEnabled( false );
    _reportMailToOwner->setEnabled( false );
    _fileRefreshAll->setEnabled( false );
    _fileRefreshSelected->setEnabled( false );
}


KDirStatApp::~KDirStatApp()
{
    delete _cleanupCollection;
}



void
KDirStatApp::initActions()
{
    _fileAskOpenDir	= KStdAction::open		( this, SLOT( fileAskOpenDir() ), 		actionCollection() );
    _fileOpenRecent	= KStdAction::openRecent	( this, SLOT( fileOpenRecent( const KURL& ) ),	actionCollection() );
    _fileCloseDir	= KStdAction::close		( this, SLOT( fileCloseDir() ), 		actionCollection() );

    _fileRefreshAll		= new KAction( i18n( "Refresh &All" ), "reload", 0,
					       this, SLOT( refreshAll() ),
					       actionCollection(), "file_refresh_all" );

    _fileRefreshSelected	= new KAction( i18n( "Refresh &Selected" ), 0,
					       this, SLOT( refreshSelected() ),
					       actionCollection(), "file_refresh_selected" );

    _fileContinueReadingAtMountPoint = new KAction( i18n( "Continue Reading at &Mount Point" ), "hdd_mount", 0,
					       this, SLOT( refreshSelected() ),
					       actionCollection(), "file_continue_reading_at_mount_point" );

    _fileQuit		= KStdAction::quit		( kapp, SLOT( quit()  		), actionCollection() );
    _editCopy		= KStdAction::copy		( this, SLOT( editCopy() 	), actionCollection() );
    _viewToolBar	= KStdAction::showToolbar	( this, SLOT( toggleToolBar()	), actionCollection() );
    _viewStatusBar	= KStdAction::showStatusbar	( this, SLOT( toggleStatusBar() ), actionCollection() );
    KAction * pref	= KStdAction::preferences	( this, SLOT( preferences()	), actionCollection() );

    _reportMailToOwner	= new KAction( i18n( "Send &Mail to Owner" ), "mail_generic", 0,
				       _treeView, SLOT( sendMailToOwner() ),
				       actionCollection(), "report_mail_to_owner" );

    _helpSendFeedbackMail = new KAction( i18n( "Send &Feedback Mail..." ), 0,
					 this, SLOT( sendFeedbackMail() ),
					 actionCollection(), "help_send_feedback_mail" );


    _fileAskOpenDir->setStatusText	( i18n( "Opens a directory"	 		) );
    _fileOpenRecent->setStatusText	( i18n( "Opens a recently used directory"	) );
    _fileCloseDir->setStatusText	( i18n( "Closes the current directory" 		) );
    _fileRefreshAll->setStatusText	( i18n( "Re-reads the entire directory tree"	) );
    _fileRefreshSelected->setStatusText	( i18n( "Re-reads the selected subtree"		) );
    _fileContinueReadingAtMountPoint->setStatusText( i18n( "Scan mounted file systems"	) );
    _fileQuit->setStatusText		( i18n( "Quits the application" 		) );
    _editCopy->setStatusText		( i18n( "Copies the URL of the selected item to the clipboard" ) );
    _viewToolBar->setStatusText		( i18n( "Enables/disables the toolbar" 		) );
    _viewStatusBar->setStatusText	( i18n( "Enables/disables the statusbar" 	) );
    pref->setStatusText			( i18n( "Opens the preferences dialog"		) );
    _reportMailToOwner->setStatusText	( i18n( "Sends a mail to the owner of the selected subtree" ) );
}


void
KDirStatApp::initCleanups()
{
    _cleanupCollection = new KCleanupCollection( actionCollection() );
    CHECK_PTR( _cleanupCollection );
    _cleanupCollection->addStdCleanups();
    _cleanupCollection->addUserCleanups( USER_CLEANUPS );
    _cleanupCollection->slotReadConfig();

    connect( _treeView,          SIGNAL( selectionChanged( KFileInfo * ) ),
	     _cleanupCollection, SIGNAL( selectionChanged( KFileInfo * ) ) );

    connect( this,               SIGNAL( readConfig( void ) ),
	     _cleanupCollection, SIGNAL( readConfig( void ) ) );

    connect( this,               SIGNAL( saveConfig( void ) ),
	     _cleanupCollection, SIGNAL( saveConfig( void ) ) );
}


void
KDirStatApp::revertCleanupsToDefaults()
{
    KCleanupCollection defaultCollection;
    defaultCollection.addStdCleanups();
    defaultCollection.addUserCleanups( USER_CLEANUPS );
    *_cleanupCollection = defaultCollection;
}


void
KDirStatApp::initPacMan()
{
    _pacMan = new KPacMan( toolBar() );
    _pacMan->setInterval( PACMAN_INTERVAL );	// millisec
    int id = ID_PACMAN;
    toolBar()->insertWidget( id, PACMAN_WIDTH, _pacMan );
    toolBar()->setItemAutoSized( id, false );
    toolBar()->insertWidget( ++id, 1, new QWidget( toolBar() ) );

    connect( _treeView, SIGNAL( startingReading() ), _pacMan, SLOT( start() ) );
    connect( _treeView, SIGNAL( finished()        ), _pacMan, SLOT( stop () ) );
}


void
KDirStatApp::initStatusBar()
{
    statusBar()->insertItem( i18n( "Ready." ), ID_STATUS_MSG );
}


void
KDirStatApp::initActivityTracker()
{
    if ( ! doFeedbackReminder() )
	return;

    _activityTracker = new KActivityTracker( this, "Feedback",
					     INITIAL_FEEDBACK_REMINDER );

    connect( _activityTracker,  SIGNAL( thresholdReached() ),
	     this,		SLOT  ( askForFeedback() ) );

    connect( _treeView,		SIGNAL( userActivity( int ) ),
	     _activityTracker,	SLOT  ( trackActivity( int ) ) );

    connect( _cleanupCollection, SIGNAL( userActivity( int ) ),
	     _activityTracker,   SLOT  ( trackActivity( int ) ) );
}


void
KDirStatApp::openURL( const KURL& url )
{
    statusMsg( i18n( "Opening directory..." ) );

    _treeView->openURL( url );
    _fileOpenRecent->addURL( url );
    _fileRefreshAll->setEnabled( true );
    setCaption( url.fileName(), false );

    statusMsg( i18n( "Ready." ) );
}


void KDirStatApp::readMainWinConfig()
{

    KConfig * config = kapp->config();
    config->setGroup( "General Options" );

    // Status settings of the various bars

    bool showToolbar = config->readBoolEntry( "Show Toolbar", true );
    _viewToolBar->setChecked( showToolbar );
    toggleToolBar();

    bool showStatusbar = config->readBoolEntry( "Show Statusbar", true );
    _viewStatusBar->setChecked( showStatusbar );
    toggleStatusBar();


    // Position settings of the various bars

    KToolBar::BarPosition toolBarPos;
    toolBarPos = ( KToolBar::BarPosition ) config->readNumEntry( "ToolBarPos", KToolBar::Top );
    toolBar( "mainToolBar" )->setBarPos( toolBarPos );

    // initialize the recent file list
    _fileOpenRecent->loadEntries( config,"Recent Files" );

    QSize size = config->readSizeEntry( "Geometry" );

    if( ! size.isEmpty() )
	resize( size );
}


void
KDirStatApp::saveMainWinConfig()
{
    KConfig * config = kapp->config();

    config->setGroup( "General Options" );

    config->writeEntry( "Geometry", 		size() );
    config->writeEntry( "Show Toolbar", 	_viewToolBar->isChecked() );
    config->writeEntry( "Show Statusbar",	_viewStatusBar->isChecked() );
    config->writeEntry( "ToolBarPos", 		(int ) toolBar( "mainToolBar" )->barPos() );

    _fileOpenRecent->saveEntries( config,"Recent Files" );
}


void
KDirStatApp::saveProperties( KConfig *config )
{
    (void) config;
    // TODO
}


void
KDirStatApp::readProperties( KConfig *config )
{
    (void) config;
    // TODO
}


bool
KDirStatApp::queryClose()
{
    return true;
}

bool
KDirStatApp::queryExit()
{
    emit saveConfig();

    return true;
}


//============================================================================
//				     Slots
//============================================================================


void
KDirStatApp::fileAskOpenDir()
{
    statusMsg( i18n( "Opening directory..." ) );

    KURL url = KFileDialog::getExistingDirectory( QString::null, this, i18n( "Open Directory..." ) );

    if( ! url.isEmpty() )
	openURL( fixedUrl( url.url() ) );

    statusMsg( i18n( "Ready." ) );
}


void
KDirStatApp::fileOpenRecent( const KURL& url )
{
    statusMsg( i18n( "Opening directory..." ) );

    if( ! url.isEmpty() )
	openURL( fixedUrl( url.url() ) );

    statusMsg( i18n( "Ready." ) );
}


void
KDirStatApp::fileCloseDir()
{
    statusMsg( i18n( "Closing directory..." ) );

    _treeView->clear();
    _fileRefreshAll->setEnabled( false );
    close();

    statusMsg( i18n( "Ready." ) );
}


void
KDirStatApp::refreshAll()
{
    statusMsg( i18n( "Refreshing directory tree..." ) );
    _treeView->refreshAll();
    statusMsg( i18n( "Ready." ) );
}


void
KDirStatApp::refreshSelected()
{
    if ( ! _treeView->selection() )
	return;

    statusMsg( i18n( "Refreshing selected subtree..." ) );
    _treeView->refreshSelected();
    statusMsg( i18n( "Ready." ) );
}


void
KDirStatApp::editCopy()
{
    if ( _treeView->selection() )
	kapp->clipboard()->setText( QString::fromLocal8Bit(_treeView->selection()->orig()->url()) );

#if 0
#warning debug
    if ( _activityTracker )
	_activityTracker->trackActivity( 800 );
#endif
}


void
KDirStatApp::selectionChanged( KFileInfo *selection )
{
    if ( selection )
    {
	_editCopy->setEnabled( true );
	_reportMailToOwner->setEnabled( true );
	_fileRefreshSelected->setEnabled( ! selection->isDotEntry() );

	if ( selection->isMountPoint() &&
	     selection->readState() == KDirOnRequestOnly )
	{
	    _fileContinueReadingAtMountPoint->setEnabled( true );
	}
	else
	    _fileContinueReadingAtMountPoint->setEnabled( false );

	statusMsg( QString::fromLocal8Bit(selection->url()) );
    }
    else
    {
	_editCopy->setEnabled( false );
	_reportMailToOwner->setEnabled( false );
	_fileRefreshSelected->setEnabled( false );
	_fileContinueReadingAtMountPoint->setEnabled( false );
	statusMsg( "" );
    }
}


void
KDirStatApp::toggleToolBar()
{
    if   ( _viewToolBar->isChecked() )	toolBar( "mainToolBar" )->show();
    else				toolBar( "mainToolBar" )->hide();
}


void
KDirStatApp::toggleStatusBar()
{
    if   ( _viewStatusBar->isChecked() )	statusBar()->show();
    else					statusBar()->hide();
}


void
KDirStatApp::preferences()
{
    if ( ! _settingsDialog )
    {
	_settingsDialog = new KDirStat::KSettingsDialog( this );
	CHECK_PTR( _settingsDialog );
    }

    if ( ! _settingsDialog->isVisible() )
	_settingsDialog->show();
}


void
KDirStatApp::askForFeedback()
{
    if ( ! doFeedbackReminder() )
	return;

    KConfig * config = kapp->config();

    switch ( KMessageBox::warningYesNoCancel( this,
					      i18n( "Now that you know this program for some time,\n"
						    "wouldn't you like to tell the authors your opinion about it?\n"
						    "\n"
						    "Open Source software depends on user feedback.\n"
						    "Your opinion can help us make the software better." ),
					      i18n( "Please tell us your opinion!" ),	// caption
					      i18n( "Open &Feedback Form..." ),		// yesButton
					      i18n( "&No, and don't ask again!" )	// noButton
					      )
	     )
    {
	case KMessageBox::Yes:
	    sendFeedbackMail();
	    break;

	case KMessageBox::No:	// ...and don't ask again
	    config->setGroup( "Feedback" );
	    config->writeEntry( "dontAsk", true );
	    config->sync();	// make sure this doesn't get lost even if the app is killed or crashes
	    break;

	case KMessageBox::Cancel:
	    break;
    }

    config->setGroup( "Feedback" );
    int  remindersCount = config->readNumEntry ( "remindersCount", 0 );
    config->writeEntry( "remindersCount", ++remindersCount );

    if ( _activityTracker )
    {
	_activityTracker->setThreshold( _activityTracker->threshold()
					+ FEEDBACK_REMINDER_INTERVAL );
    }
}


void
KDirStatApp::feedbackMailSent()
{
    KConfig * config = kapp->config();
    config->setGroup( "Feedback" );
    config->writeEntry( "mailSent", true );
    config->sync();
}


bool
KDirStatApp::doFeedbackReminder()
{
    KConfig * config = kapp->config();
    config->setGroup( "Feedback" );

    bool mailSent	= config->readBoolEntry( "mailSent", false );
    bool dontAsk	= config->readBoolEntry( "dontAsk", false );
    int  remindersCount = config->readNumEntry ( "remindersCount", 0 );

    return !mailSent && !dontAsk && remindersCount < 5;
}


void
KDirStatApp::statusMsg( const QString &text )
{
    // Change status message permanently

    statusBar()->clear();
    statusBar()->changeItem( text, ID_STATUS_MSG );
}


void
KDirStatApp::contextMenu( KDirTreeViewItem *item, const QPoint &pos )
{
    NOT_USED( item );

    if ( _treeViewContextMenu )
	_treeViewContextMenu->popup( pos );
}


void
KDirStatApp::createTreeMapView()
{
    if ( _treeMapView )
	delete _treeMapView;

    _treeMapView = new KTreeMapView( _treeView->tree(), _splitter );
    CHECK_PTR( _treeMapView );
    _treeMapView->show(); // QSplitter needs an explicit show() for new children
}


void
KDirStatApp::deleteTreeMapView()
{
    if ( _treeMapView )
	delete _treeMapView;

    _treeMapView = 0;
}



// EOF
