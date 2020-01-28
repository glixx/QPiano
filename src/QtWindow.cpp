/*!
    @file           QtWindow.cpp

    @brief          xxx.

    @author         L. J. Barman

    Copyright (c)   2008-2013, L. J. Barman, all rights reserved

    This file is part of the PianoBooster application

    PianoBooster is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    PianoBooster is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PianoBooster.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "GlView.h"
#include "QtWindow.h"
#include "version.txt"

#ifdef __linux__
#ifndef USE_REALTIME_PRIORITY
#define USE_REALTIME_PRIORITY 0
#endif
#endif

#if USE_REALTIME_PRIORITY
#include <sched.h>

/* sets the process to "policy" policy at given priority */
static int set_realtime_priority(int policy, int prio)
{
    struct sched_param schp;
    memset(&schp, 0, sizeof(schp));

    schp.sched_priority = prio;
    if (sched_setscheduler(0, policy, &schp) != 0) {
        perror("sched_setscheduler");
        return -1;
    }
    return 0;
}
#endif


QtWindow::QtWindow()
{
    m_settings = new CSettings(this);
    setWindowIcon(QIcon(":/images/pianobooster.png"));
    setWindowTitle(tr("Piano Booster"));

    Cfg::setDefaults();

    decodeCommandLine();

    if (Cfg::experimentalSwapInterval != -1)
    {
        QGLFormat fmt;
        fmt.setSwapInterval(Cfg::experimentalSwapInterval);
        int value = fmt.swapInterval();
        ppLogInfo("Open GL Swap Interval %d", value);
        QGLFormat::setDefaultFormat(fmt);
    }

    for (int i = 0; i < MAX_RECENT_FILES; ++i)
         m_recentFileActs[i] = 0;
    m_separatorAct = 0;


#if USE_REALTIME_PRIORITY
    int rt_prio = sched_get_priority_max(SCHED_FIFO);
    set_realtime_priority(SCHED_FIFO, rt_prio);
#endif

    m_glWidget = new CGLView(this, m_settings);
    m_glWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_song = m_glWidget->getSongObject();
    m_score = m_glWidget->getScoreObject();


    QHBoxLayout *mainLayout = new QHBoxLayout;
    QVBoxLayout *columnLayout = new QVBoxLayout;

    m_sidePanel = new GuiSidePanel(this, m_settings);
    m_topBar = new GuiTopBar(this, m_settings);
    m_tutorWindow = new QTextBrowser(this);
    m_tutorWindow->hide();

    m_settings->init(m_song, m_sidePanel, m_topBar);

    mainLayout->addWidget(m_sidePanel);
    columnLayout->addWidget(m_topBar);
    columnLayout->addWidget(m_glWidget);
    columnLayout->addWidget(m_tutorWindow);
    mainLayout->addLayout(columnLayout);

    m_song->init2(m_score, m_settings);

    m_sidePanel->init(m_song, m_song->getTrackList(), m_topBar);
    m_topBar->init(m_song);

    QWidget *centralWin = new QWidget();
    centralWin->setLayout(mainLayout);

    setCentralWidget(centralWin);

    m_glWidget->setFocus(Qt::ActiveWindowFocusReason);

    m_song->setPianoSoundPatches(m_settings->value("Keyboard/RightSound", Cfg::defaultRightPatch()).toInt() - 1,
                                 m_settings->value("Keyboard/WrongSound", Cfg::defaultWrongPatch()).toInt() - 1, true);

    QString midiInputName = m_settings->value("Midi/Input").toString();
    if (midiInputName.startsWith(tr("None")))
        CChord::setPianoRange(PC_KEY_LOWEST_NOTE, PC_KEY_HIGHEST_NOTE);
    else
        CChord::setPianoRange(m_settings->value("Keyboard/LowestNote", 0).toInt(),
                          m_settings->value("Keyboard/HighestNote", 127).toInt());

    m_song->setLatencyFix(m_settings->value("Midi/Latency", 0).toInt());


#if defined (Q_OS_LINUX) || defined (Q_OS_UNIX)
    m_glWidget->m_cfg_openGlOptimise = 0; //  two is full GlOptimise
#else
    m_glWidget->m_cfg_openGlOptimise = 0; //  1 is full GlOptimise
#endif

    if  (m_settings->value("Display/OpenGlOptimise").toString() == "true") // this used to be true for backward compatability
        m_settings->setValue("Display/OpenGlOptimise", m_glWidget->m_cfg_openGlOptimise );
    if  (m_settings->value("Display/OpenGlOptimise").toString() == "false") // this used to be boolean for backward compatability
    {
        m_glWidget->m_cfg_openGlOptimise = 0;
        m_settings->setValue("Display/OpenGlOptimise", m_glWidget->m_cfg_openGlOptimise );
    }

    m_glWidget->m_cfg_openGlOptimise = m_settings->value("Display/OpenGlOptimise", m_glWidget->m_cfg_openGlOptimise ).toInt();
    m_song->cfg_timingMarkersFlag = m_settings->value("Score/TimingMarkers", m_song->cfg_timingMarkersFlag ).toBool();
    m_song->cfg_stopPointMode = static_cast<stopPointMode_t> (m_settings->value("Score/StopPointMode", m_song->cfg_stopPointMode ).toInt());
    m_song->cfg_rhythmTapping = static_cast<rhythmTapping_t> (m_settings->value("Score/RtyhemTappingMode", m_song->cfg_rhythmTapping ).toInt());


    m_song->openMidiPort(CMidiDevice::MIDI_INPUT, midiInputName);
    m_song->openMidiPort(CMidiDevice::MIDI_OUTPUT,m_settings->value("Midi/Output").toString());

    readSettings();

    QTimer::singleShot(100, [&](){
        QString songName = m_settings->value("CurrentSong").toString();
        if (!songName.isEmpty())
            m_settings->openSongFile( songName );
    });


}

void QtWindow::init()
{

    m_settings->loadSettings();

    createActions();
    createMenus();
    readSettings();

    refreshTranslate();
    show();
}

QtWindow::~QtWindow()
{
    delete m_settings;
}

///////////////////////////////////////////////////////////////////////////////
//! @brief               Displays the usage
void QtWindow::displayUsage()
{
    fprintf(stdout, "Usage: pianobooster [flags] [midifile]\n");
    fprintf(stdout, "  -d, --debug             Increase the debug level.\n");
    fprintf(stdout, "  -q, --quick-start       Quick start.\n");
    fprintf(stdout, "      --Xnote-length      Displays the note length (experimental)\n");
    fprintf(stdout, "      --Xtick-rate=RATE   Adjust the tick rate in mSec (experimental).\n");
    fprintf(stdout, "                          default 4 (12 windows).\n");
    fprintf(stdout, "  -h, --help              Displays this help message.\n");
    fprintf(stdout, "  -v, --version           Displays version number and then exits.\n");
    fprintf(stdout, "  -l   --log              Write debug info to the \"pb.log\" log file.\n");
    fprintf(stdout, "       --midi-input-dump  Displays the midi input in hex.\n");
    fprintf(stdout, "       --lights:          Turns on the keyboard lights.\n");
}

int QtWindow::decodeIntegerParam(QString arg, int defaultParam)
{
    int n = arg.lastIndexOf('=');
    if (n == -1 || (n + 1) >= arg.size())
        return defaultParam;
    bool ok;
    int value = arg.mid(n+1).toInt(&ok);
    if (ok)
        return value;
    return defaultParam;
}

bool QtWindow::validateIntegerParam(QString arg)
{
    int n = arg.lastIndexOf('=');
    if (n == -1 || (n + 1) >= arg.size())
        return false;
    bool ok;
    arg.mid(n+1).toInt(&ok);
     return ok;
}
bool QtWindow::validateIntegerParamWithMessage(QString arg)
{
    bool ok = validateIntegerParam(arg);
    if (!ok) {
        fprintf(stderr, "ERROR: Invalid parameter to a command line argument \"%s\".\n", qPrintable(arg));
        exit(0);
    }
     return ok;
}

void QtWindow::decodeMidiFileArg(QString arg)
{

    QFileInfo fileInfo(arg);

    if (!fileInfo.exists() )
    {
        QMessageBox::warning(0, tr("PianoBooster Midi File Error"),
                 tr("Cannot open \"%1\"").arg(QString(fileInfo.absoluteFilePath())));
        exit(1);
    }
        else if ( !(fileInfo.fileName().endsWith(".mid", Qt::CaseInsensitive ) ||
             fileInfo.fileName().endsWith(".midi", Qt::CaseInsensitive ) ||
             fileInfo.fileName().endsWith(".kar", Qt::CaseInsensitive )) )
    {
        QMessageBox::warning(0, tr("PianoBooster Midi File Error"),
                 tr("\"%1\" is not a Midi File").arg(QString(fileInfo.fileName())));
        exit(1);
    }
    else
    {
        bool vaildMidiFile = true;
        QFile file(fileInfo.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly))
            vaildMidiFile = false;
        else
        {
            QByteArray bytes = file.read(4);
            for (int i = 0; i < 4; i++)
            {
                if (bytes[i] !="MThd"[i] )
                    vaildMidiFile = false;
            }
            file.close();
        }
        if (vaildMidiFile ==  true)
            m_settings->setValue("CurrentSong", fileInfo.absoluteFilePath());
        else
        {
            QMessageBox::warning(0, tr("PianoBooster Midi File Error"),
                 tr("\"%1\" is not a valid Midi file").arg(QString(fileInfo.absoluteFilePath())));
            exit(1);
        }
    }
}

void QtWindow::decodeCommandLine()
{
    bool hasMidiFile = false;
    QStringList argList = QCoreApplication::arguments();
    QString arg;
    for (int i = 0; i < argList.size(); ++i)
    {
        arg = argList[i];
        if (arg.startsWith("-"))
        {
            if (arg.startsWith("-d") || arg.startsWith("--debug"))
                Cfg::logLevel++;
            else if (arg.startsWith("-q") || arg.startsWith("--quick-start"))
                Cfg::quickStart = true;
            else if (arg.startsWith("--Xnote-length"))
                Cfg::experimentalNoteLength = true;
            else if (arg.startsWith("--Xtick-rate")) {
                if (validateIntegerParamWithMessage(arg)) {
                    Cfg::tickRate = decodeIntegerParam(arg, 12);
                }
            } else if (arg.startsWith("-l") || arg.startsWith("--log"))
                Cfg::useLogFile = true;
            else if (arg.startsWith("--midi-input-dump"))
                Cfg::midiInputDump = true;

            else if (arg.startsWith("-X1"))
                Cfg::experimentalTempo = true;
            else if (arg.startsWith("-Xswap"))
                Cfg::experimentalSwapInterval = decodeIntegerParam(arg, 100);

            else if (arg.startsWith("--lights"))
                Cfg::keyboardLightsChan = 1-1;  // Channel 1 (really a zero)

            else if (arg.startsWith("-h") || arg.startsWith("-?") || arg.startsWith("--help"))
            {
                displayUsage();
                exit(0);
            }
            else if (arg.startsWith("-v") || arg.startsWith("--version"))
            {
                fprintf(stdout, "pianobooster Version " PB_VERSION"\n");
                exit(0);
            }
            else
            {
                fprintf(stderr, "ERROR: Unknown arguments.\n");
                displayUsage();
                exit(0);
            }
        }
        else {
            if ( hasMidiFile == false && i > 0)
            {
                hasMidiFile = true;
                decodeMidiFileArg(arg);
            }
        }
    }
}

void QtWindow::addShortcutAction(const QString & key, const char * method)
{
    QAction* act = new QAction(this);
    act->setShortcut(m_settings->value(key).toString());
    connect(act, SIGNAL(triggered()), this, method);
    addAction(act);
}

void QtWindow::createActions()
{
    m_openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    m_openAct->setShortcut(tr("Ctrl+O"));
    m_openAct->setToolTip(tr("Open an existing file"));
    connect(m_openAct, SIGNAL(triggered()), this, SLOT(open()));

    m_exitAct = new QAction(tr("E&xit"), this);
    m_exitAct->setShortcut(tr("Ctrl+Q"));
    m_exitAct->setToolTip(tr("Exit the application"));
    connect(m_exitAct, SIGNAL(triggered()), this, SLOT(close()));

    m_aboutAct = new QAction(tr("&About"), this);
    m_aboutAct->setToolTip(tr("Show the application's About box"));
    connect(m_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    m_shortcutAct = new QAction(tr("&PC Shortcut Keys"), this);
    m_shortcutAct->setToolTip(tr("The PC Keyboard shortcut keys"));
    connect(m_shortcutAct, SIGNAL(triggered()), this, SLOT(keyboardShortcuts()));

    m_setupMidiAct = new QAction(tr("&Midi Setup ..."), this);
    m_setupMidiAct->setShortcut(tr("Ctrl+S"));
    m_setupMidiAct->setToolTip(tr("Setup the Midi input and output"));
    connect(m_setupMidiAct, SIGNAL(triggered()), this, SLOT(showMidiSetup()));

    m_setupKeyboardAct = new QAction(tr("Piano &Keyboard Setting ..."), this);
    m_setupKeyboardAct->setShortcut(tr("Ctrl+K"));
    m_setupKeyboardAct->setToolTip(tr("Change the piano keyboard settings"));
    connect(m_setupKeyboardAct, SIGNAL(triggered()), this, SLOT(showKeyboardSetup()));

    m_fullScreenStateAct = new QAction(tr("&Fullscreen"), this);
    m_fullScreenStateAct->setToolTip(tr("Fullscreen mode"));
    m_fullScreenStateAct->setShortcut(tr("F11"));
    m_fullScreenStateAct->setCheckable(true);
    connect(m_fullScreenStateAct, SIGNAL(triggered()), this, SLOT(onFullScreenStateAct()));

    m_sidePanelStateAct = new QAction(tr("&Show the Side Panel"), this);
    m_sidePanelStateAct->setToolTip(tr("Show the Left Side Panel"));
    m_sidePanelStateAct->setShortcut(tr("F12"));
    m_sidePanelStateAct->setCheckable(true);
    m_sidePanelStateAct->setChecked(true);
    connect(m_sidePanelStateAct, SIGNAL(triggered()), this, SLOT(toggleSidePanel()));

    m_viewPianoKeyboard = new QAction(tr("Show Piano &Keyboard"), this);
    m_viewPianoKeyboard->setToolTip(tr("Show Piano Keyboard Widget"));
    m_viewPianoKeyboard->setCheckable(true);
    m_viewPianoKeyboard->setChecked(false);
    if (m_settings->value("View/PianoKeyboard").toString()=="on"){
        m_viewPianoKeyboard->setChecked(true);
    }
    connect(m_viewPianoKeyboard, SIGNAL(triggered()), this, SLOT(onViewPianoKeyboard()));

    m_coloredNotes = new QAction(tr("Color Coded Notes"), this);
    m_coloredNotes->setToolTip(tr("Color Code Notes in Score"));
    m_coloredNotes->setCheckable(true);
    m_coloredNotes->setChecked(false);
    if (m_settings->value("View/ColoredNotes").toString()=="on"){
        m_coloredNotes->setChecked(true);
    }
    connect(m_coloredNotes, SIGNAL(triggered()), this, SLOT(onColoredNotes()));

    m_setupPreferencesAct = new QAction(tr("&Preferences ..."), this);
    m_setupPreferencesAct->setToolTip(tr("Settings"));
    m_setupPreferencesAct->setShortcut(tr("Ctrl+P"));
    connect(m_setupPreferencesAct, SIGNAL(triggered()), this, SLOT(showPreferencesDialog()));

    m_songDetailsAct = new QAction(tr("&Song Details ..."), this);
    m_songDetailsAct->setToolTip(tr("Song Settings"));
    m_songDetailsAct->setShortcut(tr("Ctrl+S"));
    connect(m_songDetailsAct, SIGNAL(triggered()), this, SLOT(showSongDetailsDialog()));

    QAction* act = new QAction(this);
    act->setShortcut(tr("Shift+F1"));
    connect(act, SIGNAL(triggered()), this, SLOT(enableFollowTempo()));
    addAction(act);

    act = new QAction(this);
    act->setShortcut(tr("Alt+F1"));
    connect(act, SIGNAL(triggered()), this, SLOT(disableFollowTempo()));
    addAction(act);

    addShortcutAction("ShortCuts/RightHand",        SLOT(on_rightHand()));
    addShortcutAction("ShortCuts/BothHands",        SLOT(on_bothHands()));
    addShortcutAction("ShortCuts/LeftHand",         SLOT(on_leftHand()));
    addShortcutAction("ShortCuts/PlayFromStart",    SLOT(on_playFromStart()));
    addShortcutAction("ShortCuts/PlayPause",        SLOT(on_playPause()));
    addShortcutAction("ShortCuts/Faster",           SLOT(on_faster()));
    addShortcutAction("ShortCuts/Slower",           SLOT(on_slower()));
    addShortcutAction("ShortCuts/NextSong",         SLOT(on_nextSong()));
    addShortcutAction("ShortCuts/PreviousSong",     SLOT(on_previousSong()));
    addShortcutAction("ShortCuts/NextBook",         SLOT(on_nextBook()));
    addShortcutAction("ShortCuts/PreviousBook",     SLOT(on_previousBook()));


     for (int i = 0; i < MAX_RECENT_FILES; ++i) {
         m_recentFileActs[i] = new QAction(this);
         m_recentFileActs[i]->setVisible(false);
         connect(m_recentFileActs[i], SIGNAL(triggered()),
                 this, SLOT(openRecentFile()));
     }


}

void QtWindow::createMenus()
{
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->setToolTipsVisible(true);
    m_fileMenu->addAction(m_openAct);
    m_separatorAct = m_fileMenu->addSeparator();
    for (int i = 0; i < MAX_RECENT_FILES; ++i)
       m_fileMenu->addAction(m_recentFileActs[i]);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAct);
    updateRecentFileActions();

    m_viewMenu = menuBar()->addMenu(tr("&View"));
    m_viewMenu->setToolTipsVisible(true);
    m_viewMenu->addAction(m_sidePanelStateAct);
    m_viewMenu->addAction(m_fullScreenStateAct);
    m_viewMenu->addAction(m_viewPianoKeyboard);
    m_viewMenu->addAction(m_coloredNotes);

    m_songMenu = menuBar()->addMenu(tr("&Song"));
    m_songMenu->setToolTipsVisible(true);
    m_songMenu->addAction(m_songDetailsAct);

    m_setupMenu = menuBar()->addMenu(tr("Set&up"));
    m_setupMenu->setToolTipsVisible(true);
    m_setupMenu->addAction(m_setupMidiAct);
    m_setupMenu->addAction(m_setupKeyboardAct);
    m_setupMenu->addAction(m_setupPreferencesAct);

    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    m_helpMenu->setToolTipsVisible(true);

    QAction* act;
    act = new QAction(tr("&Help"), this);
    act->setToolTip(tr("Piano Booster Help"));
    connect(act, SIGNAL(triggered()), this, SLOT(help()));
    m_helpMenu->addAction(act);

    act = new QAction(tr("&Website"), this);
    act->setToolTip(tr("Piano Booster Website"));
    connect(act, SIGNAL(triggered()), this, SLOT(website()));
    m_helpMenu->addAction(act);

    m_helpMenu->addAction(m_shortcutAct);
    m_helpMenu->addAction(m_aboutAct);
}
void QtWindow::openRecentFile()
 {
     QAction *action = qobject_cast<QAction *>(sender());
     if (action)
         m_settings->openSongFile(action->data().toString());
}

void QtWindow::showMidiSetup(){
    bool isPlaying=false;

    if(m_song->playingMusic()){
        isPlaying=true;
        m_topBar->on_playButton_clicked(true);
    }

    m_glWidget->stopTimerEvent();
    GuiMidiSetupDialog midiSetupDialog(this);
    midiSetupDialog.init(m_song, m_settings);
    midiSetupDialog.exec();

    m_glWidget->startTimerEvent();    
    if (isPlaying){
        m_topBar->on_playButton_clicked(true);
    }
}

// load the recent file list from the config file into the file menu
void QtWindow::updateRecentFileActions()
{

    QStringList files = m_settings->value("RecentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MAX_RECENT_FILES);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        if (m_recentFileActs[i] == 0)
            break;
        m_recentFileActs[i]->setText(text);
        m_recentFileActs[i]->setData(files[i]);
        m_recentFileActs[i]->setVisible(true);
    }

    for (int j = numRecentFiles; j < MAX_RECENT_FILES; ++j) {
        if (m_recentFileActs[j] == 0)
            break;
        m_recentFileActs[j]->setVisible(false);
    }

    if (m_separatorAct)
        m_separatorAct->setVisible(numRecentFiles > 0);
}

QString QtWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

// Just used for the RecentFileList
void QtWindow::setCurrentFile(const QString &fileName)
{

    setWindowFilePath(fileName);

    QStringList files = m_settings->value("RecentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MAX_RECENT_FILES)
        files.removeLast();

    m_settings->setValue("RecentFileList", files);

    updateRecentFileActions();

}

void QtWindow::website()
{
    QDesktopServices::openUrl(QUrl("https://github.com/captnfab/PianoBooster/tree/master/doc"));
}

void QtWindow::help()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle (tr("Piano Booster Help"));
    msgBox.setText(
            tr(
   "<h3>Getting Started</h3>"


   "<p>You need a <b>MIDI Piano Keyboard </b> and a <b>MIDI interface</b> for the PC. If you "
   "don't have a MIDI keyboard you can still try out PianoBooster using the PC keyboard, 'X' is "
   "middle C.</p>"

   "<p>To hear the music you will need a <b>General Midi sound synthesizer</b>. "
   "The \"Microsoft GS Wavetable software synthesizer\" that comes with Windows can be used "
   "but it introduces an unacceptable delay (latency). In Linux you can use "
   "<a href=\"http://www.fluidsynth.org\">FluidSynth</a> or <a href=\"http://timidity.sourceforge.net/\">Timidity</a></p>"


   "<p>PianoBooster works best with MIDI files that have separate left and right piano parts "
   "using MIDI channels 3 and 4."

   "<h3>Setting Up</h3>"


   "<p>First use the <i>Setup/Midi Setup</i> menu and in the dialog box select the MIDI input and MIDI "
   "output interfaces that match your hardware. "

   "Next use <i>File/Open</i> to open the MIDI file \".mid\" or a karaoke \".kar\" file. "
   "Now select whether you want to just <i>listen</i> to the music or "
   "<i>play along</i> on the piano keyboard by setting the <i>skill</i> level on the side panel. Finally when "
   "you are ready click the <i>play icon</i> (or press the <i>space bar</i>) to roll the music."

   "<h3>Hints on Playing the Piano</h3>"
   "<p>For hints on how to play the piano see: "
   "<a href=\"https://github.com/captnfab/PianoBooster/blob/master/doc/pianohints.md\" ><b>Piano Hints</b></a></p>"

   "<h3>More Information</h3>"
   "<p>For more help please visit the PianoBooster "
   "<a href=\"https://github.com/captnfab/PianoBooster/tree/master/doc\" ><b>website</b></a>, "
   "the PianoBooster <a href=\"https://github.com/captnfab/PianoBooster/blob/master/doc/faq.md\" ><b>FAQ</b></a> "
   "and the <a href=\"http://piano-booster.2625608.n2.nabble.com/Piano-Booster-Users-f1591936.html\" ><b>user forum</b></a>."

                ));
    msgBox.setMinimumWidth(600);
    msgBox.exec();
}

void QtWindow::about()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle (tr("About Piano Booster"));
    msgBox.setText(
            tr("<b>PianoBooster - Version %1</b> <br><br>").arg(PB_VERSION) +
            tr("<b>Boost</b> your <b>Piano</b> playing skills!<br><br>") +
            "<a href=\"https://github.com/captnfab/PianoBooster\" ><b>https://github.com/captnfab/PianoBooster</b></a><br><br>" +
            tr("Copyright(c) L. J. Barman, 2008-2013; All rights reserved.<br>") +
            tr("Copyright(c) Fabien Givors, 2018-2019; All rights reserved.<br>") +
            "<br>" +
            tr("This program is made available "
                "under the terms of the GNU General Public License version 3 as published by "
                "the Free Software Foundation.<br><br>"
            )
            #if !defined(USE_SYSTEM_RTMIDI)
             +
            tr("This program also contains RtMIDI: realtime MIDI i/o C++ classes<br>") +
            tr("Copyright(c) Gary P. Scavone, 2003-2019; All rights reserved.")
            #endif
    );
    msgBox.setMinimumWidth(600);
    msgBox.exec();
}

QString QtWindow::displayShortCut(QString key, QString description)
{
    QString space = tr("space");

    QString str = QString("<tr>"
                "<td>%1</td>"
                "<td>%2</td>"
                "</tr>").arg( description ).arg(tr(m_settings->value(key).toString().toUtf8().data()));
    return str;

}

void QtWindow::keyboardShortcuts()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle (tr("PC Keyboard ShortCuts"));
    QString msg =
            tr(
                "<h2><center>Keyboard shortcuts</center></h2>"
                "<p>The following PC keyboard shortcuts have been defined.</p>"
                "<center><table  border='1' cellspacing='0' cellpadding='4' >"
                );

    msg += tr(
                "<tr>"
                "<th>Action</th>"
                "<th>Key</th>"
                "</tr>"
            );
    msg += displayShortCut("ShortCuts/RightHand", tr("Choose the right hand"));
    msg += displayShortCut("ShortCuts/BothHands", tr("Choose both hands"));
    msg += displayShortCut("ShortCuts/LeftHand", tr("Choose the left Hand"));
    msg += displayShortCut("ShortCuts/PlayFromStart", tr("Play from start toggle"));
    msg += displayShortCut("ShortCuts/PlayPause", tr("Play Pause Toggle"));
    msg += displayShortCut("ShortCuts/Faster",  tr("Increase the speed by 5%"));
    msg += displayShortCut("ShortCuts/Slower", tr("Increase the speed by 5%"));
    msg += displayShortCut("ShortCuts/NextSong", tr("Change to the Next Song"));
    msg += displayShortCut("ShortCuts/PreviousSong", tr("Change to the Previous Song"));
    msg += displayShortCut("ShortCuts/NextBook", tr("Change to the Next Book"));
    msg += displayShortCut("ShortCuts/PreviousBook", tr("Change to the Previous Book"));

    msg += tr(
                "<tr><td>Fake Piano keys</td><td>X is middle C</td></tr>"
                "</table> </center><br>"
                );
    msgBox.setText(msg);

    msgBox.setMinimumWidth(600);
    msgBox.exec();
}


void QtWindow::open()
{
    m_glWidget->stopTimerEvent();

    QFileInfo currentSong = m_settings->getCurrentSongLongFileName();

    QString dir;
    if (currentSong.isFile())
        dir = currentSong.path();
    else
        dir = QDir::homePath();

    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Midi File"),
                            dir, tr("Midi Files") + " (*.mid *.MID *.midi *.MIDI *.kar *.KAR)");
    if (!fileName.isEmpty()) {

        m_settings->openSongFile(fileName);
        setCurrentFile(fileName);
    }
    m_glWidget->startTimerEvent();
}

void QtWindow::readSettings()
{
    QPoint pos = m_settings->value("Window/Pos", QPoint(25, 25)).toPoint();
    QSize size = m_settings->value("Window/Size", QSize(800, 600)).toSize();
    resize(size);
    move(pos);
}

void QtWindow::writeSettings()
{
    m_settings->setValue("Window/Pos", pos());
    m_settings->setValue("Window/Size", size());
    m_settings->writeSettings();
}

void QtWindow::closeEvent(QCloseEvent *event)
{
    if (m_song->playingMusic())
    {
        m_song->playMusic(false);
    }

    writeSettings();
}


void QtWindow::keyPressEvent ( QKeyEvent * event )
{
    if (event->text().length() == 0)
        return;

    if (event->isAutoRepeat() == true)
        return;

    if (event->key() == Qt::Key_F1)
        return;

    int c = event->text().toLatin1().at(0);
    m_song->pcKeyPress( c, true);
}

void QtWindow::keyReleaseEvent ( QKeyEvent * event )
{
    if (event->isAutoRepeat() == true)
        return;

    if (event->text().length() == 0)
        return;

    int c = event->text().toLatin1().at(0);
    m_song->pcKeyPress( c, false);
}

void QtWindow::loadTutorHtml(const QString & name)
{
    if (name.isEmpty())
    {
        m_tutorWindow->hide();
        m_tutorWindow->clear();
    }
    else
    {
        QFile file(name);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

        QTextStream out(&file);
        out.setCodec("UTF-8");

	QString text=out.readAll();
        m_tutorWindow->setHtml(tr(text.toUtf8().data()));
        m_tutorWindow->setFixedHeight(130);

        m_tutorWindow->show();

        file.close();
    }

}

void QtWindow::refreshTranslate(){
#ifndef NO_LANGS
    QString localeDirectory =
 #ifdef Q_OS_WIN32
        QApplication::applicationDirPath() + "/translations/";
 #endif
 #if defined (Q_OS_LINUX) || defined (Q_OS_UNIX)
        QApplication::applicationDirPath() + "/../share/games/" + QSTR_APPNAME + "/translations/";
 #endif
 #ifdef Q_OS_DARWIN
        QApplication::applicationDirPath() + "/../Resources/translations/";
 #endif

    QFile fileTestLocale(localeDirectory);
    if (!fileTestLocale.exists()){
        localeDirectory=QString(PREFIX)+"/"+QString(DATA_DIR)+"/translations/";
    }

    QString locale = m_settings->value("General/lang",QLocale::system().bcp47Name()).toString();

    qApp->removeTranslator(&translator);
    qApp->removeTranslator(&translatorMusic);
    qApp->removeTranslator(&qtTranslator);

    // save original
    if (listWidgetsRetranslateUi.size()==0){
        QList<QWidget*> l2 = this->findChildren<QWidget *>();
        for (auto &w:l2){
            QMap<QString,QString> m;
            m["toolTip"]=w->toolTip();
            m["whatsThis"]=w->whatsThis();
            m["windowTitle"]=w->windowTitle();
            m["statusTip"]=w->statusTip();
            listWidgetsRetranslateUi[w]=m;
        }

        QList<QAction*> l = this->findChildren<QAction *>();
        for (auto &w:l){
            QMap<QString,QString> m;
            m["toolTip"]=w->toolTip();
            m["whatsThis"]=w->whatsThis();
            m["statusTip"]=w->statusTip();
            m["text"]=w->text();
            listActionsRetranslateUi[w]=m;
        }

    }

    // set translator for app
    if (!translator.load(QSTR_APPNAME + QString("_") + locale , localeDirectory))
       if (!translator.load(QSTR_APPNAME + QString("_") + locale, QApplication::applicationDirPath()  + "/translations/"))
           translator.load(QSTR_APPNAME + QString("_") + locale, QApplication::applicationDirPath());
    qApp->installTranslator(&translator);

    // set translator for music
    if (!translatorMusic.load(QString("music_") + locale , localeDirectory))
       if (!translatorMusic.load(QString("music_") + locale, QApplication::applicationDirPath()  + "/translations/"))
           translatorMusic.load(QString("music_") + locale, QApplication::applicationDirPath());
    qApp->installTranslator(&translatorMusic);


    // set translator for default widget's text (for example: QMessageBox's buttons)
#ifdef __WIN32
    qtTranslator.load("qt_"+locale,localeDirectory);
#else
    qtTranslator.load("qt_"+locale,QLibraryInfo::location(QLibraryInfo::TranslationsPath));
#endif
    qApp->installTranslator(&qtTranslator);


    // retranslate UI
    QList<QWidget*> l2 = this->findChildren<QWidget *>();
    for (auto &w:l2){
        if (!w->toolTip().isEmpty()) w->setToolTip(tr(listWidgetsRetranslateUi[w]["toolTip"].toStdString().c_str()));
        if (!w->whatsThis().isEmpty()) w->setWhatsThis(tr(listWidgetsRetranslateUi[w]["whatsThis"].toStdString().c_str()));
        if (!w->windowTitle().isEmpty()) w->setWindowTitle(tr(listWidgetsRetranslateUi[w]["windowTitle"].toStdString().c_str()));
        if (!w->statusTip().isEmpty()) w->setStatusTip(tr(listWidgetsRetranslateUi[w]["statusTip"].toStdString().c_str()));
    }

    QList<QAction*> l = this->findChildren<QAction *>();
    for (auto &w:l){
        if (!w->toolTip().isEmpty()) w->setToolTip(tr(listActionsRetranslateUi[w]["toolTip"].toStdString().c_str()));
        if (!w->whatsThis().isEmpty()) w->setWhatsThis(tr(listActionsRetranslateUi[w]["whatsThis"].toStdString().c_str()));
        if (!w->statusTip().isEmpty()) w->setStatusTip(tr(listActionsRetranslateUi[w]["statusTip"].toStdString().c_str()));
        if (!w->text().isEmpty()) w->setText(tr(listActionsRetranslateUi[w]["text"].toStdString().c_str()));
    }

    m_sidePanel->updateTranslate();
    m_topBar->updateTranslate();
    m_settings->updateWarningMessages();
    m_settings->updateTutorPage();

#endif
}
