/*
Copyright 2017, CNRS

This file is part of sofaqtquick.

sofaqtquick is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sofaqtquick is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.

Contributors:
    - damien.marchal@univ-lille1.fr
*/
#ifndef DEFAULTAPPLICATION_H
#define DEFAULTAPPLICATION_H

#include "SofaQtQuickGUI.h"
#include "SofaApplication.h"

class QApplication;
class QQmlApplicationEngine;
class QOpenGLDebugLogger;
class QQuickWindow;
class QQuickItem;
class QSettings;

namespace sofa
{
namespace qtquick
{

/// \class Useful tool when creating applications
class SOFA_SOFAQTQUICKGUI_API DefaultApplication  : public SofaApplication
{
public:
    Q_OBJECT

public:
    DefaultApplication(QObject* parent=0) ;
    virtual ~DefaultApplication() ;

    void setOpenGLDebugContext() override;    // must be call before the window has been shown
    void useOpenGLDebugLogger() override;     // must be call after a valid opengl debug context has been made current

    void useDefaultSofaPath() override;
    void applySettings(const QString& defaultSettingsPath = QString(),
                       const QString& backupSettingsPath = QString()) override;
    void finalizeSettings() override ;
    void makeBackupSettings(const QString& backupSettingsPath = QString()) override;
    void applyBackupSettings(const QString& backupSettingsPath = QString()) override;
    void applyDefaultSettings(const QString& defaultSettingsPath = QString(),
                              const QString& backupSettingsPath = QString()) override;
    void copySettings(const QSettings& src, QSettings& dst) override;

    /// Centralized prerequisite for most sofaqtquick applications.
    /// Every applications will be updated when modifying this code.
    /// IMPORTANT NOTE: To be called in the main function BEFORE the QApplication object creation
    bool surfaceDefinition() override;
    void instanciate(QQmlApplicationEngine& applicationEngine) override;
    void destruction() override;
    bool mainInitialization(QApplication& app,
                            QQmlApplicationEngine &applicationEngine,
                            const QString& mainScript) override;

};

}

}

#endif // DEFAULTAPPLICATION_H
