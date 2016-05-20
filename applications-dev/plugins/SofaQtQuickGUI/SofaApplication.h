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

/// \class Represents the whole application
class SOFA_SOFAQTQUICKGUI_API SofaApplication : public QObject
{
    Q_OBJECT

protected:
    SofaApplication(QObject* parent = 0);

public:
    ~SofaApplication();

    static SofaApplication* Instance();

public:
    Q_PROPERTY(int overrideCursorShape READ overrideCursorShape WRITE setOverrideCursorShape NOTIFY overrideCursorShapeChanged)

public:
    int overrideCursorShape() const;
    void setOverrideCursorShape(int newCursorShape);

signals:
    void overrideCursorShapeChanged();

public:
    Q_INVOKABLE QString binaryDirectory() const;
    Q_INVOKABLE void saveScreenshot(const QString& path);

    Q_INVOKABLE QObject* instanciateURL(const QUrl& url);

    Q_INVOKABLE QPointF mapItemToScene(QQuickItem* item, const QPointF& point) const;
    Q_INVOKABLE bool isAncestorItem(QQuickItem* item, QQuickItem* ancestorItem) const;
    Q_INVOKABLE QQuickWindow* itemWindow(QQuickItem* item) const;
    Q_INVOKABLE void trimComponentCache(QObject* object = 0);
    Q_INVOKABLE void clearSettingGroup(const QString& group);

    Q_INVOKABLE QQuaternion quaternionFromEulerAngles(const QVector3D& eulerAngles) const;
    Q_INVOKABLE QVector3D quaternionToEulerAngles(const QQuaternion& quaternion) const;
    Q_INVOKABLE QVariantList quaternionToAxisAngle(const QQuaternion& quaternion) const; // return [QVector3D axis, float angle];
    Q_INVOKABLE QQuaternion quaternionFromAxisAngle(const QVector3D& axis, float angle) const;
    Q_INVOKABLE QQuaternion quaternionDifference(const QQuaternion& q0, const QQuaternion& q1) const;
    Q_INVOKABLE QQuaternion quaternionMultiply(const QQuaternion& q0, const QQuaternion& q1) const;
    Q_INVOKABLE QQuaternion quaternionConjugate(const QQuaternion& q) const;
    Q_INVOKABLE QVector3D quaternionMap(const QQuaternion& q, const QVector3D& v) const;

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

    static void Destruction();

    /// Centralized common settings for most sofaqtquick applications.
    /// Every applications will be updated when modifying this code.
    /// To be called in the main function.
    static bool DefaultMain(QApplication& app, QQmlApplicationEngine& applicationEngine , const QString &mainScript);

private:
    static SofaApplication* OurInstance;

};

}

}

#endif // SOFAAPPLICATION_H
