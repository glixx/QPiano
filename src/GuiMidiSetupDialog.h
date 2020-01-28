/*********************************************************************************/
/*!
@file           GuiMidiSetupDialog.h

@brief          xxxx.

@author         L. J. Barman

    Copyright (c)   2008-2013, L. J. Barman, all rights reserved

    This file is part of the QPiano application

    QPiano is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QPiano is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QPiano.  If not, see <http://www.gnu.org/licenses/>.

*/
/*********************************************************************************/

#ifndef __GUIMIDISETUPDIALOG_H__
#define __GUIMIDISETUPDIALOG_H__

#include <QtWidgets>

#include "Song.h"
#include "Settings.h"

#include "ui_GuiMidiSetupDialog.h"

class GuiMidiSetupDialog : public QDialog, private Ui::GuiMidiSettingsDialog
{
    Q_OBJECT

public:
    GuiMidiSetupDialog(QWidget *parent = 0);

    void init(CSong* song, CSettings* settings);

private slots:
    void accept();
    void on_midiInputCombo_activated (int index);
    void on_midiOutputCombo_activated (int index);
    void on_latencyFixButton_clicked ( bool checked );
    void on_fluidAddButton_clicked ( bool checked );
    void on_fluidRemoveButton_clicked ( bool checked );
    void on_audioDriverCombo_currentIndexChanged ( int index );
    void on_enableFluidSynth_stateChanged(int status);
    void on_enableTimidity_stateChanged(int status);

private:

    void updateMidiInfoText();
    void updateFluidInfoText();
    void setDefaultFluidSynth();
    CSettings* m_settings;
    CSong* m_song;
    int m_latencyFix;
    bool m_latencyChanged;
    bool m_midiChanged;
};

#endif //__GUIMIDISETUPDIALOG_H__
