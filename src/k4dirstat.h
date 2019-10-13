/*
 *   File name:	k4dirstat.h
 *   Summary:	The K4DirStat Application
 *   License:	GPL - See file COPYING for details.
 *
 *   Author:	Stefan Hundhammer <kdirstat@gmx.de>
 *		Joshua Hodosh <kdirstat@grumpypenguin.org>
 *		Parts auto-generated by KDevelop
 *
 *   Updated:	2010-03-01
 */

#ifndef K4DIRSTAT_H
#define K4DIRSTAT_H

#include <kxmlguiwindow.h>

class QPrinter;
class KUrl;

class QSplitter;
class KActivityTracker;
class KFeedbackDialog;
class KPacMan;
class KAction;
class KRecentFilesAction;
class KToggleAction;

namespace KDirStat {
class KCleanupCollection;
class KDirTreeView;
class KDirTreeViewItem;
class KDirTree;
class KFileInfo;
class KSettingsDialog;
class KTreemapView;
class KTreemapTile;
} // namespace KDirStat

using namespace KDirStat;

/**
 * This class serves as the main window for k4dirstat.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author %{AUTHOR} <%{EMAIL}>
 * @version %{VERSION}
 */
class k4dirstat : public KXmlGuiWindow {
  Q_OBJECT
public:
  /**
   * Default Constructor
   */
  k4dirstat();

  static k4dirstat *instance() { return instance_; }

  /**
   * Default Destructor
   */
  virtual ~k4dirstat();

  /**
   * Open an URL specified by command line argument.
   **/
  void openURL(const QUrl &url);

  /**
   * Return the main window's @ref KDirTreeView.
   **/
  KDirTreeView *treeView() const { return _treeView; }

  /**
   * Returns the main window's @ref KTreemapView or 0 if there is none.
   *
   * Caution: Do not try to cache this value. The treemap view is destroyed
   * and re-created frequently!
   **/
  KTreemapView *treemapView() const { return _treemapView; }

public slots:
  /**
   * Open a directory tree.
   **/
  void fileAskOpenDir();

  /**
   * Open a (possibly remote) directory tree.
   **/
  void fileAskOpenUrl();

  /**
   * Refresh the entire directory tree, i.e. re-read everything from disk.
   **/
  void refreshAll();

  /**
   * Refresh the selected subtree, i.e. re-read it from disk.
   **/
  void refreshSelected();

  /**
   * Refresh the entire directory tree, i.e. re-read everything from disk.
   **/
  void stopReading();

  /**
   * Open a directory tree from the "recent" menu.
   **/
  void fileOpenRecent(const QUrl &url);

  /**
   * asks for saving if the file is modified, then closes the current file
   * and window
   **/
  void fileCloseDir();

  /**
   * put the marked text/object into the clipboard
   **/
  void editCopy();

  /**
   * Notification that the view's selection has changed.
   * Enable/disable user actions as appropriate.
   **/
  void selectionChanged(KDirTree*);

  /**
   * Ask user what application to open a file or directory with
   **/
  void cleanupOpenWith();

  /**
   * Toggle treemap view
   **/
  void toggleTreemapView();

  /**
   * Zoom in the treemap at the currently selected tile.
   **/
  void treemapZoomIn();

  /**
   * Zoom out the treemap after zooming in.
   **/
  void treemapZoomOut();

  /**
   * Select the parent of the currently selected treemap tile.
   **/
  void treemapSelectParent();

  /**
   * Rebuild the treemap.
   **/
  void treemapRebuild();

  /**
   * Invoke online help about treemaps.
   **/
  void treemapHelp();

  /**
   * Open settings dialog
   **/
  void preferences();

  /**
   * Changes the statusbar contents for the standard label permanently, used
   * to indicate current actions.
   *
   * @param text the text that is displayed in the statusbar
   **/
  void statusMsg(const QString &text);

  /**
   * Opens a context menu for tree view items.
   **/
  void contextMenu(const QPoint &pos);

  /**
   * Opens a context menu for treemap tiles.
   **/
  void contextMenu(KTreemapTile *tile, const QPoint &pos);

  /**
   * Create a treemap view. This makes only sense after a directory tree is
   * completely read.
   **/
  void createTreemapView();

  /**
   * Create a treemap view after all events are processed.
   **/
  void createTreemapViewDelayed();

  /**
   * Delete an existing treemap view if there is one.
   **/
  void deleteTreemapView();

  /**
   * Sends a user feedback mail.
   **/
  // void sendFeedbackMail();

  /**
   * Read configuration for the main window.
   **/
  void readMainWinConfig();

  /**
   * Save the main window's configuration.
   **/
  void saveMainWinConfig();

  /**
   * Revert all cleanups to default values.
   **/
  void revertCleanupsToDefaults();

  /**
   * For the settings dialog only: Return the internal cleanup collection.
   **/
  KCleanupCollection *cleanupCollection() { return _cleanupCollection; }

  /**
   * Returns true if the pacman animation in the tool bar is enabled, false
   * otherwise.
   **/
  bool pacManEnabled() const { return _pacMan != 0; }

  /**
   * Ask user if he wouldn't like to rate this program.
   **/
  // void askForFeedback();

  /**
   * Notification that a feedback mail has been sent, thus don't remind
   * the user any more.
   **/
  // void feedbackMailSent();

  /**
   * Update enabled/disabled state of the user actions.
   **/
  void updateActions();

  /**
   * Open a file selection box to save the current directory tree to a
   * kdirstat cache file
   **/
  void askWriteCache();

  /**
   * Open a file selection box to read a directory tree from a kdirstat cache
   * file
   **/
  void askReadCache();

private slots:
  void triggerSaveConfig();

signals:

  /**
   * Emitted when the configuration is to be read - other than at program
   * startup / object creation where each object is responsible for reading
   * its configuraton at an appropriate time.
   **/
  void readConfig();

  /**
   * Emitted when the configuration is to be saved.
   **/
  void saveConfig();

  // private slots:
  //    void optionsPreferences();

protected:
  /**
   * Initialize @ref KCleanup actions.
   **/
  void initCleanups();

  /**
   * Set up status bar for the main window by initializing a status label.
   **/
  void initStatusBar();

  // Widgets

  QSplitter *_splitter;
  KDirTreeView *_treeView;
  KTreemapView *_treemapView;
  KPacMan *_pacMan;
  QWidget *_pacManDelimiter;
  QMenu *_treeViewContextMenu;
  QMenu *_treemapContextMenu;
  KDirStat::KSettingsDialog *_settingsDialog;
  KFeedbackDialog *_feedbackDialog;
  KActivityTracker *_activityTracker;

  QAction *_fileAskOpenDir;
  QAction *_fileAskOpenUrl;
  KRecentFilesAction *_fileOpenRecent;
  QAction *_fileCloseDir;
  QAction *_fileRefreshAll;
  QAction *_fileRefreshSelected;
  QAction *_fileReadExcludedDir;
  QAction *_fileContinueReadingAtMountPoint;
  QAction *_fileStopReading;
  QAction *_fileAskWriteCache;
  QAction *_fileAskReadCache;
  QAction *_fileQuit;
  QAction *_editCopy;
  QAction *_cleanupOpenWith;
  QAction *_treemapZoomIn;
  QAction *_treemapZoomOut;
  QAction *_treemapSelectParent;
  QAction *_treemapRebuild;

  QAction *_reportMailToOwner;
  QAction *_helpSendFeedbackMail;
  KToggleAction *_showTreemapView;

  KCleanupCollection *_cleanupCollection;

  int _treemapViewHeight;

private:
  void setupActions();
  static k4dirstat *instance_;

private:
  QPrinter *m_printer;
};

#endif // _K4DIRSTAT_H_
