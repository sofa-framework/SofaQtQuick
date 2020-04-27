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

#ifndef SOFABASEAPPLICATION_H
#define SOFABASEAPPLICATION_H

#include <SofaQtQuickGUI/config.h>

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QPointF>
#include <QQuaternion>
#include <QProcess>
#include <QImage>
#include <QMap>
#include <QTimer>
class QApplication;
class QQmlApplicationEngine;
class QOpenGLDebugLogger;
class QQuickWindow;
class QQuickItem;
class QSettings;

#include <SofaQtQuickGUI/Bindings/SofaComponent.h>
#include <SofaQtQuickGUI/Bindings/SofaBase.h>
#include <SofaQtQuickGUI/Bindings/SofaData.h>
#include <SofaPython3/PythonEnvironment.h>
#include <sofa/core/DataTracker.h>

namespace sofaqtquick
{

using sofa::core::DataTracker;
using sofa::core::objectmodel::BaseData;
using sofaqtquick::bindings::SofaData;
using sofaqtquick::bindings::QmlDDGNode;


class ProcessState;
class SofaProject;

/// \class Useful tool when creating applications
class SOFA_SOFAQTQUICKGUI_API SofaBaseApplication : public QObject
{
    Q_OBJECT

//protected:
//    SofaBaseApplication(QObject* parent = nullptr);

public:
    SofaBaseApplication(QObject* parent = nullptr);
    ~SofaBaseApplication();

    static SofaBaseApplication* Instance();

public:
    Q_PROPERTY(sofaqtquick::bindings::SofaBase* selectedComponent READ getSelectedComponent WRITE setSelectedComponent NOTIFY selectedComponentChanged)
    Q_PROPERTY(int overrideCursorShape READ overrideCursorShape WRITE setOverrideCursorShape NOTIFY overrideCursorShapeChanged)
    Q_PROPERTY(sofaqtquick::SofaProject* currentProject READ getCurrentProject WRITE setCurrentProject NOTIFY currentProjectChanged)

public:
    int overrideCursorShape() const;
    void setOverrideCursorShape(int newCursorShape);

    sofaqtquick::SofaProject* getCurrentProject();
    void setCurrentProject(sofaqtquick::SofaProject* newProject);

signals:
    void overrideCursorShapeChanged();
    void signalComponent(QString path) ;
    void selectedComponentChanged(sofaqtquick::bindings::SofaBase* newSelectedComponent);
    void currentProjectChanged(sofaqtquick::SofaProject* newProject);

public:
    void setProjectDirectory(const std::string& dir);
    std::string getProjectDirectory();
    Q_SLOT void copyToClipboard(const QString& text);
    Q_SLOT void openInExplorer(const QString& folder) const;
    Q_SLOT void openInTerminal(const QString& folder) const;
    Q_SLOT void openInEditor(const QString& fullpath, const int line = 0) const;
    Q_SLOT void openInEditorFromUrl(const QUrl& fullpath, const int line = 0) const;
    Q_INVOKABLE QString createFolderIn(const QString& parent);

    Q_SLOT bool createFolder(const QString& destination);
    Q_SLOT bool removeFolder(const QString& destination);
	Q_SLOT bool copyFolder(const QString& source, const QString& destination);

	Q_INVOKABLE QStringList findFiles(const QString& dirPath, const QStringList& nameFilters);

    Q_INVOKABLE QString loadFile(const QString& filename);
    Q_SLOT bool saveFile(const QString& destination, const QString& data);
    Q_SLOT bool copyFile(const QString& source, const QString& destination);
    Q_INVOKABLE bool fileExists(const QString& filepath);

    Q_SLOT QImage screenshotComponent(QQuickItem* item, const QSize& forceSize = QSize()) const;
    Q_SLOT QImage screenshotComponent(QQmlComponent* component, const QSize& forceSize = QSize()) const; // \warning created object screenshot will be linked to the global context hence the component bindings (apart from the ones using the component context and the global context) will not be kept, prefer the version of screenshotComponent that need an item you created yourself if you need bindings with a parent context
    Q_SLOT QImage screenshotComponent(const QUrl& componentUrl, const QSize& forceSize = QSize()) const;

    Q_SLOT bool screenshotComponent(QQuickItem* item, const QString& destination, const QSize& forceSize = QSize()) const;
    Q_SLOT bool screenshotComponent(QQmlComponent* component, const QString& destination, const QSize& forceSize = QSize()) const; // \warning created object screenshot will be linked to the global context hence the component bindings (apart from the ones using the component context and the global context) will not be kept, prefer the version of screenshotComponent that need an item you created yourself if you need bindings with a parent context
    Q_SLOT bool screenshotComponent(const QUrl& componentUrl, const QString& destination, const QSize& forceSize = QSize()) const;

    Q_INVOKABLE bool runPythonScript(const QString& script); // \return true if script ran successfuly, false on error
    Q_INVOKABLE bool runPythonFile(const QString& filename); // \return true if script ran successfuly, false on error

    Q_INVOKABLE QVariantList executeProcess(const QString& command, int timeOutMsecs = -1); // \return [exit status (0 on success, 1 on crash), exit code, standard output, standard error]
    Q_INVOKABLE sofaqtquick::ProcessState* executeProcessAsync(const QString& command, const QStringList& args, const QString& workingDirectory);

	Q_INVOKABLE void addNextFrameAction(QJSValue& jsFunction); // \brief will execute the jsfunction (that must live that far) at the next frame

	Q_INVOKABLE QString pythonDirectory() const;
	Q_INVOKABLE void setPythonDirectory(const QString& pythonDirectory);

    Q_INVOKABLE QString dataDirectory() const;
    Q_INVOKABLE void setDataDirectory(const QString& dataDirectory);

    Q_INVOKABLE QString binaryDirectory() const;
    Q_INVOKABLE void saveScreenshot(const QString& path);

    Q_INVOKABLE QString templatesDirectory() const;
    Q_INVOKABLE QString inspectorsDirectory() const;
    Q_INVOKABLE QString assetsDirectory() const;

    Q_INVOKABLE bool createInspector(QString file);
    Q_INVOKABLE bool createAssetTemplate(QString file);

    Q_INVOKABLE QObject* instanciateURL(const QUrl& url);

    Q_INVOKABLE QPointF mapItemToScene(QQuickItem* item, const QPointF& point) const;
    Q_INVOKABLE bool isAncestorItem(QQuickItem* item, QQuickItem* ancestorItem) const;
    Q_INVOKABLE QQuickWindow* itemWindow(QQuickItem* item) const;
    Q_INVOKABLE void trimComponentCache(QObject* object = nullptr);
    Q_INVOKABLE void clearSettingGroup(const QString& group);
    Q_INVOKABLE int objectDepthFromRoot(QObject* object);

    Q_INVOKABLE QString toLocalFile(const QUrl& url);

    sofaqtquick::bindings::SofaBase* getSelectedComponent() const ;
    void setSelectedComponent(sofaqtquick::bindings::SofaBase* m_selectedComponent);

public:
    static void SetSelectedComponent(sofaqtquick::bindings::SofaBase* selectedComponent);
    static sofaqtquick::bindings::SofaBase* GetSelectedComponent();

    static void InitOpenGL();
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
    static bool DefaultMain(QApplication& app, QQmlApplicationEngine& applicationEngine , const QString &mainScript);


private:
    static SofaBaseApplication* OurInstance;

    SofaProject*            m_currentProject;

    QString					myPythonDirectory;
    QString                 myDataDirectory;

    sofa::core::objectmodel::Base::SPtr m_selectedComponent;

    QTimer m_viewUpdater;
    std::map<QmlDDGNode*, int> m_pendingUpdates;
public:
    static void updateAllDataView();
    static void requestDataViewUpdate(QmlDDGNode*);
    static void removePendingDataViewUpdate(QmlDDGNode*);
};

}

#endif // SOFAAPPLICATION_H
