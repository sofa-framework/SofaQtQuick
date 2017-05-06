/*
Copyright 2015, Anatoscope

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
*/

#ifndef SOFAAPPLICATION_H
#define SOFAAPPLICATION_H

#include "SofaQtQuickGUI.h"

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QPointF>
#include <QQuaternion>
#include <QProcess>

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

class SofaApplication : public QObject
{
public:
    Q_OBJECT

public:
    SofaApplication(QObject* parent=0) : QObject(parent){}
    virtual ~SofaApplication(){}

    virtual void setOpenGLDebugContext() = 0 ;    // must be call before the window has been shown
    virtual void useOpenGLDebugLogger() = 0 ;     // must be call after a valid opengl debug context has been made current

    virtual void useDefaultSofaPath() = 0 ;
    virtual void applySettings(const QString& defaultSettingsPath = QString(),
                       const QString& backupSettingsPath = QString()) = 0 ;
    virtual void finalizeSettings() = 0 ;
    virtual void makeBackupSettings(const QString& backupSettingsPath = QString()) = 0 ;
    virtual void applyBackupSettings(const QString& backupSettingsPath = QString()) = 0 ;
    virtual void applyDefaultSettings(const QString& defaultSettingsPath = QString(),
                              const QString& backupSettingsPath = QString()) = 0 ;
    virtual void copySettings(const QSettings& src, QSettings& dst) = 0 ;

    /// Centralized prerequisite for most sofaqtquick applications.
    /// Every applications will be updated when modifying this code.
    /// IMPORTANT NOTE: To be called in the main function BEFORE the QApplication object creation
    virtual bool surfaceDefinition() = 0  ;
    virtual void instanciate(QQmlApplicationEngine& applicationEngine) = 0 ;
    virtual void destruction() = 0  ;
    virtual bool mainInitialization(QApplication& app,
                                    QQmlApplicationEngine &applicationEngine,
                                     const QString& mainScript) = 0 ;
};

class ProcessState;

/// \class Useful tool when creating applications
class SOFA_SOFAQTQUICKGUI_API MainApplication : public QObject
{
    Q_OBJECT

protected:
    MainApplication(QObject* parent = 0);

public:
    ~MainApplication();

public:
    Q_PROPERTY(int overrideCursorShape READ overrideCursorShape WRITE setOverrideCursorShape NOTIFY overrideCursorShapeChanged)

public:
    int overrideCursorShape() const;
    void setOverrideCursorShape(int newCursorShape);

signals:
    void overrideCursorShapeChanged();

public:
    Q_SLOT void copyToClipboard(const QString& text);
    Q_SLOT void openInExplorer(const QString& folder) const;

    Q_SLOT bool createFolder(const QString& destination);
    Q_SLOT bool removeFolder(const QString& destination);
    Q_SLOT bool copyFolder(const QString& source, const QString& destination);

    Q_INVOKABLE QStringList findFiles(const QString& dirPath, const QStringList& nameFilters);

    Q_INVOKABLE QString loadFile(const QString& filename);
    Q_SLOT bool saveFile(const QString& destination, const QString& data);
    Q_SLOT bool copyFile(const QString& source, const QString& destination);

    Q_SLOT bool screenshotComponent(const QUrl& componentUrl, const QString& destination);

    Q_INVOKABLE bool runPythonScript(const QString& script); // \return true if script ran successfuly, false on error
    Q_INVOKABLE bool runPythonFile(const QString& filename); // \return true if script ran successfuly, false on error

    Q_INVOKABLE QVariantList executeProcess(const QString& command, int timeOutMsecs = -1); // \return [exit status (0 on success, 1 on crash), exit code, standard output, standard error]
    Q_INVOKABLE sofa::qtquick::ProcessState* executeProcessAsync(const QString& command);

    Q_INVOKABLE void addNextFrameAction(QJSValue& jsFunction); // \brief will execute the jsfunction (that must live that far) at the next frame

    Q_INVOKABLE QString pythonDirectory() const;
    Q_INVOKABLE void setPythonDirectory(const QString& pythonDirectory);

    Q_INVOKABLE QString dataDirectory() const;
    Q_INVOKABLE void setDataDirectory(const QString& dataDirectory);

    Q_INVOKABLE QString binaryDirectory() const;
    Q_INVOKABLE void saveScreenshot(const QString& path);

    Q_INVOKABLE QObject* instanciateURL(const QUrl& url);

    Q_INVOKABLE QPointF mapItemToScene(QQuickItem* item, const QPointF& point) const;
    Q_INVOKABLE bool isAncestorItem(QQuickItem* item, QQuickItem* ancestorItem) const;
    Q_INVOKABLE QQuickWindow* itemWindow(QQuickItem* item) const;
    Q_INVOKABLE void trimComponentCache(QObject* object = 0);
    Q_INVOKABLE void clearSettingGroup(const QString& group);
    Q_INVOKABLE int objectDepthFromRoot(QObject* object);

    Q_INVOKABLE QString toLocalFile(const QUrl& url);

    Q_INVOKABLE QQuaternion quaternionFromEulerAngles(const QVector3D& eulerAngles) const;
    Q_INVOKABLE QVector3D quaternionToEulerAngles(const QQuaternion& quaternion) const;
    Q_INVOKABLE QVariantList quaternionToAxisAngle(const QQuaternion& quaternion) const; // return [QVector3D axis, float angle];
    Q_INVOKABLE QQuaternion quaternionFromAxisAngle(const QVector3D& axis, float angle) const;
    Q_INVOKABLE QQuaternion quaternionDifference(const QQuaternion& q0, const QQuaternion& q1) const;
    Q_INVOKABLE QQuaternion quaternionMultiply(const QQuaternion& q0, const QQuaternion& q1) const;
    Q_INVOKABLE QQuaternion quaternionConjugate(const QQuaternion& q) const;
    Q_INVOKABLE QVector3D quaternionRotate(const QQuaternion& q, const QVector3D& v) const;
    Q_INVOKABLE QVariantList quaternionSwingTwistDecomposition(const QQuaternion& q, const QVector3D& direction);

public:
    static void SetOpenGLDebugContext();    // must be call before the window has been shown
    static void UseOpenGLDebugLogger();     // must be call after a valid opengl debug context has been made current

    static void UseDefaultSofaPath();
    static void ApplySettings(const QString& defaultSettingsPath = QString(), const QString& backupSettingsPath = QString());
    static void FinalizeSettings();
    static void MakeBackupSettings(const QString& backupSettingsPath = QString());
    static void ApplyBackupSettings(const QString& backupSettingsPath = QString());
    static void ApplyDefaultSettings(const QString& defaultSettingsPath = QString(), const QString& backupSettingsPath = QString());
    static void CopySettings(const QSettings& src, QSettings& dst);

    /// Centralized prerequisite for most sofaqtquick applications.
    /// Every applications will be updated when modifying this code.
    /// IMPORTANT NOTE: To be called in the main function BEFORE the QApplication object creation
    static bool Initialization();
    static void Instanciate(QQmlApplicationEngine& applicationEngine);
    static void Destruction();

    /// Centralized common settings for most sofaqtquick applications.
    /// Every applications will be updated when modifying this code.
    /// To be called in the main function.
    static bool MainInitialization(QApplication& app, QQmlApplicationEngine& applicationEngine , const QString &mainScript);


    static void setApplicationSingleton(SofaApplication *app) ;
    static SofaApplication* getApplicationSingleton() ;
private:
    static SofaApplication* m_maininstance;


    QString					myPythonDirectory;
    QString                 myDataDirectory;
};

}

}

#endif // SOFAAPPLICATION_H
