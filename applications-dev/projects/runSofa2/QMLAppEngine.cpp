#include "QMLAppEngine.h"

#include <iostream>

// Keep the snippet simple!
static QString MyQmlApplicationEngine_mainQml;
static QList<QQmlError> MyQmlApplicationEngine_warnings;

MyQmlApplicationEngine::MyQmlApplicationEngine(const QString &mainQml)
{
    MyQmlApplicationEngine_mainQml = mainQml;

    // Would be nice to have this in QQmlApplicationEngine
    connect(this, &QQmlEngine::warnings, [](const QList<QQmlError> &warnings) {
        MyQmlApplicationEngine_warnings.append(warnings);
    });

    QStringList colors;
    colors.append(QStringLiteral("red"));
    colors.append(QStringLiteral("green"));
    colors.append(QStringLiteral("blue"));
    colors.append(QStringLiteral("black"));
    rootContext()->setContextProperty("myColors", colors);
}

QString MyQmlApplicationEngine::mainQml() const
{
    return MyQmlApplicationEngine_mainQml;
}

QQuickWindow *MyQmlApplicationEngine::mainWindow()
{
    if (rootObjects().isEmpty())
        return nullptr;

    return qobject_cast<QQuickWindow *>(rootObjects().first());
}

QList<QQmlError> MyQmlApplicationEngine::warnings() const
{
    return MyQmlApplicationEngine_warnings;
}
